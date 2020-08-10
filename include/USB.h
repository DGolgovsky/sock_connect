#ifndef SOCK_CONNECT_USB_H
#define SOCK_CONNECT_USB_H

#include "Connection.h"
#include <sys/ioctl.h>
#include <fcntl.h>
#include <termios.h>

class USB
{
private:
	int fd = -1;
	ssize_t msg_sz{};
	std::string m_address;
	speed_t m_speed;
	bool state{true};

public:
	/**
	 * @param address path to device {/dev/ttyACM0 | /dev/ttyUSB0}
	 * @param speed connection m_speed: B9600, B57600, B115200
	 */
	explicit USB(const std::string &address, speed_t speed);
	explicit USB(const char *address, 		 speed_t speed);
	~USB();

	/**
	 * Connect/open port
	 */
	bool Connect();
	void Shutdown();
	void Shutdown(int id);

	template <typename T>
	std::size_t Receive(T *value, std::size_t tu_size);

	template <typename T>
	std::size_t Send(T const *value, std::size_t tu_size);

	bool status() const;

	/**
	 * Current here for compability
	 * Do the same as get_descriptor
	 * @return fd
	 */
	int id() const;
	int Accept(std::string);
	bool Listen() const;
	bool Bind() const;
	void assign_thread(int id);

private:
	int get_descriptor() const;
	void setRTS() const;
	void clrRTS() const;
};

#endif // SOCK_CONNECT_USB_H
