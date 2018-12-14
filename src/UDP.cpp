#include "UDP.h"
#include <cstring>

#ifdef NDEBUG
#include <iostream>
#endif

UDP::UDP(uint32_t address, uint16_t port)
        : Connection(_UDP, address, port)
        //, socket_(_UDP)
{
    int set = 1;
    setsockopt(socket_.id(), SOL_SOCKET, SO_BROADCAST, &set, sizeof(set));
    std::memset(&buffer, '\0', sizeof(buffer));
}

UDP::~UDP()
{
    shutdown(socket_.id(), SHUT_RDWR);
}

ssize_t UDP::Receive(uint8_t *value, std::size_t)
{
    msg_sz = recvfrom(socket_.id(), value, sizeof(value), MSG_WAITALL, nullptr, nullptr);

#ifdef NDEBUG
    std::clog << "Received: \"" << value << "\"["
              << sizeof(value) << "]\n" << std::flush;
#endif
    return msg_sz;
}

ssize_t UDP::Receive(std::string &message, std::size_t)
{
    msg_sz = recvfrom(socket_.id(), buffer, sizeof(buffer), MSG_WAITALL, nullptr, nullptr);

    buffer[msg_sz] = '\0';
    message = std::string(buffer, static_cast<std::size_t>(msg_sz));
    //message.erase(1, std::min(message.find_first_not_of('0'), message.size()-1));
#ifdef NDEBUG
    std::clog << "Received: \"" << message.data() << "\"["
              << message.size() << "]\n" << std::flush;
#endif
    return msg_sz;
}

ssize_t UDP::Send(const uint8_t *value, std::size_t)
{
    msg_sz = sendto(socket_.id()
            , value
            , sizeof(value)
            , MSG_CONFIRM
            , (const sockaddr *) &socket_addr
            , sizeof(socket_addr) );
#ifdef NDEBUG
    if (msg_sz)
        std::clog << "_UDP Sent: " << value << '\n' << std::flush;
#endif
    return msg_sz;
}

ssize_t UDP::Send(const std::string &message, std::size_t)
{
    msg_sz = sendto(socket_.id()
            , message.data()
            , message.size()
            , MSG_CONFIRM
            , (const sockaddr *) &socket_addr
            , sizeof(socket_addr) );
#ifdef NDEBUG
    if (msg_sz)
        std::clog << "_UDP Sent: " << message << '\n' << std::flush;
#endif
    return msg_sz;
}
