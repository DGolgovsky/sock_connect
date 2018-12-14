#ifndef SOCK_CONNECT_H
#define SOCK_CONNECT_H

#include "TCP.h"
#include "UDP.h"
#include "UNIX.h"
#include "USB.h"

template<class Type>
class Connector
{
private:
    Type connection;
public:
    Connector(uint32_t addr, uint16_t port)
            : connection(addr, port) {}

    Connector(const std::string &address, speed_t speed)
            : connection(address, speed) {}

    ~Connector() = default;

    /**
     * OVERRIDED FUNCTIONS
     */
    int Accept() const {
        return connection.Accept();
    }

    bool Listen() const {
        return connection.Listen();
    }

    bool Bind(bool listen) const {
        return connection.Bind(listen);
    }

    ssize_t Send(const uint8_t *value, std::size_t size) {
        return connection.Send(value, size);
    }

    ssize_t Send(const std::string &value, std::size_t size) {
        return connection.Send(value, size);
    }

    ssize_t Receive(uint8_t *value, std::size_t size) {
        return connection.Receive(value, size);
    }

    ssize_t Receive(std::string &value, std::size_t size) {
        return connection.Receive(value, size);
    }

    bool Connect() {
        return connection.Connect();
    }

    void Shutdown() {
        return connection.Shutdown();
    }

    void setRTS() {
        return connection.setRTS();
    }

    void clrRTS() {
        return connection.clrRTS();
    }

    int id() const {
        return connection.id();
    }

    bool status() const {
        return connection.status();
    }

};

#endif // SOCK_CONNECT_H
