#ifndef SOCK_CONNECT_UDP_H
#define SOCK_CONNECT_UDP_H

#include "include/connection.h"

class udp: public connection
{
public:
    explicit udp(uint32_t address, uint16_t port);
    explicit udp(const char *address, uint16_t port);
    explicit udp(std::string const &address, uint16_t port);
    ~udp() override;

    /**
     * @brief receive value
     * Virtual receiving function for values
     * @param value Address of value to receive
     * @param tu_size Bytes to receive
     * @return Count of received bytes
     */
    template<typename T>
    size_t receive(T *value, size_t tu_size);

    /**
     * @brief send value
     * Virtual sending function for values
     * @param value Address of value to send
     * @param tu_size Bytes to send
     * @return Count of sent bytes
     */
    template<typename T>
    size_t send(T const *value, size_t tu_size);

private:
    ssize_t msg_sz_{};
};

#endif // SOCK_CONNECT_UDP_H
