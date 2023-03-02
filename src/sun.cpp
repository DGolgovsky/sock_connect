#include "interface/sun.h"

sun::sun(std::string const &path)
    : connection(SUN, path)
{
#ifndef NDEBUG
    debug_mutex.lock();
    std::clog << "[SOCK_CONNECT] sun::sun("
              << "std::string path: " << path << ")" << '\n' << std::flush;
    debug_mutex.unlock();
#endif
}

sun::sun(char const *path)
    : connection(SUN, path)
{
#ifndef NDEBUG
    debug_mutex.lock();
    std::clog << "[SOCK_CONNECT] sun::sun(" << type_name<decltype(path)>()
              << " path: " << path << ")" << '\n' << std::flush;
    debug_mutex.unlock();
#endif
}

sun::~sun()
{
#ifndef NDEBUG
    debug_mutex.lock();
    std::clog << "[SOCK_CONNECT] sun::~sun)\n" << std::flush;
    debug_mutex.unlock();
#endif
}

template<typename T>
size_t sun::receive(T *value, size_t tu_size)
{
    auto recv_left = tu_size;
    size_t total = 0;
    while (total < tu_size)
    {
        msg_sz_ = ::read(get_descriptor(), value + total, recv_left);
        if (msg_sz_ < 1)
        {
#ifndef NDEBUG
            debug_mutex.lock();
            std::clog << "[SOCK_CONNECT] sun::receive<"
                      << type_name<decltype(value)>()
                      << ">(fd: " << get_descriptor() << "): DISCONNECTED"
                      << '\n' << std::flush;
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
    std::clog << "[SOCK_CONNECT] sun::receive<" << type_name<decltype(value)>()
              << ">: <" << print_values(value, tu_size) << " [+" << total
              << "]>\n" << std::flush;
    debug_mutex.unlock();
#endif
    return total;
}

template<typename T>
size_t sun::send(T const *value, size_t tu_size)
{
    auto send_left = tu_size;
    size_t total = 0;
    while (send_left > 0)
    {
        msg_sz_ = ::write(get_descriptor(), value + total, send_left);
        if (msg_sz_ < 1)
        {
#ifndef NDEBUG
            debug_mutex.lock();
            std::clog << "[SOCK_CONNECT] sun::send<"
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
    std::clog << "[SOCK_CONNECT] sun::send<" << type_name<decltype(value)>()
              << ">: <" << print_values(value, tu_size)
              << " [+" << total << "]>\n" << std::flush;
    debug_mutex.unlock();
#endif
    return total;
}

template size_t sun::receive(char *, size_t);
template size_t sun::receive(unsigned char *, size_t);
template size_t sun::receive(short int *, size_t);
template size_t sun::receive(unsigned short int *, size_t);
template size_t sun::receive(int *, size_t);
template size_t sun::receive(unsigned int *, size_t);
template size_t sun::receive(long int *, size_t);
template size_t sun::receive(unsigned long int *, size_t);
template size_t sun::receive(long long int *, size_t);
template size_t sun::receive(unsigned long long int *, size_t);
template size_t sun::receive(float *, size_t);
template size_t sun::receive(double *, size_t);
template size_t sun::receive(long double *, size_t);
template size_t sun::receive(bool *, size_t);

template size_t sun::send(char const *, size_t);
template size_t sun::send(unsigned char const *, size_t);
template size_t sun::send(short int const *, size_t);
template size_t sun::send(unsigned short int const *, size_t);
template size_t sun::send(int const *, size_t);
template size_t sun::send(unsigned int const *, size_t);
template size_t sun::send(long int const *, size_t);
template size_t sun::send(unsigned long int const *, size_t);
template size_t sun::send(long long int const *, size_t);
template size_t sun::send(unsigned long long int const *, size_t);
template size_t sun::send(float const *, size_t);
template size_t sun::send(double const *, size_t);
template size_t sun::send(long double const *, size_t);
template size_t sun::send(bool const *, size_t);

template<>
size_t sun::receive(std::string *value, size_t const tu_size)
{
    if (value->size() < tu_size && tu_size < value->max_size())
    {
        value->resize(tu_size, '\0');
    }
    return sun::receive(&value->front(), tu_size);
}

template<>
size_t sun::send(std::string const *value, size_t const tu_size)
{
    return sun::send(value->c_str(), tu_size);
}
