#ifndef SOCK_CONNECT_USB_H
#define SOCK_CONNECT_USB_H

#include "include/connection.h"
#include <sys/ioctl.h>
#include <fcntl.h>
#include <termios.h>

class usb final : public connection
{
public:
    /**
     * @param address path to device {/dev/ttyACM0 | /dev/ttyUSB0}
     * @param speed connection_ dev_speed_: B9600, B57600, B115200
     */
    explicit usb(const std::string &address, speed_t speed);
    explicit usb(const char *address, speed_t speed);
    ~usb() override;

    /**
     * @brief Receive value
     * Virtual receiving function for values
     * @param value Address of value to receive
     * @param tu_size Bytes to receive
     * @return Count of received bytes
     */
    template <typename T>
    size_t receive(T *value, size_t tu_size);

    /**
     * @brief Send value
     * Virtual sending function for values
     * @param value Address of value to send
     * @param tu_size Bytes to send
     * @return Count of sent bytes
      */
    template <typename T>
    size_t send(T const *value, size_t tu_size);

    int accept(std::string *) override;
    bool connect() override;
    void listen() const override;
    void bind(bool) const override;
    void assign_thread(int id) override;

private:
    int get_descriptor() const override;
    void set_rts() const;
    void clr_rts() const;

private:
    std::string dev_path_;
    speed_t dev_speed_;
};

#endif // SOCK_CONNECT_USB_H
