#include "TCP.h"
#include <cstring>

#ifdef NDEBUG
#include <iostream>
#endif

TCP::TCP(uint32_t address, uint16_t port)
        : Connection(_TCP, address, port) {
    int set = 1;
    setsockopt(transmission, SOL_SOCKET, SO_REUSEADDR, &set, sizeof(set));
    std::memset(&buffer, '\0', sizeof(buffer));
}

TCP::~TCP() {
    shutdown(transmission, SHUT_RDWR);
}

ssize_t TCP::Receive(uint8_t *value, const size_t tu_size) {
    msg_sz = recv(transmission, value, tu_size, MSG_NOSIGNAL);

    if (msg_sz < 1) {
        this->state = false;
#ifdef NDEBUG
        std::clog << "TCP<uint8_t>::Receive: status <DISCONNECTED>\n" << std::flush;
#endif
        return 0;
    }
#ifdef NDEBUG
    std::clog << "TCP<uint8_t>::Receive: received <" << value << " [+"
              << tu_size << "]>\n" << std::flush;
#endif
    return msg_sz;
}

ssize_t TCP::Receive(uint16_t *value, const std::size_t tu_size) {
    msg_sz = recv(transmission, value, tu_size, MSG_NOSIGNAL);

    if (msg_sz < 1) {
        this->state = false;
#ifdef NDEBUG
        std::clog << "TCP<uint16_t>::Receive: status <DISCONNECTED>\n" << std::flush;
#endif
        return 0;
    }
#ifdef NDEBUG
    std::clog << "TCP<uint16_t>::Receive: received <" << value << " [+"
              << tu_size << "]>\n" << std::flush;
#endif
    return msg_sz;
}

ssize_t TCP::Receive(uint32_t *value, const std::size_t tu_size) {
    msg_sz = recv(transmission, value, tu_size, MSG_NOSIGNAL);

    if (msg_sz < 1) {
        this->state = false;
#ifdef NDEBUG
        std::clog << "TCP<uint32_t>::Receive: status <DISCONNECTED>\n" << std::flush;
#endif
        return 0;
    }
#ifdef NDEBUG
    std::clog << "TCP<uint32_t>::Receive: received <" << value << " [+"
              << tu_size << "]>\n" << std::flush;
#endif
    return msg_sz;
}

ssize_t TCP::Receive(std::string &message, const std::size_t tu_size) {
    msg_sz = recv(transmission, buffer, tu_size, MSG_NOSIGNAL);

    if (msg_sz < 1) {
        this->state = false;
#ifdef NDEBUG
        std::clog << "TCP<char*>::Receive: status <DISCONNECTED>\n" << std::flush;
#endif
        return 0;
    }
    buffer[msg_sz] = '\0';
    message = std::string(buffer, static_cast<std::size_t>(msg_sz));
    //message.erase(1, std::min(message.find_first_not_of('0'), message.size()));
#ifdef NDEBUG
    std::clog << "TCP<char*>::Receive: received <" << message.data() << " [+"
              << message.size() << "]>\n" << std::flush;
#endif
    return msg_sz;
}

ssize_t TCP::Send(const uint8_t *value, const std::size_t tu_size) {
    if ((msg_sz = send(transmission, value, tu_size, MSG_NOSIGNAL)) < 1) {
        this->state = false;
#ifdef NDEBUG
        std::clog << "TCP<uint8_t>::Send: status <DISCONNECTED>\n" << std::flush;
#endif
        return 0;
    }
#ifdef NDEBUG
    std::clog << "TCP<uint8_t>::Send: sent <" << value << " [+"
              << tu_size << "]>\n" << std::flush;
#endif
    return msg_sz;
}

ssize_t TCP::Send(const uint16_t *value, const std::size_t tu_size) {
    if ((msg_sz = send(transmission, value, tu_size, MSG_NOSIGNAL)) < 1) {
        this->state = false;
#ifdef NDEBUG
        std::clog << "TCP<uint16_t>::Send: status <DISCONNECTED>\n" << std::flush;
#endif
        return 0;
    }
#ifdef NDEBUG
    std::clog << "TCP<uint16_t>::Send: sent <" << value << " [+"
              << tu_size << "]>\n" << std::flush;
#endif
    return msg_sz;
}

ssize_t TCP::Send(const uint32_t *value, const std::size_t tu_size) {
    if ((msg_sz = send(transmission, value, tu_size, MSG_NOSIGNAL)) < 1) {
        this->state = false;
#ifdef NDEBUG
        std::clog << "TCP<uint32_t>::Send: status <DISCONNECTED>\n" << std::flush;
#endif
        return 0;
    }
#ifdef NDEBUG
    std::clog << "TCP<uint32_t>::Send: sent <" << value << " [+"
              << tu_size << "]>\n" << std::flush;
#endif
    return msg_sz;
}

ssize_t TCP::Send(const std::string &message, const std::size_t tu_size) {
    if ((msg_sz = send(transmission, message.data(), tu_size, MSG_NOSIGNAL)) < 1) {
        this->state = false;
#ifdef NDEBUG
        std::clog << "TCP<char*>::Send: status <DISCONNECTED>\n" << std::flush;
#endif
        return 0;
    }
#ifdef NDEBUG
    std::clog << "TCP<char*>::Send: sent <" << message.data() << " [+"
              << message.size() << "]>\n" << std::flush;
#endif
    return msg_sz;
}
