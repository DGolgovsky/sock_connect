#ifndef _LIB_SOCK_CONNECT_SOCK_CONNECT_H
#define _LIB_SOCK_CONNECT_SOCK_CONNECT_H

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
    template<typename T, typename U>
    Connector(const T addr, U port) : connection(addr, port) {}
    ~Connector() = default;

    int Accept() { return connection.Accept(); }
    bool Listen() const { return connection.Listen(); }
    bool Bind(bool listen = false) const { return connection.Bind(listen); }

    template<typename T>
    ssize_t Send(const T *value, std::size_t size) {
        return connection.Send(value, size);
    }

    template<typename T>
    ssize_t Receive(T *value, std::size_t size) {
        return connection.Receive(value, size);
    }

    int Connect() { return connection.Connect(); }
    void Shutdown(int id = 0) { return connection.Shutdown(id); }

    void setRTS() { return connection.setRTS(); }
    void clrRTS() { return connection.clrRTS(); }

    int id() { return connection.id(); }
    bool status() const { return connection.status(); }

    void assign_client(int id = 0) { return connection.assign_client(id); }
};

#endif // _LIB_SOCK_CONNECT_SOCK_CONNECT_H
