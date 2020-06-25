#ifndef _LIB_SOCK_CONNECT_USB_SOCKET_H
#define _LIB_SOCK_CONNECT_USB_SOCKET_H

#include "Connection.h"
#include <termios.h>

class USB
{
private:
	int fd = -1;
	char buffer[1500]{};
	ssize_t msg_sz{-1};
	std::string address;
	speed_t speed;
public:
	bool state{false};

public:
	/**
	 * @param address path to device {/dev/ttyACM0 | /dev/ttyUSB0}
	 * @param speed connection speed: B9600, B57600, B115200
	 */
	explicit USB(std::string address, speed_t speed);
	~USB();

	/**
	 * Connect/open port
	 */
	bool Connect();
	void Shutdown();

	template <typename T>
	ssize_t Receive(T *value, std::size_t const tu_size);

	template <typename T>
	ssize_t Send(T const *value, std::size_t const tu_size);

	void setRTS() const;
	void clrRTS() const;

	int id() const;
	bool status() const;
};

#endif // _LIB_SOCK_CONNECT_USB_SOCKET_H
