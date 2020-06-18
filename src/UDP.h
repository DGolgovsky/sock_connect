#ifndef _LIB_SOCK_CONNECT_UDP_SOCKET_H
#define _LIB_SOCK_CONNECT_UDP_SOCKET_H

#include "Connection.h"

class UDP : public Connection
{
private:
	/**
	 * The MTU (Maximum Transmission Unit) for Ethernet value is 1500 bytes
	 */
	char *buffer = nullptr;
	ssize_t msg_sz{};

public:
	explicit UDP(uint32_t address, uint16_t port);

	explicit UDP(const char *address, uint16_t port);

	explicit UDP(const std::string &address, uint16_t port);

	~UDP() override;

	/**
	 * @brief Receive int
	 * Virtual receiving function for values
	 * @param value
	 * @param tu_size
	 * @return message size
	 */
	template <typename T, typename S>
	ssize_t Receive(T *value, S tu_size);

	/**
	 * @brief Send value
	 * Virtual sending function for values
	 * @param value
	 * @param tu_size
	 * @return message size
	 */
	template <typename T, typename S>
	ssize_t Send(T const *value, S tu_size);
};

#endif // _LIB_SOCK_CONNECT_UDP_SOCKET_H
