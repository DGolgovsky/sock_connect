#include "TCP.h"
#include <cstring>

#ifdef NDEBUG
#include <iostream>
#endif

TCP::TCP(uint32_t address, uint16_t port)
        : Connection(_TCP, address, port)
        //, socket_(_TCP)
{
    int set = 1;
    setsockopt(socket_.id(), SOL_SOCKET, SO_REUSEADDR, &set, sizeof(set));
    std::memset(&buffer, '\0', sizeof(buffer));
}

TCP::~TCP()
{
    shutdown(socket_.id(), SHUT_RDWR);
}

ssize_t TCP::Receive(uint8_t *value, std::size_t tu_size)
{
    ssize_t msg_sz = recv(socket_.id(), value, tu_size, MSG_NOSIGNAL);

    if (msg_sz < 0) {
        this->state = false;
        return 0;
    }
#ifdef NDEBUG
    std::clog << "Received: \"" << value << "\"["
              << sizeof(value) << "]\n" << std::flush;
#endif
    return msg_sz;
}

ssize_t TCP::Receive(std::string &message, std::size_t tu_size)
{
    msg_sz = recv(socket_.id(), buffer, tu_size, MSG_NOSIGNAL);

    if (msg_sz < 0) {
        this->state = false;
        return 0;
    }
    buffer[msg_sz] = '\0';
    message = std::string(buffer, static_cast<std::size_t>(msg_sz));
    //message.erase(1, std::min(message.find_first_not_of('0'), message.size()));
#ifdef NDEBUG
    std::clog << "Received: \"" << message.data() << "\"["
              << message.size() << "]\n" << std::flush;
#endif
    return msg_sz;
}

ssize_t TCP::Send(const uint8_t *value, std::size_t tu_size)
{
    if ((msg_sz = send(socket_.id(), value, tu_size, MSG_NOSIGNAL)) < 0) {
        this->state = false;
        return 0;
    }
#ifdef NDEBUG
    std::clog << "TCP Sent: \"" << value << "\"["
              << tu_size << "]\n" << std::flush;
#endif
    return msg_sz;
}

ssize_t TCP::Send(const std::string &message, std::size_t)
{
    if ((msg_sz = send(socket_.id()
            , message.data()
            , message.size()
            , MSG_NOSIGNAL)) < 0) {
        this->state = false;
        return 0;
    }
#ifdef NDEBUG
    std::clog << "TCP Sent: \"" << message.data() << "\"["
              << message.size() << "]\n" << std::flush;
#endif
    return msg_sz;
}
