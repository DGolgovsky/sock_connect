#ifndef TCP_SOCKET_H
#define TCP_SOCKET_H

#include "Connection.h"


class TCP : public Connection {
private:
    /*
     * The MTU (Maximum Transmission Unit) for Ethernet value is 1500 bytes
     */
    char buffer[1500];
    ssize_t msg_sz;

public:
    explicit TCP(uint32_t address, uint16_t port);

    ~TCP() override;

    /**
     * @brief Receive int
     * Virtual receiving function for int values
     * @param value
     * @param tu_size Package size. By default is sizeof(int)
     * @return message size
     */
    ssize_t Receive(uint8_t *value, std::size_t tu_size);

    ssize_t Receive(uint16_t *value, std::size_t tu_size);

    ssize_t Receive(uint32_t *value, std::size_t tu_size);

    /**
     * @brief Receive std::string
     * Virtual receiving function for char
     * @param message Reference to message var
     * @param tu_size Packet size. Maximum: sysctl -n net.core.rmem_max
     * @return message size
     */
    ssize_t Receive(std::string &message, std::size_t tu_size);

    /**
     * @brief Send int value
     * Virtual sending function for int values
     * @param value
     * @return message size
     */
    ssize_t Send(const uint8_t *value, std::size_t tu_size);

    ssize_t Send(const uint16_t *value, std::size_t tu_size);

    ssize_t Send(const uint32_t *value, std::size_t tu_size);

    /**
     * @brief Send std::string
     * Virtual sending function for std::string
     * @param message
     * @return message size
     */
    ssize_t Send(const std::string &message, std::size_t tu_size);
};

#endif // TCP_SOCKET_H
