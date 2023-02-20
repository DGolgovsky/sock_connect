#include <utility>
#include <map>
#include <memory>
#include <algorithm>

#include "include/connection.h"

/**
 * Transform <const char*> IP-address to uint32_t
 * "127.0.0.1" -> 0x7f000001
 */
static unsigned int ip_to_int(std::string const &str)
{
#ifndef NDEBUG
    debug_mutex.lock();
    std::clog << "[SOCK_CONNECT] connection_::ip_to_int("
              << "const char *ip: " << str << ")\n" << std::flush;
    debug_mutex.unlock();
#endif
    uint32_t ip = 0;
    uint32_t part = 0;
    uint32_t part_count = 0;

    for (const auto &c: str)
    {
        if (std::isdigit(c))
        {
            part = (part * 10) + (static_cast<unsigned int>(c - '0'));
        }
        else if (c == '.')
        {
            if (++part_count == 4)
            {
                break;
            }
            ip = (ip << 8) + part;
            part = 0;
        }
        else
        {
            break;
        }
    }
    ip = (ip << 8) + part;
    if (++part_count < 4)
    {
        ip = ip << ((4 - part_count) << 3);
    }

    return ip;
}

connection::connection(conn_type cp, uint32_t address, uint16_t port)
    : socket_(cp)
    , address_(address)
    , port_(port)
{
    conn_memset();
    socket_addr_.sin_family = AF_INET;
    socket_addr_.sin_addr.s_addr = htonl(address_);
    socket_addr_.sin_port = htons(port_);
    ptr_addr_ = reinterpret_cast<sockaddr *>(&socket_addr_);
    size_addr_ = sizeof(socket_addr_);
    client_list_ = new storage_t(32);
    timeval set = {60, 0};
    int reuse = 1;
    setsockopt(socket_.id(), SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    setsockopt(socket_.id(), SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(reuse));
    setsockopt(socket_.id(), SOL_SOCKET, SO_SNDTIMEO, reinterpret_cast<char *>(&set), sizeof(set));

#ifndef NDEBUG
    debug_mutex.lock();
    std::clog << "[SOCK_CONNECT] connection_::connection_<" << socket_.c_type() << ">("
              << type_name<decltype(address)>() << " address: " << address << ", "
              << type_name<decltype(port)>() << " port: " << port << ") ["
              << "SOCK_ID = " << socket_.id() << "]\n" << std::flush;
    debug_mutex.unlock();
#endif
    this->state_ = true;
}

connection::connection(conn_type cp, char const *address, uint16_t port)
    : connection(cp, ip_to_int(address), port)
{
}

connection::connection(conn_type cp, std::string const &address, uint16_t port)
    : connection(cp, ip_to_int(address), port)
{
}

connection::connection(conn_type cp, const char &&address, uint16_t port)
    : connection(cp, ip_to_int(&address), port)
{
}

connection::connection(conn_type cp, std::string const &socket_path)
    : socket_(cp)
    , m_path_(socket_path)
{
    conn_memset();
    unix_addr_.sun_family = AF_UNIX;
    strcpy(unix_addr_.sun_path, socket_path.c_str());
    ptr_addr_ = reinterpret_cast<sockaddr *>(&unix_addr_);
    size_addr_ = sizeof(unix_addr_);
    client_list_ = new storage_t(32);
#ifndef NDEBUG
    debug_mutex.lock();
    std::clog << "[SOCK_CONNECT] connection_::connection_<" << socket_.c_type() << ">("
              << "std::string socket_path: " << socket_path << ")" << '\n' << std::flush;
    debug_mutex.unlock();
#endif
    this->state_ = true;
}

connection::connection(conn_type cp, char const *path)
    : connection(cp, static_cast<std::string>(path))
{
}

void connection::conn_memset()
{
    memset(&socket_addr_, '\0', sizeof(socket_addr_));
    memset(&client_addr_, '\0', sizeof(client_addr_));
    memset(&unix_addr_, '\0', sizeof(unix_addr_));
}

connection::~connection()
{
#ifndef NDEBUG
    debug_mutex.lock();
    std::clog << "[SOCK_CONNECT] connection_::connection_<" << socket_.c_type() << ">()\n" << std::flush;
    debug_mutex.unlock();
#endif
    this->shutdown(0);
    delete client_list_;
}

void connection::shutdown(int id)
{
    if (!id)
    {
        for (auto it: *client_list_)
        {
            ::shutdown(it.second, SHUT_RDWR);
        }
        id = this->get_descriptor();
        this->state_ = false;
    }

    if (::shutdown(id, SHUT_RDWR) < 0)
    {
        if (close(id) < 0)
        {
            throw std::runtime_error("[SOCK_CONNECT] shutdown failed, error number: "
                                     + std::to_string(errno));
        }
    }
    else
    {
#ifndef NDEBUG
        debug_mutex.lock();
        std::clog << "[SOCK_CONNECT] connection_::shutdown<" << socket_.c_type() << ">(): " << id << '\n'
                  << std::flush;
        debug_mutex.unlock();
#endif
    }
    if (socket_.c_type() == "sun")
    {
        unlink(m_path_.c_str());
#ifndef NDEBUG
        debug_mutex.lock();
        std::clog << "[SOCK_CONNECT] connection_::shutdown<" << socket_.c_type() << ">(): " << m_path_ << '\n'
                  << std::flush;
        debug_mutex.unlock();
#endif
        this->state_ = false;
    }
}

bool connection::bind(bool listen) const
{
    if (::bind(socket_.id(), ptr_addr_, size_addr_) < 0)
    {
        close(socket_.id());
        throw std::runtime_error(
            "[SOCK_CONNECT] " + socket_.c_type() + " bind failed, error number: "
            + std::to_string(errno));
    }
#ifndef NDEBUG
    if (socket_.c_type() != "sun")
    {
        debug_mutex.lock();
        std::clog << "[SOCK_CONNECT] " << socket_.c_type() << "::bind(" << inet_ntoa(socket_addr_.sin_addr)
                  << ":" << ntohs(socket_addr_.sin_port) << ")\n" << std::flush;
        debug_mutex.unlock();
    }
#endif
    if (listen)
    {
        return this->listen();
    }

    return true;
}

bool connection::listen() const
{
    if (socket_.c_type() == "UDP")
    {
        return true;
    }
    if (::listen(socket_.id(), SOMAXCONN) < 0)
    {
        throw std::runtime_error(
            "[SOCK_CONNECT] " + socket_.c_type() + " listen failed, error number: "
            + std::to_string(errno));
    }
#ifndef NDEBUG
    if (socket_.c_type() != "sun")
    {
        debug_mutex.lock();
        std::clog << "[SOCK_CONNECT] " << socket_.c_type() << "::listen(" << inet_ntoa(socket_addr_.sin_addr)
                  << ":" << ntohs(socket_addr_.sin_port) << ")\n" << std::flush;
        debug_mutex.unlock();
    }
#endif

    return true;
}

int connection::accept(std::string *client_address)
{
    socklen_t sz = sizeof(client_addr_);
    int transmission = ::accept(socket_.id(), reinterpret_cast<sockaddr *>(&client_addr_), &sz);
    if (transmission < 0)
    {
        throw std::runtime_error(
            "[SOCK_CONNECT] " + socket_.c_type() + " accept failed, error number: " + std::to_string(errno));
    }
#ifndef NDEBUG
    debug_mutex.lock();
    std::clog << "[SOCK_CONNECT] " << socket_.c_type() << "::accept("
              << (socket_.c_type() != "sun" ? inet_ntoa(client_addr_.sin_addr) : this->m_path_)
              << ":" << ntohs(client_addr_.sin_port) << ") | fd_: " << transmission << '\n' << std::flush;
    debug_mutex.unlock();
#endif
    if (client_address)
    {
        client_address->append((socket_.c_type() != "sun" ? inet_ntoa(client_addr_.sin_addr) : this->m_path_));
    }
    // Must be called also at dev-side
    this->assign_thread(transmission);
    return transmission;
}

bool connection::connect()
{
    if (::connect(socket_.id(), ptr_addr_, sizeof(*ptr_addr_)) < 0)
    {
        return false;
    }
    timeval set = {60, 0};
    setsockopt(socket_.id(), SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<char *>(&set), sizeof(set));
#ifndef NDEBUG
    debug_mutex.lock();
    std::clog << "[SOCK_CONNECT] " << socket_.c_type() << "::connect("
              << (socket_.c_type() != "sun" ? inet_ntoa(socket_addr_.sin_addr) : this->m_path_)
              << ":" << ntohs(socket_addr_.sin_port) << ")\n" << std::flush;
    debug_mutex.unlock();
#endif
    return this->state_ = true;
}

int connection::id() const noexcept
{
    return socket_.id();
}

bool connection::status() const noexcept
{
    return state_;
}

void connection::assign_thread(int id)
{
    auto thread_id = std::this_thread::get_id();
    auto it = std::find_if(client_list_->begin(), client_list_->end(),
                           [&thread_id](const std::pair<std::thread::id, int> &element)
                           {
                               return element.first == thread_id;
                           });
    if (it != client_list_->end())
    {
        it->second = id;
    }
    else
    {
        client_list_->emplace_back(std::this_thread::get_id(), id);
    }
    //client_list_[std::this_thread::get_id()] = id;
}

int connection::get_descriptor()
{
    auto thread_id = std::this_thread::get_id();
    auto it = std::find_if(client_list_->begin(), client_list_->end(),
                           [&thread_id](const std::pair<std::thread::id, int> &element)
                           {
                               return element.first == thread_id;
                           });
    if (it != client_list_->end())
    {
        return it->second;
    }
    else
    {
        return this->id();
    }
}
