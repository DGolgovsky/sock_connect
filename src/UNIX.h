#ifndef _LIB_SOCK_CONNECT_UNIX_SOCKET_H
#define _LIB_SOCK_CONNECT_UNIX_SOCKET_H

#include "Connection.h"

class UNIX : public Connection
{
private:
	/**
	 * The MTU (Maximum Transmission Unit) for Ethernet value is 1500 bytes
	 */
	char buffer[1500]{};
	ssize_t msg_sz{};

public:
	explicit UNIX(std::string &path);

	~UNIX() override;

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
	ssize_t Send(const T *value, S tu_size);
};

#endif // _LIB_SOCK_CONNECT_UNIX_SOCKET_H
