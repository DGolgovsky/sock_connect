#include "USB.h"
#include <iostream>

USB::USB(const std::string &address, speed_t speed)
    : address(address)
    , speed(speed)
{ }

USB::~USB()
{
    Shutdown();
}

bool USB::Connect()
{
    F_ID = open(address.data(), O_RDWR | O_NOCTTY|O_NONBLOCK);

    if (F_ID == -1) {
        this->state = false;
        Shutdown();
        return false;
    }

    termios options{}; /* port parameters */
    tcgetattr(F_ID, &options); /* read parameters */

    cfsetispeed(&options, speed); /* Set the input baud rate stored in *TERMIOS_P to SPEED.  */
    cfsetospeed(&options, speed); /* Set the output baud rate stored in *TERMIOS_P to SPEED.  */

    options.c_cc[VTIME]    = 0; /* Time o receive byte 20*0.1 = 2 sec */
    options.c_cc[VMIN]     = 0; /* min byte reads */

    options.c_lflag &= ~ICANON; /* non-canonical mode */
    options.c_cflag &= ~PARENB; /*  odd-bit not used */
    options.c_cflag &= ~CSTOPB; /* stop-bit */
    options.c_cflag &= ~CSIZE;  /* byte size */
    options.c_cflag |= CS8;  /* 8 bits */

    options.c_lflag = 0;
    options.c_oflag &= ~OPOST; /*  post-processing off */

    tcsetattr(F_ID, TCSANOW, &options); /* save params */

#ifdef NDEBUG
    std::clog << "Connected to: " << address.data()
                  << ":" << speed << '\n' << std::flush;
#endif
    this->state = true;
    return true;
}

ssize_t USB::Receive(std::string &message, std::size_t tu_size)
{
    msg_sz = -1;
    if (select_status())
        msg_sz = read(F_ID, buffer, tu_size);

    if (msg_sz < 0) {
        this->state = false;
        return 0;
    }

    buffer[msg_sz] = '\0';
    message = std::string(buffer, static_cast<std::size_t>(msg_sz));
#ifdef NDEBUG
    std::clog << "USB Received: \"" << message.data() << "\"["
              << message.size() << "]\n" << std::flush;
#endif
    return msg_sz;
}

ssize_t USB::Receive(uint8_t *value, std::size_t tu_size)
{
    msg_sz = -1;
    if (select_status())
        msg_sz = read(F_ID, value, tu_size);

    if (msg_sz < 0) {
        this->state = false;
        return 0;
    }
#ifdef NDEBUG
    std::clog << "USB Received: \"" << value << "\"["
              << tu_size << "]\n" << std::flush;
#endif
    return msg_sz;
}

ssize_t USB::Send(const std::string &message, std::size_t tu_size)
{
    msg_sz = -1;
    if (select_status())
        msg_sz = write(F_ID, message.data(), tu_size);

    if (msg_sz == -1) {
        this->state = false;
        return 0;
    }
#ifdef NDEBUG
    std::clog << "USB Sent: \"" << message.data() << "\"["
              << message.size() << "]\n" << std::flush;
#endif
    return msg_sz;
}

ssize_t USB::Send(const uint8_t *value, std::size_t tu_size)
{
    msg_sz = -1;
    if (select_status())
        msg_sz = write(F_ID, value, tu_size);

    if (msg_sz == -1) {
        this->state = false;
        return 0;
    }
#ifdef NDEBUG
    std::clog << "USB Sent: \"" << value << "\"["
              << tu_size << "]\n" << std::flush;
#endif
    return msg_sz;
}

void USB::Shutdown()
{
    if (F_ID >= 0)
        close(F_ID);

    F_ID = -1;
}

void USB::setRTS()
{
    int status = 0;
    ioctl(F_ID, TIOCMGET, &status);
    status |= TIOCM_RTS;
    ioctl(F_ID, TIOCMSET, &status);
}

void USB::clrRTS()
{
    int status = 0;
    ioctl(F_ID, TIOCMGET, &status);
    status &= ~TIOCM_RTS;
    ioctl(F_ID, TIOCMSET, &status);
}

bool USB::status() const
{
    return F_ID > -1;
}

int USB::id() const
{
    return F_ID;
}

bool USB::select_status()
{
    timeval tv{};
    tv.tv_sec = 0;
    tv.tv_usec = 0;

    fd_set rfds; /* fd_set for select and pselect.  */
    FD_ZERO(&rfds);
    FD_SET(this->F_ID, &rfds);

    auto return_value = select(this->F_ID + 1, &rfds, nullptr, nullptr, &tv);
    return return_value && FD_ISSET(this->F_ID, &rfds);
}
