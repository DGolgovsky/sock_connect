#ifndef UNIX_SOCKET_H
#define UNIX_SOCKET_H

#include "Connection.h"


class UNIX : public Connection {
private:
    /*
     * The MTU (Maximum Transmission Unit) for Ethernet value is 1500 bytes
     */
    char buffer[1500];
    ssize_t msg_sz;

public:
    explicit UNIX(std::string &path);

    ~UNIX() override;

    ssize_t Receive(uint8_t *value, std::size_t tu_size);

    ssize_t Receive(uint16_t *value, std::size_t tu_size);

    ssize_t Receive(uint32_t *value, std::size_t tu_size);

    ssize_t Receive(std::string &message, std::size_t tu_size);

    ssize_t Send(const uint8_t *value, std::size_t tu_size);

    ssize_t Send(const uint16_t *value, std::size_t tu_size);

    ssize_t Send(const uint32_t *value, std::size_t tu_size);

    ssize_t Send(const std::string &message, std::size_t tu_size);
};

#endif // UNIX_SOCKET_H
