#include "TCP.h"

#ifdef NDEBUG
#include <iostream>
#endif

TCP::TCP(uint32_t address, uint16_t port)
        : Connection(_TCP, address, port) {
    int set = 1;
    setsockopt(clients[std::this_thread::get_id()], SOL_SOCKET, SO_REUSEADDR,
               &set, sizeof(set));
    memset(&buffer, '\0', sizeof(buffer));
}

TCP::TCP(const char *address, uint16_t port)
        : Connection(_TCP, address, port) {
    int set = 1;
    setsockopt(clients[std::this_thread::get_id()], SOL_SOCKET, SO_REUSEADDR,
               &set, sizeof(set));
    memset(&buffer, '\0', sizeof(buffer));
}

TCP::~TCP() {
    shutdown(clients[std::this_thread::get_id()], SHUT_RDWR);
}

template<>
ssize_t TCP::Receive(uint8_t *value, const std::size_t tu_size) {
    // map with threads name
    msg_sz = recv(clients[std::this_thread::get_id()], value, tu_size,
                  MSG_NOSIGNAL);

    if (msg_sz < 1) {
        this->state = false;
#ifdef NDEBUG
        debug_mutex.lock();
        std::clog << "[SOCK_CONNECT] TCP<uint8_t>::Receive: status <DISCONNECTED>\n" << std::flush;
        debug_mutex.unlock();
#endif
        return 0;
    }
#ifdef NDEBUG
    debug_mutex.lock();
    std::clog << "[SOCK_CONNECT] TCP<uint8_t>::Receive: received <";
    print_values(value, tu_size);
    std::clog << " [+" << tu_size << "]>\n" << std::flush;
    debug_mutex.unlock();
#endif
    return msg_sz;
}

template<>
ssize_t TCP::Receive(uint16_t *value, const std::size_t tu_size) {
    msg_sz = recv(clients[std::this_thread::get_id()], value, tu_size,
                  MSG_NOSIGNAL);

    if (msg_sz < 1) {
        this->state = false;
#ifdef NDEBUG
        debug_mutex.lock();
        std::clog << "[SOCK_CONNECT] TCP<uint16_t>::Receive: status <DISCONNECTED>\n" << std::flush;
        debug_mutex.unlock();
#endif
        return 0;
    }
#ifdef NDEBUG
    debug_mutex.lock();
    std::clog << "[SOCK_CONNECT] TCP<uint16_t>::Receive: received <";
    print_values(value, tu_size);
    std::clog << " [+" << tu_size << "]>\n" << std::flush;
    debug_mutex.unlock();
#endif
    return msg_sz;
}

template<>
ssize_t TCP::Receive(uint32_t *value, const std::size_t tu_size) {
    msg_sz = recv(clients[std::this_thread::get_id()], value, tu_size,
                  MSG_NOSIGNAL);

    if (msg_sz < 1) {
        this->state = false;
#ifdef NDEBUG
        debug_mutex.lock();
        std::clog << "[SOCK_CONNECT] TCP<uint32_t>::Receive: status <DISCONNECTED>\n" << std::flush;
        debug_mutex.unlock();
#endif
        return 0;
    }
#ifdef NDEBUG
    debug_mutex.lock();
    std::clog << "[SOCK_CONNECT] TCP<uint32_t>::Receive: received <";
    print_values(value, tu_size);
    std::clog << " [+" << tu_size << "]>\n" << std::flush;
    debug_mutex.unlock();
#endif
    return msg_sz;
}

template<>
ssize_t TCP::Receive(std::string *value, const std::size_t tu_size) {
    msg_sz = recv(clients[std::this_thread::get_id()], buffer, tu_size,
                  MSG_NOSIGNAL);

    if (msg_sz < 1) {
        this->state = false;
#ifdef NDEBUG
        debug_mutex.lock();
        std::clog << "[SOCK_CONNECT] TCP<char*>::Receive: status <DISCONNECTED>\n" << std::flush;
        debug_mutex.unlock();
#endif
        return 0;
    }
    buffer[msg_sz] = '\0';
    *value = std::string(buffer, static_cast<std::size_t>(msg_sz));
    //value.erase(1, std::min(value.find_first_not_of('0'), value.size()));
#ifdef NDEBUG
    debug_mutex.lock();
    std::clog << "[SOCK_CONNECT] TCP<char*>::Receive: received <" << value->data() << " [+"
              << value->size() << "]>\n" << std::flush;
    debug_mutex.unlock();
#endif
    return msg_sz;
}

template<>
ssize_t TCP::Receive(char *value, const std::size_t tu_size) {
    std::string msg{value};
    msg_sz = TCP::Receive(&msg, tu_size);
    strcpy(value, msg.c_str());
    return msg_sz;
}

template<>
ssize_t TCP::Send(const uint8_t *value, const std::size_t tu_size) {
    if ((msg_sz = send(clients[std::this_thread::get_id()], value, tu_size,
                       MSG_NOSIGNAL)) < 1) {
        this->state = false;
#ifdef NDEBUG
        debug_mutex.lock();
        std::clog << "[SOCK_CONNECT] TCP<uint8_t>::Send: status <DISCONNECTED>\n" << std::flush;
        debug_mutex.unlock();
#endif
        return 0;
    }
#ifdef NDEBUG
    debug_mutex.lock();
    std::clog << "[SOCK_CONNECT] TCP<uint8_t>::Send: sent <";
    print_values(value, tu_size);
    std::clog << " [+" << tu_size << "]>\n" << std::flush;
    debug_mutex.unlock();
#endif
    return msg_sz;
}

template<>
ssize_t TCP::Send(const uint16_t *value, const std::size_t tu_size) {
    if ((msg_sz = send(clients[std::this_thread::get_id()], value, tu_size,
                       MSG_NOSIGNAL)) < 1) {
        this->state = false;
#ifdef NDEBUG
        debug_mutex.lock();
        std::clog << "[SOCK_CONNECT] TCP<uint16_t>::Send: status <DISCONNECTED>\n" << std::flush;
        debug_mutex.unlock();
#endif
        return 0;
    }
#ifdef NDEBUG
    debug_mutex.lock();
    std::clog << "[SOCK_CONNECT] TCP<uint16_t>::Send: sent <";
    print_values(value, tu_size);
    std::clog << " [+" << tu_size << "]>\n" << std::flush;
    debug_mutex.unlock();
#endif
    return msg_sz;
}

template<>
ssize_t TCP::Send(const uint32_t *value, const std::size_t tu_size) {
    if ((msg_sz = send(clients[std::this_thread::get_id()], value, tu_size,
                       MSG_NOSIGNAL)) < 1) {
        this->state = false;
#ifdef NDEBUG
        debug_mutex.lock();
        std::clog << "[SOCK_CONNECT] TCP<uint32_t>::Send: status <DISCONNECTED>\n" << std::flush;
        debug_mutex.unlock();
#endif
        return 0;
    }
#ifdef NDEBUG
    debug_mutex.lock();
    std::clog << "[SOCK_CONNECT] TCP<uint32_t>::Send: sent <";
    print_values(value, tu_size);
    std::clog << " [+" << tu_size << "]>\n" << std::flush;
    debug_mutex.unlock();
#endif
    return msg_sz;
}

template<>
ssize_t TCP::Send(const std::string *value, const std::size_t tu_size) {
    if ((msg_sz = send(clients[std::this_thread::get_id()], value->data(),
                       tu_size, MSG_NOSIGNAL)) <
        1) {
        this->state = false;
#ifdef NDEBUG
        debug_mutex.lock();
        std::clog << "[SOCK_CONNECT] TCP<char*>::Send: status <DISCONNECTED>\n" << std::flush;
        debug_mutex.unlock();
#endif
        return 0;
    }
#ifdef NDEBUG
    debug_mutex.lock();
    std::clog << "[SOCK_CONNECT] TCP<char*>::Send: sent <" << value->data() << " [+"
              << value->size() << "]>\n" << std::flush;
    debug_mutex.unlock();
#endif
    return msg_sz;
}

template<>
ssize_t TCP::Send(const char *value, const std::size_t tu_size) {
    std::string msg{value};
    return TCP::Send(&msg, tu_size);
}