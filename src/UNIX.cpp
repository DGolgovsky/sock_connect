#include "UNIX.h"

#ifdef NDEBUG
#include <iostream>
#endif

UNIX::UNIX(std::string &path)
        : Connection(_UNIX, path) {

}

UNIX::~UNIX() {
    //shutdown(transmission, SHUT_RDWR);
    if (transmission >= 0)
        close(transmission);
}

ssize_t UNIX::Receive(uint8_t *value, std::size_t tu_size) {
    msg_sz = read(transmission, value, tu_size);

    if (msg_sz < 1) {
        this->state = false;
#ifdef NDEBUG
        std::clog << "UNIX<uint8_t>::Receive: status <DISCONNECTED>\n" << std::flush;
#endif
        return 0;
    }
#ifdef NDEBUG
    std::clog << "UNIX<uint8_t>::Receive: received <" << value << " [+"
              << tu_size << "]>\n" << std::flush;
#endif
    return msg_sz;

}

ssize_t UNIX::Receive(uint16_t *value, std::size_t tu_size) {
    msg_sz = read(transmission, value, tu_size);

    if (msg_sz < 1) {
        this->state = false;
#ifdef NDEBUG
        std::clog << "UNIX<uint16_t>::Receive: status <DISCONNECTED>\n" << std::flush;
#endif
        return 0;
    }
#ifdef NDEBUG
    std::clog << "UNIX<uint16_t>::Receive: received <" << value << " [+"
              << tu_size << "]>\n" << std::flush;
#endif
    return msg_sz;
}

ssize_t UNIX::Receive(uint32_t *value, std::size_t tu_size) {
    msg_sz = read(transmission, value, tu_size);

    if (msg_sz < 1) {
        this->state = false;
#ifdef NDEBUG
        std::clog << "UNIX<uint32_t>::Receive: status <DISCONNECTED>\n" << std::flush;
#endif
        return 0;
    }
#ifdef NDEBUG
    std::clog << "UNIX<uint32_t>::Receive: received <" << value << " [+"
              << tu_size << "]>\n" << std::flush;
#endif
    return msg_sz;
}

ssize_t UNIX::Receive(std::string &message, std::size_t tu_size) {
    msg_sz = read(transmission, buffer, tu_size);

    if (msg_sz < 1) {
        this->state = false;
#ifdef NDEBUG
        std::clog << "UNIX<char*>::Receive: status <DISCONNECTED>\n" << std::flush;
#endif
        return 0;
    }
    buffer[msg_sz] = '\0';
    message = std::string(buffer, static_cast<std::size_t>(msg_sz));
    //message.erase(1, std::min(message.find_first_not_of('0'), message.size()));
#ifdef NDEBUG
    std::clog << "UNIX<char*>::Receive: received <" << message.data() << " [+"
              << message.size() << "]>\n" << std::flush;
#endif
    return msg_sz;
}

ssize_t UNIX::Send(const uint8_t *value, std::size_t tu_size) {
    if ((msg_sz = write(transmission, value, tu_size)) < 1) {
        this->state = false;
#ifdef NDEBUG
        std::clog << "UNIX<uint8_t>::Send: status <DISCONNECTED>\n" << std::flush;
#endif
        return 0;
    }
#ifdef NDEBUG
    std::clog << "UNIX<uint8_t>::Send: sent <" << value << " [+"
              << tu_size << "]>\n" << std::flush;
#endif
    return msg_sz;
}

ssize_t UNIX::Send(const uint16_t *value, std::size_t tu_size) {
    if ((msg_sz = write(transmission, value, tu_size)) < 1) {
        this->state = false;
#ifdef NDEBUG
        std::clog << "UNIX<uint16_t>::Send: status <DISCONNECTED>\n" << std::flush;
#endif
        return 0;
    }
#ifdef NDEBUG
    std::clog << "UNIX<uint16_t>::Send: sent <" << value << " [+"
              << tu_size << "]>\n" << std::flush;
#endif
    return msg_sz;
}

ssize_t UNIX::Send(const uint32_t *value, std::size_t tu_size) {
    if ((msg_sz = write(transmission, value, tu_size)) < 1) {
        this->state = false;
#ifdef NDEBUG
        std::clog << "UNIX<uint32_t>::Send: status <DISCONNECTED>\n" << std::flush;
#endif
        return 0;
    }
#ifdef NDEBUG
    std::clog << "UNIX<uint32_t>::Send: sent <" << value << " [+"
              << tu_size << "]>\n" << std::flush;
#endif
    return msg_sz;
}

ssize_t UNIX::Send(const std::string &message, std::size_t tu_size) {
    if ((msg_sz = write(transmission, message.data(), tu_size)) < 1) {
        this->state = false;
#ifdef NDEBUG
        std::clog << "UNIX<char*>::Send: status <DISCONNECTED>\n" << std::flush;
#endif
        return 0;
    }
#ifdef NDEBUG
    std::clog << "UNIX<char*>::Send: sent <" << message.data() << " [+"
              << message.size() << "]>\n" << std::flush;
#endif
    return msg_sz;
}
