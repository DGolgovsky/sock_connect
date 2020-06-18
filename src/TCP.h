#ifndef _LIB_SOCK_CONNECT_TCP_SOCKET_H
#define _LIB_SOCK_CONNECT_TCP_SOCKET_H

#include "Connection.h"

class TCP : public Connection
{
private:
	/**
	 * The MTU (Maximum Transmission Unit) for Ethernet value is 1500 bytes
	 */
	char *buffer = nullptr;
	ssize_t msg_sz{};

public:
	explicit TCP(uint32_t address, uint16_t port);

	explicit TCP(const char *address, uint16_t port);

	explicit TCP(std::string const &address, uint16_t port);

	~TCP() override;

	/**
	 * @brief Receive value
	 * Virtual receiving function for values
	 * @param value
	 * @param tu_size Package size.
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

#endif // _LIB_SOCK_CONNECT_TCP_SOCKET_H
