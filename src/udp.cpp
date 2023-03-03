#include "interface/udp.h"

udp::udp(uint32_t address, uint16_t port)
    : connection(UDP, address, port)
{
#ifndef NDEBUG
    debug_mutex.lock();
    std::clog << "[SOCK_CONNECT] udp::udp(" << type_name<decltype(address)>()
              << " address: " << address << ", " << type_name<decltype(port)>()
              << " port: " << port << ")" << '\n' << std::flush;
    debug_mutex.unlock();
#endif
}

udp::udp(const char *address, uint16_t port)
    : connection(UDP, address, port)
{
#ifndef NDEBUG
    debug_mutex.lock();
    std::clog << "[SOCK_CONNECT] udp::udp(" << type_name<decltype(address)>()
              << " address: " << address << ", " << type_name<decltype(port)>()
              << " port: " << port << ")" << '\n' << std::flush;
    debug_mutex.unlock();
#endif
}

udp::udp(const std::string &address, uint16_t port)
    : connection(UDP, address.data(), port)
{
#ifndef NDEBUG
    debug_mutex.lock();
    std::clog << "[SOCK_CONNECT] udp::udp(" << type_name<decltype(address)>()
              << " address: " << address << ", " << type_name<decltype(port)>()
              << " port: " << port << ")" << '\n' << std::flush;
    debug_mutex.unlock();
#endif
}

udp::~udp()
{
#ifndef NDEBUG
    debug_mutex.lock();
    std::clog << "[SOCK_CONNECT] udp::~udp()\n" << std::flush;
    debug_mutex.unlock();
#endif
}

template <typename T>
size_t udp::receive(T *value, size_t tu_size)
{
    auto recv_left = tu_size;
    size_t total = 0;
    while (total < tu_size)
    {
        msg_sz_ = ::recvfrom(get_descriptor(), value + total, recv_left, 0,
                             addr_ptr_, &addr_size_);
        if (msg_sz_ < 1)
        {
#ifndef NDEBUG
            debug_mutex.lock();
            std::clog << "[SOCK_CONNECT] udp::receive<"
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
    std::clog << "[SOCK_CONNECT] udp::receive<" << type_name<decltype(value)>()
              << ">: <" << print_values(value, tu_size) << " [+" << total
              << "]>\n" << std::flush;
    debug_mutex.unlock();
#endif
    return total;
}

template <typename T>
size_t udp::send(T const *value, size_t tu_size)
{
    /**
     * UDP MTU = 576
     * net.core.rmem_max = 212992
     */
    auto send_left = tu_size;
    size_t total = 0;
    unsigned long frames = tu_size / 576;
    unsigned long last_packet = tu_size - frames * 576;
    while (send_left > 0)
    {
        msg_sz_ = ::sendto(get_descriptor(), value + total,
                           frames ? 576 : last_packet, 0, addr_ptr_,
                           addr_size_);
        if (msg_sz_ < 1)
        {
#ifndef NDEBUG
            debug_mutex.lock();
            std::clog << "[SOCK_CONNECT] udp::send<"
                      << type_name<decltype(value)>() << ">(fd: "
                      << get_descriptor() << "): DISCONNECTED" << '\n'
                      << std::flush;
            debug_mutex.unlock();
#endif
            this->state_ = false;
            return total;
        }
        frames--;
        send_left -= static_cast<unsigned long>(msg_sz_);
        total += static_cast<unsigned long>(msg_sz_);
    }
#ifndef NDEBUG
    debug_mutex.lock();
    std::clog << "[SOCK_CONNECT] udp::send<" << type_name<decltype(value)>()
              << ">: <" << print_values(value, tu_size) << " [+" << total
              << "]>\n" << std::flush;
    debug_mutex.unlock();
#endif
    return total;
}

template size_t udp::receive(char *, size_t);
template size_t udp::receive(unsigned char *, size_t);
template size_t udp::receive(short int *, size_t);
template size_t udp::receive(unsigned short int *, size_t);
template size_t udp::receive(int *, size_t);
template size_t udp::receive(unsigned int *, size_t);
template size_t udp::receive(long int *, size_t);
template size_t udp::receive(unsigned long int *, size_t);
template size_t udp::receive(long long int *, size_t);
template size_t udp::receive(unsigned long long int *, size_t);
template size_t udp::receive(float *, size_t);
template size_t udp::receive(double *, size_t);
template size_t udp::receive(long double *, size_t);
template size_t udp::receive(bool *, size_t);

template size_t udp::send(char const *, size_t);
template size_t udp::send(unsigned char const *, size_t);
template size_t udp::send(short int const *, size_t);
template size_t udp::send(unsigned short int const *, size_t);
template size_t udp::send(int const *, size_t);
template size_t udp::send(unsigned int const *, size_t);
template size_t udp::send(long int const *, size_t);
template size_t udp::send(unsigned long int const *, size_t);
template size_t udp::send(long long int const *, size_t);
template size_t udp::send(unsigned long long int const *, size_t);
template size_t udp::send(float const *, size_t);
template size_t udp::send(double const *, size_t);
template size_t udp::send(long double const *, size_t);
template size_t udp::send(bool const *, size_t);

template <>
size_t udp::receive(std::string *value, size_t const tu_size)
{
    if (value->size() < tu_size && tu_size < value->max_size())
    {
        value->resize(tu_size, '\0');
    }
    return udp::receive(&value->front(), tu_size);
}

template <>
size_t udp::send(std::string const *value, size_t const tu_size)
{
    return udp::send(value->c_str(), tu_size);
}
