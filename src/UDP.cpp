#include "UDP.h"

#ifdef NDEBUG
#include <iostream>
#endif

UDP::UDP(uint32_t address, uint16_t port)
        : Connection(_UDP, address, port) {
    /*
    int set = 1;
    setsockopt(clients[std::this_thread::get_id()], SOL_SOCKET, SO_REUSEADDR,
               &set, sizeof(int));
               */
    memset(&buffer, '\0', sizeof(buffer));
}

UDP::UDP(const char *address, uint16_t port)
        : Connection(_UDP, address, port) {
    /*
    int set = 1;
    setsockopt(clients[std::this_thread::get_id()], SOL_SOCKET, SO_REUSEADDR,
               &set, sizeof(set));
               */
    memset(&buffer, '\0', sizeof(buffer));
}

UDP::~UDP() {
    shutdown(clients[std::this_thread::get_id()], SHUT_RDWR);
}

template<>
ssize_t UDP::Receive(uint8_t *value, const std::size_t tu_size) {
    msg_sz = recvfrom(clients[std::this_thread::get_id()], value, tu_size, 0,
                      ptr_addr, &size_addr);

#ifdef NDEBUG
    debug_mutex.lock();
    std::clog << "[SOCK_CONNECT] UDP<uint8_t>::Receive: received <";
    print_values(value, tu_size);
    std::clog << " [+" << tu_size << "]>\n" << std::flush;
    debug_mutex.unlock();
#endif
    return msg_sz;
}

template<>
ssize_t UDP::Receive(uint16_t *value, const std::size_t tu_size) {
    msg_sz = recvfrom(clients[std::this_thread::get_id()], value, tu_size, 0,
                      ptr_addr, &size_addr);

#ifdef NDEBUG
    debug_mutex.lock();
    std::clog << "[SOCK_CONNECT] UDP<uint16_t>::Receive: received <";
    print_values(value, tu_size);
    std::clog << " [+" << tu_size << "]>\n" << std::flush;
    debug_mutex.unlock();
#endif
    return msg_sz;
}

template<>
ssize_t UDP::Receive(uint32_t *value, const std::size_t tu_size) {
    msg_sz = recvfrom(clients[std::this_thread::get_id()], value, tu_size, 0,
                      ptr_addr, &size_addr);

#ifdef NDEBUG
    debug_mutex.lock();
    std::clog << "[SOCK_CONNECT] UDP<uint32_t>::Receive: received <";
    print_values(value, tu_size);
    std::clog << " [+" << tu_size << "]>\n" << std::flush;
    debug_mutex.unlock();
#endif
    return msg_sz;
}

template<>
ssize_t UDP::Receive(std::string *value, const std::size_t tu_size) {
    msg_sz = recvfrom(clients[std::this_thread::get_id()], buffer, tu_size, 0,
                      ptr_addr, &size_addr);

    buffer[msg_sz] = '\0';
    *value = std::string(buffer, static_cast<std::size_t>(msg_sz));
    //value.erase(1, std::min(value.find_first_not_of('0'), value.size()-1));
#ifdef NDEBUG
    debug_mutex.lock();
    std::clog << "[SOCK_CONNECT] UDP<char*>::Receive: received <" << value->data() << " [+"
              << value->size() << "]>\n" << std::flush;
    debug_mutex.unlock();
#endif
    return msg_sz;
}

template<>
ssize_t UDP::Receive(char *value, const std::size_t tu_size) {
    std::string msg{};
    msg_sz = UDP::Receive(&msg, tu_size);
    strcpy(value, msg.c_str());
    return msg_sz;
}

template<>
ssize_t UDP::Send(const uint8_t *value, const std::size_t tu_size) {
    msg_sz = sendto(clients[std::this_thread::get_id()], value, tu_size, 0,
                    ptr_addr, size_addr);
#ifdef NDEBUG
    if (msg_sz) {
        debug_mutex.lock();
        std::clog << "[SOCK_CONNECT] UDP<uint8_t>::Send: sent <";
    print_values(value, tu_size);
    std::clog << " [+" << tu_size << "]>\n" << std::flush;
        debug_mutex.unlock();
    }
#endif
    return msg_sz;
}

template<>
ssize_t UDP::Send(const uint16_t *value, const std::size_t tu_size) {
    msg_sz = sendto(clients[std::this_thread::get_id()], value, tu_size, 0,
                    ptr_addr, size_addr);
#ifdef NDEBUG
    if (msg_sz) {
        debug_mutex.lock();
        std::clog << "[SOCK_CONNECT] UDP<uint16_t>::Send: sent <";
    print_values(value, tu_size);
    std::clog << " [+" << tu_size << "]>\n" << std::flush;
        debug_mutex.unlock();
    }
#endif
    return msg_sz;
}

template<>
ssize_t UDP::Send(const uint32_t *value, const std::size_t tu_size) {
    msg_sz = sendto(clients[std::this_thread::get_id()], value, tu_size, 0,
                    ptr_addr, size_addr);
#ifdef NDEBUG
    if (msg_sz) {
        debug_mutex.lock();
        std::clog << "[SOCK_CONNECT] UDP<uint32_t>::Send: sent <";
        print_values(value, tu_size);
        std::clog << " [+" << tu_size << "]>\n" << std::flush;
        debug_mutex.unlock();
    }
#endif
    return msg_sz;
}

template<>
ssize_t UDP::Send(const std::string *value, const std::size_t tu_size) {
    msg_sz = sendto(clients[std::this_thread::get_id()], value->data(), tu_size,
                    0,
                    ptr_addr, size_addr);
#ifdef NDEBUG
    if (msg_sz) {
        debug_mutex.lock();
        std::clog << "[SOCK_CONNECT] UDP<char*>::Send: sent <" << value->data() << " [+"
                  << value->size() << "]>\n" << std::flush;
        debug_mutex.unlock();
    }
#endif
    return msg_sz;
}

template<>
ssize_t UDP::Send(const char *value, const std::size_t tu_size) {
    std::string msg{value};
    return UDP::Send(&msg, tu_size);
}
