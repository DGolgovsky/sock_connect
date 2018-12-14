#ifndef UDP_SOCKET_H
#define UDP_SOCKET_H

#include "Connection.h"

class UDP : public Connection
{
private:
    /**
     * The MTU (Maximum Transmission Unit) for Ethernet value is 1500 bytes
     */
    char buffer[1500];
    ssize_t msg_sz;

public:
    explicit UDP(uint32_t address, uint16_t port);

    ~UDP() override;

    /**
     * @brief Receive int, UDP
     * Virtual receiving function for int values as UDP connections without size
     * @param value
     * @return message size
     */
    ssize_t Receive(uint8_t *value, std::size_t tu_size);

    /**
     * @brief Receive std::string, UDP
     * Virtual receiving function for std::string messages as UDP connections without size
     * @param value
     * @return message size
     */
    ssize_t Receive(std::string &message, std::size_t tu_size);

    /**
     * @brief Send int value
     * Virtual sending function for int values
     * @param value
     * @return message size
     */
    ssize_t Send(const uint8_t *value, std::size_t);

    /**
     * @brief Send std::string
     * Virtual sending function for std::string
     * @param message
     * @return message size
     */
    ssize_t Send(const std::string &message, std::size_t);
};

#endif // UDP_SOCKET_H
