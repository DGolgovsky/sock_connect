#include "interface/tcp.h"

tcp::tcp(uint32_t address, uint16_t port)
    : connection(TCP, address, port)
{
#ifndef NDEBUG
    debug_mutex.lock();
    std::clog << "[SOCK_CONNECT] tcp::tcp(" << type_name<decltype(address)>()
              << " address: " << address << ", " << type_name<decltype(port)>()
              << " port: " << port << ")" << '\n' << std::flush;
    debug_mutex.unlock();
#endif
}

tcp::tcp(const char *address, uint16_t port)
    : connection(TCP, address, port)
{
#ifndef NDEBUG
    debug_mutex.lock();
    std::clog << "[SOCK_CONNECT] tcp::tcp(" << type_name<decltype(address)>()
              << " address: " << address << ", " << type_name<decltype(port)>()
              << " port: " << port << ")" << '\n' << std::flush;
    debug_mutex.unlock();
#endif
}

tcp::tcp(const std::string &address, uint16_t port)
    : connection(TCP, address.data(), port)
{
#ifndef NDEBUG
    debug_mutex.lock();
    std::clog << "[SOCK_CONNECT] tcp::tcp(" << type_name<decltype(address)>()
              << " address: " << address << ", " << type_name<decltype(port)>()
              << " port: " << port << ")" << '\n' << std::flush;
    debug_mutex.unlock();
#endif
}

tcp::~tcp()
{
#ifndef NDEBUG
    debug_mutex.lock();
    std::clog << "[SOCK_CONNECT] tcp::~tcp()\n" << std::flush;
    debug_mutex.unlock();
#endif
}

template <typename T>
size_t tcp::receive(T *value, size_t tu_size)
{
    auto recv_left = tu_size;
    size_t total = 0;
    while (total < tu_size)
    {
        msg_sz_ = ::recv(get_descriptor(), value + total, recv_left,
                         MSG_NOSIGNAL);
        if (msg_sz_ < 1)
        {
#ifndef NDEBUG
            debug_mutex.lock();
            std::clog << "[SOCK_CONNECT] tcp::receive<"
                      << type_name<decltype(value)>() << ">(fd: "
                      << get_descriptor() << "): DISCONNECTED" << '\n'
                      << std::flush;
            debug_mutex.unlock();
#endif
            this->state_ = false;
            return total;
        }
        recv_left -= static_cast<unsigned long>(msg_sz_);
        total += static_cast<unsigned long>(msg_sz_);
    }
#ifndef NDEBUG
    debug_mutex.lock();
    std::clog << "[SOCK_CONNECT] tcp::receive<" << type_name<decltype(value)>()
              << ">: <" << print_values(value, tu_size) << " [+" << total
              << "]>\n" << std::flush;
    debug_mutex.unlock();
#endif
    return total;
}

template <typename T>
size_t tcp::send(T const *value, size_t tu_size)
{
    /**
     * TCP MTU = 1460
     * net.core.rmem_max = 212992
     */
    auto send_left = tu_size;
    size_t total = 0;
    while (send_left > 0)
    {
        msg_sz_ = ::send(get_descriptor(), value + total, send_left,
                         MSG_NOSIGNAL);
        if (msg_sz_ < 1)
        {
#ifndef NDEBUG
            debug_mutex.lock();
            std::clog << "[SOCK_CONNECT] tcp::send<"
                      << type_name<decltype(value)>() << ">(fd: "
                      << get_descriptor() << "): DISCONNECTED" << '\n'
                      << std::flush;
            debug_mutex.unlock();
#endif
            this->state_ = false;
            return total;
        }
        send_left -= static_cast<unsigned long>(msg_sz_);
        total += static_cast<unsigned long>(msg_sz_);
    }
#ifndef NDEBUG
    debug_mutex.lock();
    std::clog << "[SOCK_CONNECT] tcp::send<" << type_name<decltype(value)>()
              << ">: <" << print_values(value, tu_size) << " [+" << total
              << "]>\n" << std::flush;
    debug_mutex.unlock();
#endif
    return total;
}

template size_t tcp::receive(char *, size_t);
template size_t tcp::receive(unsigned char *, size_t);
template size_t tcp::receive(short int *, size_t);
template size_t tcp::receive(unsigned short int *, size_t);
template size_t tcp::receive(int *, size_t);
template size_t tcp::receive(unsigned int *, size_t);
template size_t tcp::receive(long int *, size_t);
template size_t tcp::receive(unsigned long int *, size_t);
template size_t tcp::receive(long long int *, size_t);
template size_t tcp::receive(unsigned long long int *, size_t);
template size_t tcp::receive(float *, size_t);
template size_t tcp::receive(double *, size_t);
template size_t tcp::receive(long double *, size_t);
template size_t tcp::receive(bool *, size_t);

template size_t tcp::send(char const *, size_t);
template size_t tcp::send(unsigned char const *, size_t);
template size_t tcp::send(short int const *, size_t);
template size_t tcp::send(unsigned short int const *, size_t);
template size_t tcp::send(int const *, size_t);
template size_t tcp::send(unsigned int const *, size_t);
template size_t tcp::send(long int const *, size_t);
template size_t tcp::send(unsigned long int const *, size_t);
template size_t tcp::send(long long int const *, size_t);
template size_t tcp::send(unsigned long long int const *, size_t);
template size_t tcp::send(float const *, size_t);
template size_t tcp::send(double const *, size_t);
template size_t tcp::send(long double const *, size_t);
template size_t tcp::send(bool const *, size_t);

template <>
size_t tcp::receive(std::string *value, size_t const tu_size)
{
    value->resize(tu_size < value->max_size() ? tu_size : value->max_size(),
                  '\0');
    return tcp::receive(&value->front(), tu_size);
}

template <>
size_t tcp::send(std::string const *value, size_t const tu_size)
{
    return tcp::send(value->c_str(), tu_size);
}
