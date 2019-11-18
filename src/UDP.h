#ifndef _LIB_SOCK_CONNECT_UDP_SOCKET_H
#define _LIB_SOCK_CONNECT_UDP_SOCKET_H

#include "Connection.h"

class UDP : public Connection
{
private:
    /**
     * The MTU (Maximum Transmission Unit) for Ethernet value is 1500 bytes
     */
    char buffer[1500]{};
    ssize_t msg_sz{};

public:
    explicit UDP(uint32_t address, uint16_t port);
    explicit UDP(const char* address, uint16_t port);
    ~UDP() override;

    /**
     * @brief Receive int, UDP
     * Virtual receiving function for int values as UDP connections without size
     * @param value
     * @param tu_size
     * @return message size
     */
    template <typename T, typename S>
    ssize_t Receive(T *value, S const tu_size);

    /**
     * @brief Send int value
     * Virtual sending function for int values
     * @param value
     * @param tu_size
     * @return message size
     */
    template <typename T, typename S>
    ssize_t Send(T const *value, S const tu_size);
};

#endif // _LIB_SOCK_CONNECT_UDP_SOCKET_H
