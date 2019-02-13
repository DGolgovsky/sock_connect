#ifndef USB_SOCKET_H
#define USB_SOCKET_H

#include "Connection.h"

#include <fcntl.h>
#include <termios.h>
#include <sys/ioctl.h>

class USB {
private:
    int F_ID = -1;
    char buffer[1500];
    ssize_t msg_sz;
    std::string address;
    speed_t speed;
public:
    bool state{false};

public:
    /**
     * @param address path to device {/dev/ttyACM0 | /dev/ttyUSB0}
     * @param speed connection speed: B9600, B57600, B115200
     */
    USB(const std::string &address, speed_t speed);

    ~USB();

    /**
     * Connect/open port
     */
    bool Connect();

    void Shutdown();

    ssize_t Receive(std::string &message, std::size_t tu_size);

    ssize_t Receive(uint8_t *value, std::size_t tu_size);

    ssize_t Receive(uint16_t *value, std::size_t tu_size);

    ssize_t Receive(uint32_t *value, std::size_t tu_size);

    ssize_t Send(const std::string &value, std::size_t tu_size);

    ssize_t Send(const uint8_t *value, std::size_t tu_size);

    ssize_t Send(const uint16_t *value, std::size_t tu_size);

    ssize_t Send(const uint32_t *value, std::size_t tu_size);

    void setRTS();

    bool select_status();

    void clrRTS();

    int id() const;

    bool status() const;
};

#endif // USB_SOCKET_H
