#include <cstring>
#include <algorithm>

#include "Connection.h"

#ifdef NDEBUG
#include <iostream>
#endif

Connection::Connection(conn_type cp, uint32_t address, uint16_t port)
        : socket_(cp)
        , address_(address)
        , port_(port)
{
    std::memset(&socket_addr, '\0', sizeof(socket_addr));
    std::memset(&client_addr, '\0', sizeof(client_addr));
    socket_addr.sin_family = AF_INET;
    socket_addr.sin_addr.s_addr = htonl(address_);
    socket_addr.sin_port = htons(port_);
}

Connection::Connection(conn_type cp, std::string &path)
        : socket_(cp)
        , m_path(path)
{
    std::memset(&socket_addr, '\0', sizeof(socket_addr));
    std::memset(&client_addr, '\0', sizeof(client_addr));
    unix_addr.sun_family = AF_UNIX;
}

Connection::~Connection()
{
    shutdown(socket_.id(), SHUT_RDWR);
}

int Connection::Accept() const
{
    int client_id = accept(socket_.id()
            , (sockaddr*)&client_addr
            , (socklen_t*)sizeof(client_addr));

    if (client_id < 0) {
        throw std::runtime_error("Accept failed, error number: "
                                 + std::to_string(errno));
    }
#ifdef NDEBUG
    std::clog << "Client Connected: " << inet_ntoa(client_addr.sin_addr)
                  << ":" << ntohs(client_addr.sin_port) << '\n' << std::flush;
#endif
    return client_id;
}

bool Connection::Bind(bool listen) const
{
    if (bind(socket_.id(), (sockaddr*) &socket_addr, sizeof(socket_addr)) < 0) {
        /*throw std::runtime_error("_TCP Bind failed, error number: "
                                 + std::to_string(errno));*/
#ifdef NDEBUG
        std::clog << "Binding failed: " << '\n' << std::flush;
#endif
        return false;
    }

    if (listen)
        this->Listen();

    return true;
}

bool Connection::Listen() const
{
    if (listen(socket_.id(), SOMAXCONN) < 0) {
        /*throw std::runtime_error("_TCP Listen failed, error number: "
                                 + std::to_string(errno));*/
#ifdef NDEBUG
        std::clog << "Listen failed: " << '\n' << std::flush;
#endif
        return false;
    }

    return true;
}

bool Connection::Connect()
{
    receiver_id = connect(socket_.id(), (sockaddr*)(&socket_addr), sizeof(socket_addr));

    if (receiver_id < 0) {
        shutdown(receiver_id, SHUT_RDWR);
        this->state = false;
        return false;
    }
#ifdef NDEBUG
    std::clog << "Connected to: " << inet_ntoa(socket_addr.sin_addr)
                  << ":" << ntohs(socket_addr.sin_port) << '\n' << std::flush;
#endif
    this->state = true;

    return true;
}

void Connection::Shutdown() const
{
#ifdef NDEBUG
    std::clog << "Disconnected: " << inet_ntoa(socket_addr.sin_addr)
                  << ":" << ntohs(socket_addr.sin_port) << '\n' << std::flush;
#endif
    if (shutdown(socket_.id(), SHUT_RDWR) < 0) {
        throw std::runtime_error("Shutdown failed, error number: "
                                 + std::to_string(errno));
    }
}

int Connection::id() const noexcept
{
    return this->socket_.id();
}

bool Connection::status() const
{
    return state;
}
