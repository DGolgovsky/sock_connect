#include "UDP.h"
#include <cstring>

#ifdef NDEBUG
#include <iostream>
#endif

UDP::UDP(uint32_t address, uint16_t port)
        : Connection(_UDP, address, port) {
    int set = 1;
    setsockopt(transmission, SOL_SOCKET, SO_BROADCAST, &set, sizeof(set));
    std::memset(&buffer, '\0', sizeof(buffer));
}

UDP::~UDP() {
    shutdown(transmission, SHUT_RDWR);
}

ssize_t UDP::Receive(uint8_t *value, std::size_t tu_size) {
    msg_sz = recvfrom(transmission, value, tu_size, MSG_WAITALL, nullptr, nullptr);

#ifdef NDEBUG
    std::clog << "UDP<uint8_t>::Receive: received <" << value << " [+"
              << tu_size << "]>\n" << std::flush;
#endif
    return msg_sz;
}

ssize_t UDP::Receive(uint16_t *value, const std::size_t tu_size) {
    msg_sz = recvfrom(transmission, value, tu_size, MSG_WAITALL, nullptr, nullptr);

#ifdef NDEBUG
    std::clog << "UDP<uint16_t>::Receive: received <" << value << " [+"
              << tu_size << "]>\n" << std::flush;
#endif
    return msg_sz;
}

ssize_t UDP::Receive(uint32_t *value, const std::size_t tu_size) {
    msg_sz = recvfrom(transmission, value, tu_size, MSG_WAITALL, nullptr, nullptr);

#ifdef NDEBUG
    std::clog << "UDP<uint32_t>::Receive: received <" << value << " [+"
              << tu_size << "]>\n" << std::flush;
#endif
    return msg_sz;
}

ssize_t UDP::Receive(std::string &message, const std::size_t) {
    msg_sz = recvfrom(transmission, buffer, sizeof(buffer), MSG_WAITALL, nullptr, nullptr);

    buffer[msg_sz] = '\0';
    message = std::string(buffer, static_cast<std::size_t>(msg_sz));
    //message.erase(1, std::min(message.find_first_not_of('0'), message.size()-1));
#ifdef NDEBUG
    std::clog << "UDP<char*>::Receive: received <" << message.data() << " [+"
              << message.size() << "]>\n" << std::flush;
#endif
    return msg_sz;
}

ssize_t UDP::Send(const uint8_t *value, const std::size_t tu_size) {
    msg_sz = sendto(transmission, value, tu_size, MSG_CONFIRM, (const sockaddr *) &socket_addr, sizeof(socket_addr));
#ifdef NDEBUG
    if (msg_sz)
        std::clog << "UDP<uint8_t>::Send: sent <" << value << " [+"
              << tu_size << "]>\n" << std::flush;
#endif
    return msg_sz;
}

ssize_t UDP::Send(const uint16_t *value, const std::size_t tu_size) {
    msg_sz = sendto(transmission, value, tu_size, MSG_CONFIRM, (const sockaddr *) &socket_addr, sizeof(socket_addr));
#ifdef NDEBUG
    if (msg_sz)
        std::clog << "UDP<uint16_t>::Send: sent <" << value << " [+"
              << tu_size << "]>\n" << std::flush;
#endif
    return msg_sz;
}

ssize_t UDP::Send(const uint32_t *value, const std::size_t tu_size) {
    msg_sz = sendto(transmission, value, tu_size, MSG_CONFIRM, (const sockaddr *) &socket_addr, sizeof(socket_addr));
#ifdef NDEBUG
    if (msg_sz)
        std::clog << "UDP<uint32_t>::Send: sent <" << value << " [+"
              << tu_size << "]>\n" << std::flush;
#endif
    return msg_sz;
}

ssize_t UDP::Send(const std::string &message, const std::size_t) {
    msg_sz = sendto(transmission, message.data(), message.size(), MSG_CONFIRM, (const sockaddr *) &socket_addr,
                    sizeof(socket_addr));
#ifdef NDEBUG
    if (msg_sz)
        std::clog << "UDP<char*>::Send: sent <" << message.data() << " [+"
              << message.size() << "]>\n" << std::flush;
#endif
    return msg_sz;
}
