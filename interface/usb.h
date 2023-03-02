#ifndef SOCK_CONNECT_USB_H
#define SOCK_CONNECT_USB_H

#include "include/connection.h"
#include <sys/ioctl.h>
#include <fcntl.h>
#include <termios.h>

class usb final
{
public:
    /**
     * @param address path to device {/dev/ttyACM0 | /dev/ttyUSB0}
     * @param speed connection_ m_speed_: B9600, B57600, B115200
     */
    explicit usb(const std::string &address, speed_t speed);
    explicit usb(const char *address, speed_t speed);
    ~usb();

    /**
     * @brief Receive value
     * Virtual receiving function for values
     * @param value Address of value to receive
     * @param tu_size Bytes to receive
     * @return Count of received bytes
     */
    template<typename T>
    size_t receive(T *value, size_t tu_size);

    /**
     * @brief Send value
     * Virtual sending function for values
     * @param value Address of value to send
     * @param tu_size Bytes to send
     * @return Count of sent bytes
      */
    template<typename T>
    size_t send(T const *value, size_t tu_size);

    /**
     * connect/open port
     */
    bool connect();
    void shutdown();
    void shutdown(int id);
    bool status() const;

    /** Functions below here for compatibility */
    int id() const;
    int accept(const std::string &) const;
    void listen() const;
    void bind(bool) const;
    void assign_thread(int id) const;

private:
    int get_descriptor() const;
    void set_rts() const;
    void clr_rts() const;

private:
    int fd_ = -1;
    ssize_t msg_sz_{};
    std::string m_address_;
    speed_t m_speed_;
    bool state_{false};
};

#endif // SOCK_CONNECT_USB_H
