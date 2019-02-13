#ifndef SOCK_CONNECT_H
#define SOCK_CONNECT_H

#include "TCP.h"
#include "UDP.h"
#include "UNIX.h"
#include "USB.h"
#include "ip_to_int.h"


template<class Type>
class Connector {
private:
    Type connection;
public:
    Connector(uint32_t addr, uint16_t port)
            : connection(addr, port) {}

    Connector(const char *addr, uint16_t port)
            : connection(addr, port) {}

    Connector(const std::string &address, speed_t speed)
            : connection(address, speed) {}

    ~Connector() = default;

    /**
     * OVERRIDED FUNCTIONS
     */
    int Accept() {
        return connection.Accept();
    }

    bool Listen() const {
        return connection.Listen();
    }

    bool Bind(bool listen = false) const {
        return connection.Bind(listen);
    }

    template<typename T>
    ssize_t Send(const T *value, std::size_t size) {
        return connection.Send(value, size);
    }

    template<typename T>
    ssize_t Receive(T *value, std::size_t size) {
        return connection.Receive(value, size);
    }

    int Connect() {
        return connection.Connect();
    }

    void Shutdown(int id = 0) {
        return connection.Shutdown(id);
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
