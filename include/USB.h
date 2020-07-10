#ifndef SOCK_CONNECT_USB_H
#define SOCK_CONNECT_USB_H

#include "Connection.h"
#include <termios.h>

class USB
{
private:
	int fd = -1;
	ssize_t msg_sz{-1};
	std::string m_address;
	speed_t m_speed;
public:
	bool state{false};

public:
	/**
	 * @param address path to device {/dev/ttyACM0 | /dev/ttyUSB0}
	 * @param speed connection m_speed: B9600, B57600, B115200
	 */
	explicit USB(std::string address, speed_t speed);
	~USB();

	/**
	 * Connect/open port
	 */
	bool Connect();
	void Shutdown();

	template <typename T>
	ssize_t Receive(T *value, std::size_t tu_size);

	template <typename T>
	ssize_t Send(T const *value, std::size_t tu_size);

	void setRTS() const;
	void clrRTS() const;

	int id() const;
	bool status() const;
};

#endif // SOCK_CONNECT_USB_H
