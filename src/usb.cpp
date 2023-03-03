#include "interface/usb.h"

usb::usb(std::string const &address, speed_t speed)
    : connection(USB, address)
    , dev_path_(address)
    , dev_speed_(speed)
{
#ifndef NDEBUG
    debug_mutex.lock();
    std::clog << "[SOCK_CONNECT] usb::usb(" << type_name<decltype(address)>()
              << " address: " << address << ", " << type_name<decltype(speed)>()
              << " speed: " << speed << ")" << '\n' << std::flush;
    debug_mutex.unlock();
#endif
}

usb::usb(char const *address, speed_t speed)
    : connection(USB, address)
    , dev_path_(std::string(address))
    , dev_speed_(speed)
{
#ifndef NDEBUG
    debug_mutex.lock();
    std::clog << "[SOCK_CONNECT] usb::usb(" << type_name<decltype(address)>()
              << " address: " << address << ", " << type_name<decltype(speed)>()
              << " speed: " << speed << ")" << '\n' << std::flush;
    debug_mutex.unlock();
#endif
}

usb::~usb()
{
    shutdown(socket_);
#ifndef NDEBUG
    debug_mutex.lock();
    std::clog << "[SOCK_CONNECT] usb::~usb()\n" << std::flush;
    debug_mutex.unlock();
#endif
}

bool usb::connect()
{
    if (state_)
    {
        return state_;
    }

    socket_.set_id(open(dev_path_.c_str(), O_RDWR | O_NOCTTY | O_NDELAY));

    if (socket_ == -1)
    {
        return (state_ = false);
    }

    /**
     * CONFIGURE THE UART
     * The flags (defined in /usr/include/termios.h - see http://pubs.opengroup.org/onlinepubs/007908799/xsh/termios.h.html):
     *
     * Baud rate:- B1200, B2400, B4800, B9600, B19200, B38400, B57600,
     * B115200, B230400, B460800, B500000, B576000, B921600, B1000000,
     * B1152000, B1500000, B2000000, B2500000, B3000000, B3500000, B4000000
     *
     * CSIZE:- CS5, CS6, CS7, CS8
     * CLOCAL - Ignore modem status lines
     * CREAD - Enable receiver
     * IGNPAR = Ignore characters with parity errors
     * ICRNL - Map CR to NL on input (Use for ASCII comms where you want to auto correct end of line characters - don't use for binary comms!)
     * PARENB - Parity enable
     * PARODD - Odd parity (else even)
     */
    termios tio{};
    ::tcgetattr(socket_, &tio);
    cfmakeraw(&tio);
    tio.c_iflag &= static_cast<unsigned int>(~(IXON | IXOFF));
    tio.c_oflag = 0;
    tio.c_lflag = 0;
    tio.c_cc[VTIME] = 0;
    tio.c_cc[VMIN] = 0;
    cfsetspeed(&tio, dev_speed_);
    int err = ::tcsetattr(socket_, TCSAFLUSH, &tio);
    if (err != 0)
    {
        this->shutdown(socket_);
        return (state_ = false);
    }

#ifndef NDEBUG
    debug_mutex.lock();
    std::clog << "[SOCK_CONNECT] usb::connect(" << dev_path_.data() << ":"
              << dev_speed_ << ")\n" << std::flush;
    debug_mutex.unlock();
#endif
    return (state_ = true);
}

template <typename T>
size_t usb::receive(T *value, size_t tu_size)
{
    auto recv_left = tu_size;
    size_t total = 0;
    while (total < tu_size)
    {
        msg_sz_ = read(socket_, value + total, recv_left);
        if (msg_sz_ < 0)
        {
#ifndef NDEBUG
            debug_mutex.lock();
            std::clog << "[SOCK_CONNECT] usb::receive<"
                      << type_name<decltype(value)>() << ">(fd: "
                      << get_descriptor() << "): DISCONNECTED" << '\n'
                      << std::flush;
            debug_mutex.unlock();
#endif
            this->state_ = false;
            return total;
        }
        recv_left -= static_cast<size_t>(msg_sz_);
        total += static_cast<size_t>(msg_sz_);
    }
#ifndef NDEBUG
    debug_mutex.lock();
    std::clog << "[SOCK_CONNECT] usb::receive<" << type_name<decltype(value)>()
              << ">: <" << print_values(value, tu_size) << " [+" << total
              << "]>\n" << std::flush;
    debug_mutex.unlock();
#endif
    return total;
}

template <typename T>
size_t usb::send(T const *value, size_t tu_size)
{
    auto send_left = tu_size;
    size_t total = 0;
    while (send_left > 0)
    {
        msg_sz_ = write(socket_, value + total, send_left);
        if (msg_sz_ < 0)
        {
#ifndef NDEBUG
            debug_mutex.lock();
            std::clog << "[SOCK_CONNECT] usb::send<"
                      << type_name<decltype(value)>() << ">(fd: "
                      << get_descriptor() << "): DISCONNECTED" << '\n'
                      << std::flush;
            debug_mutex.unlock();
#endif
            this->state_ = false;
            return total;
        }
        send_left -= static_cast<size_t>(msg_sz_);
        total += static_cast<size_t>(msg_sz_);
    }
#ifndef NDEBUG
    debug_mutex.lock();
    std::clog << "[SOCK_CONNECT] usb::send<" << type_name<decltype(value)>()
              << ">: <" << print_values(value, tu_size) << " [+" << total
              << "]>\n" << std::flush;
    debug_mutex.unlock();
#endif
    return total;
}

template size_t usb::receive(char *, size_t);
template size_t usb::receive(unsigned char *, size_t);
template size_t usb::receive(short int *, size_t);
template size_t usb::receive(unsigned short int *, size_t);
template size_t usb::receive(int *, size_t);
template size_t usb::receive(unsigned int *, size_t);
template size_t usb::receive(long int *, size_t);
template size_t usb::receive(unsigned long int *, size_t);
template size_t usb::receive(long long int *, size_t);
template size_t usb::receive(unsigned long long int *, size_t);
template size_t usb::receive(float *, size_t);
template size_t usb::receive(double *, size_t);
template size_t usb::receive(long double *, size_t);
template size_t usb::receive(bool *, size_t);

template size_t usb::send(char const *, size_t);
template size_t usb::send(unsigned char const *, size_t);
template size_t usb::send(short int const *, size_t);
template size_t usb::send(unsigned short int const *, size_t);
template size_t usb::send(int const *, size_t);
template size_t usb::send(unsigned int const *, size_t);
template size_t usb::send(long int const *, size_t);
template size_t usb::send(unsigned long int const *, size_t);
template size_t usb::send(long long int const *, size_t);
template size_t usb::send(unsigned long long int const *, size_t);
template size_t usb::send(float const *, size_t);
template size_t usb::send(double const *, size_t);
template size_t usb::send(long double const *, size_t);
template size_t usb::send(bool const *, size_t);

template <>
size_t usb::receive(std::string *value, size_t const tu_size)
{
    value->resize(tu_size < value->max_size() ?
                  tu_size : value->max_size(), '\0');
    return usb::receive(&value->front(), tu_size);
}

template <>
size_t usb::send(std::string const *value, size_t const tu_size)
{
    return usb::send(value->c_str(), tu_size);
}

void usb::set_rts() const
{
    int status = 0;
    ioctl(socket_, TIOCMGET, &status);
    status |= TIOCM_RTS;
    ioctl(socket_, TIOCMSET, &status);
}

void usb::clr_rts() const
{
    int status = 0;
    ioctl(socket_, TIOCMGET, &status);
    status &= ~TIOCM_RTS;
    ioctl(socket_, TIOCMSET, &status);
}

int usb::get_descriptor() const
{
    return socket_;
}

void usb::assign_thread(int)
{
#ifndef NDEBUG
    debug_mutex.lock();
    std::clog << "[SOCK_CONNECT] usb::assign_thread(): " << socket_.id()
              << ". Doesn't use in current state" << '\n' << std::flush;
    debug_mutex.unlock();
#endif
}

int usb::accept(std::string *)
{
#ifndef NDEBUG
    debug_mutex.lock();
    std::clog << "[SOCK_CONNECT] usb::accept(): " << socket_.id()
              << ". Doesn't use in current state" << '\n' << std::flush;
    debug_mutex.unlock();
#endif
    return 1;
}

void usb::bind(bool) const
{
#ifndef NDEBUG
    debug_mutex.lock();
    std::clog << "[SOCK_CONNECT] usb::bind(): " << socket_.id()
              << ". Doesn't use in current state" << '\n' << std::flush;
    debug_mutex.unlock();
#endif
}

void usb::listen() const
{
#ifndef NDEBUG
    debug_mutex.lock();
    std::clog << "[SOCK_CONNECT] usb::listen(): " << socket_.id()
              << ". Doesn't use in current state" << '\n' << std::flush;
    debug_mutex.unlock();
#endif
}

