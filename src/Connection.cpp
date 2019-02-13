#include <utility>

#include <cstring>
#include <algorithm>

#include "Connection.h"

#ifdef NDEBUG
#include <iostream>
#endif

Connection::Connection(conn_type cp, uint32_t address, uint16_t port)
        : socket_(cp), address_(address), port_(port), transmission(socket_.id()) {
    conn_memset();
    socket_addr.sin_family = AF_INET;
    socket_addr.sin_addr.s_addr = htonl(address_);
    socket_addr.sin_port = htons(port_);
    ptr_addr = (sockaddr *) &socket_addr;
}

Connection::Connection(conn_type cp, std::string socket_path = "/tmp/unix.sock")
        : socket_(cp), m_path(std::move(socket_path)), transmission(socket_.id()) {
    conn_memset();
    unix_addr.sun_family = AF_UNIX;
    ptr_addr = (sockaddr *) &unix_addr;
#ifdef NDEBUG
    std::clog << socket_.c_type() << " socket created: " << socket_path << '\n' << std::flush;
#endif
}

void Connection::conn_memset() {
    std::memset(&socket_addr, '\0', sizeof(socket_addr));
    std::memset(&client_addr, '\0', sizeof(client_addr));
    std::memset(&unix_addr, '\0', sizeof(unix_addr));
}

Connection::~Connection() {
#ifdef NDEBUG
    std::clog << socket_.c_type() << " Shutdown: " << socket_.id() << '\n' << std::flush;
#endif
    //this->Shutdown(socket_.id());
}

bool Connection::Bind(bool listen) const {
    if (bind(socket_.id(), ptr_addr, sizeof(*ptr_addr)) < 0) {
        throw std::runtime_error(socket_.c_type() + " Bind failed, error number: "
                                 + std::to_string(errno));
    }
#ifdef NDEBUG
    if (socket_.c_type() != "UNIX")
    std::clog << socket_.c_type() << " Address Binded: " << inet_ntoa(socket_addr.sin_addr)
                  << ":" << ntohs(socket_addr.sin_port) << '\n' << std::flush;
#endif
    if (listen)
        this->Listen();

    return true;
}

bool Connection::Listen() const {
    if (listen(socket_.id(), SOMAXCONN) < 0) {
        throw std::runtime_error(socket_.c_type() + " Listen failed, error number: "
                                 + std::to_string(errno));
    }
#ifdef NDEBUG
    if (socket_.c_type() != "UNIX")
    std::clog << socket_.c_type() << " Address Listened: " << inet_ntoa(socket_addr.sin_addr)
                  << ":" << ntohs(socket_addr.sin_port) << '\n' << std::flush;
#endif

    return true;
}

int Connection::Accept() {
    socklen_t addrlen = sizeof(client_addr);
    transmission = accept(socket_.id(), (sockaddr *) &client_addr, &addrlen); //(socklen_t*)sizeof(client_addr));

    if (transmission < 0) {
        throw std::runtime_error(socket_.c_type() + " Accept failed, error number: "
                                 + std::to_string(errno));
    }
#ifdef NDEBUG
    std::clog << socket_.c_type() << " Client Connected: " << inet_ntoa(client_addr.sin_addr)
                  << ":" << ntohs(client_addr.sin_port) << '\n' << std::flush;
#endif
    return transmission;
}

bool Connection::Connect() {
    if (connect(socket_.id(), ptr_addr, sizeof(*ptr_addr)) < 0) {
        this->state = false;
        return false;
    }
#ifdef NDEBUG
    std::clog << socket_.c_type() << " Connected to: " << inet_ntoa(socket_addr.sin_addr)
                  << ":" << ntohs(socket_addr.sin_port) << '\n' << std::flush;
#endif
    this->state = true;
    return true;
}

void Connection::Shutdown(int id) const {
#ifdef NDEBUG
    std::clog << socket_.c_type() << " Shutdown: " << id << '\n' << std::flush;
#endif
    if (shutdown(id, SHUT_RDWR) < 0) {
        throw std::runtime_error("Shutdown failed, error number: "
                                 + std::to_string(errno));
    }
}

int Connection::id() const noexcept {
    return this->socket_.id();
}

bool Connection::status() const {
    return state;
}