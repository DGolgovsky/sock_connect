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
#ifndef NDEBUG
    debug_mutex.lock();
    std::clog << "[SOCK_CONNECT] ip_to_int(" << "string ip: " << str << ") = "
              << ip << '\n' << std::flush;
    debug_mutex.unlock();
#endif
    return ip;
}

connection::connection(conn_type cp, uint32_t address, uint16_t port)
    : socket_(cp)
    , ip_address_(address)
    , ip_port_(port)
{
    conn_memset();
    self_socket_.sin_family = AF_INET;
    self_socket_.sin_addr.s_addr = htonl(ip_address_);
    self_socket_.sin_port = htons(ip_port_);

    addr_ptr_ = reinterpret_cast<sockaddr *>(&self_socket_);
    addr_size_ = sizeof(self_socket_);

    client_list_ = new storage_t(32);
    timeval set = {60, 0};
    int reuse = 1;
    setsockopt(socket_, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    setsockopt(socket_, SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(reuse));
    setsockopt(socket_, SOL_SOCKET, SO_SNDTIMEO,
               reinterpret_cast<char *>(&set), sizeof(set));
#ifndef NDEBUG
    debug_mutex.lock();
    std::clog << "[SOCK_CONNECT] connection::connection<"
              << socket_.connection_type() << ">("
              << type_name<decltype(address)>() << " address: " << address
              << ", "
              << type_name<decltype(port)>() << " port: " << port << ") ["
              << "SOCKET_ID = " << socket_.id() << "]\n" << std::flush;
    debug_mutex.unlock();
#endif
    this->state_ = true;
}

connection::connection(conn_type cp, char const *address, uint16_t port)
    : connection(cp, ip_to_int(address), port)
{}

connection::connection(conn_type cp, std::string const &address, uint16_t port)
    : connection(cp, ip_to_int(address), port)
{}

connection::connection(conn_type cp, const char &&address, uint16_t port)
    : connection(cp, ip_to_int(&address), port)
{}

connection::connection(conn_type cp, std::string const &sun_path)
    : socket_(cp)
    , sun_path_(sun_path)
{
    conn_memset();
    unix_socket_.sun_family = AF_UNIX;
    strcpy(unix_socket_.sun_path, sun_path_.c_str());

    addr_ptr_ = reinterpret_cast<sockaddr *>(&unix_socket_);
    addr_size_ = sizeof(unix_socket_);
    client_list_ = new storage_t(32);
#ifndef NDEBUG
    debug_mutex.lock();
    std::clog << "[SOCK_CONNECT] connection::connection<"
              << socket_.connection_type() << ">(" << "std::string sun_path: "
              << sun_path << ")" << '\n' << std::flush;
    debug_mutex.unlock();
#endif
    this->state_ = true;
}

connection::connection(conn_type cp, char const *path)
    : connection(cp, static_cast<std::string>(path))
{}

void connection::conn_memset()
{
    memset(&self_socket_, '\0', sizeof(self_socket_));
    memset(&client_socket_, '\0', sizeof(client_socket_));
    memset(&unix_socket_, '\0', sizeof(unix_socket_));
}

connection::~connection()
{
#ifndef NDEBUG
    debug_mutex.lock();
    std::clog << "[SOCK_CONNECT] connection::~connection<"
              << socket_.connection_type() << ">()\n" << std::flush;
    debug_mutex.unlock();
#endif
    try
    {
        shutdown(0);
    }
    catch (std::exception const &e)
    {
#ifndef NDEBUG
        debug_mutex.lock();
        std::clog << e.what() << '\n' << std::flush;
        debug_mutex.unlock();
#endif
    }
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
            throw std::runtime_error(
                "[SOCK_CONNECT] Shutdown socket: " + std::to_string(id) +
                " failed. Error number: " + std::to_string(errno));
        }
    }
    else
    {
#ifndef NDEBUG
        debug_mutex.lock();
        std::clog << "[SOCK_CONNECT] connection::shutdown<"
                  << socket_.connection_type() << ">(): " << id << '\n'
                  << std::flush;
        debug_mutex.unlock();
#endif
    }
    if (socket_.connection_type() == "sun")
    {
        unlink(sun_path_.c_str());
#ifndef NDEBUG
        debug_mutex.lock();
        std::clog << "[SOCK_CONNECT] connection::shutdown<"
                  << socket_.connection_type() << ">(): " << sun_path_ << '\n'
                  << std::flush;
        debug_mutex.unlock();
#endif
        this->state_ = false;
    }
}

void connection::bind(bool listen) const
{
    if (::bind(socket_, addr_ptr_, addr_size_) < 0)
    {
        close(socket_);
        throw std::runtime_error(
            "[SOCK_CONNECT] " + socket_.connection_type() +
            "::bind failed. Error number: " + std::to_string(errno));
    }
#ifndef NDEBUG
    std::string bind_addr;
    if (socket_.connection_type() != "sun")
    {
        bind_addr = inet_ntoa(self_socket_.sin_addr);
        bind_addr.append(":")
                 .append(std::to_string(ntohs(self_socket_.sin_port)));
    }
    else
    {
        bind_addr = this->sun_path_;
    }
    debug_mutex.lock();
    std::clog << "[SOCK_CONNECT] " << socket_.connection_type() << "::bind("
              << bind_addr << ")\n" << std::flush;
    debug_mutex.unlock();
#endif
    if (listen && socket_.connection_type() != "udp")
    {
        this->listen();
    }
}

void connection::listen() const
{
    if (::listen(socket_, SOMAXCONN) < 0)
    {
        throw std::runtime_error(
            "[SOCK_CONNECT] " + socket_.connection_type() +
            "::listen failed, error number: " + std::to_string(errno));
    }
#ifndef NDEBUG
    std::string bind_addr;
    if (socket_.connection_type() == "tcp")
    {
        bind_addr = inet_ntoa(self_socket_.sin_addr);
        bind_addr.append(":")
                 .append(std::to_string(ntohs(self_socket_.sin_port)));
    }
    else
    {
        bind_addr = this->sun_path_;
    }
    debug_mutex.lock();
    std::clog << "[SOCK_CONNECT] " << socket_.connection_type() << "::listen("
              << bind_addr << ")\n" << std::flush;
    debug_mutex.unlock();
#endif
}

int connection::accept(std::string *client_address)
{
    socklen_t sz = sizeof(client_socket_);
    int transmission = ::accept(socket_,
                                reinterpret_cast<sockaddr *>(&client_socket_),
                                &sz);
    if (transmission < 0)
    {
        throw std::runtime_error(
            "[SOCK_CONNECT] " + socket_.connection_type() +
            "::accept failed. Error number: " + std::to_string(errno));
    }
#ifndef NDEBUG
    debug_mutex.lock();
    std::clog << "[SOCK_CONNECT] " << socket_.connection_type() << "::accept("
              << (socket_.connection_type() != "sun" ? inet_ntoa(
                  client_socket_.sin_addr) : this->sun_path_)
              << ":" << ntohs(client_socket_.sin_port) << ") | fd: "
              << transmission << '\n' << std::flush;
    debug_mutex.unlock();
#endif
    if (client_address)
    {
        client_address->append((socket_.connection_type() != "sun" ? inet_ntoa(
            client_socket_.sin_addr) : this->sun_path_));
    }
    // Must be called also at dev-side
    this->assign_thread(transmission);
    return transmission;
}

bool connection::connect()
{
    if (::connect(socket_, addr_ptr_, sizeof(*addr_ptr_)) < 0)
    {
        return false;
    }
    timeval set = {60, 0};
    setsockopt(socket_, SOL_SOCKET, SO_RCVTIMEO,
               reinterpret_cast<char *>(&set), sizeof(set));
#ifndef NDEBUG
    debug_mutex.lock();
    std::clog << "[SOCK_CONNECT] " << socket_.connection_type() << "::connect("
              << (socket_.connection_type() != "sun" ? inet_ntoa(
                  self_socket_.sin_addr) : this->sun_path_)
              << ":" << ntohs(self_socket_.sin_port) << ")\n" << std::flush;
    debug_mutex.unlock();
#endif
    return this->state_ = true;
}

int connection::id() const noexcept
{
    return socket_;
}

bool connection::status() const noexcept
{
    return state_;
}

void connection::assign_thread(int id)
{
    auto thread_id = std::this_thread::get_id();
    auto it = std::find_if(client_list_->begin(), client_list_->end(),
                           [&thread_id]
                               (const std::pair<std::thread::id, int> &element) {
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
}

int connection::get_descriptor() const
{
    auto thread_id = std::this_thread::get_id();
    auto it = std::find_if(client_list_->begin(), client_list_->end(),
                           [&thread_id]
                               (const std::pair<std::thread::id, int> &element) {
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
