#ifndef _LIB_SOCK_CONNECT_UNIX_SOCKET_H
#define _LIB_SOCK_CONNECT_UNIX_SOCKET_H

#include "Connection.h"

class UNIX : public Connection
{
private:
    /**
     * The MTU (Maximum Transmission Unit) for Ethernet value is 1500 bytes
     */
    char buffer[1500]{};
    ssize_t msg_sz{};

public:
    explicit UNIX(std::string &path);
    ~UNIX() override;

    template <typename T, typename S>
    ssize_t Receive(T *value, S const tu_size);

    template <typename T, typename S>
    ssize_t Send(const T *value, S const tu_size);
};


#endif // _LIB_SOCK_CONNECT_UNIX_SOCKET_H
