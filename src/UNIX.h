#ifndef _LIB_SOCK_CONNECT_UNIX_SOCKET_H
#define _LIB_SOCK_CONNECT_UNIX_SOCKET_H

#include "Connection.h"

class UNIX : public Connection
{
private:
	ssize_t msg_sz{};

public:
	explicit UNIX(std::string const &path);
	explicit UNIX(char const *path);
	~UNIX() override;

	/**
	 * @brief Receive value
	 * Virtual receiving function for values
	 * @param value
	 * @param tu_size Package size.
	 * @return message size
	 */
	template <typename T>
	ssize_t Receive(T *value, std::size_t tu_size);

	/**
	 * @brief Send value
	 * Virtual sending function for values
	 * @param value
	 * @param tu_size
	 * @return message size
	 */
	template <typename T>
	ssize_t Send(T const *value, std::size_t tu_size);
};

#endif // _LIB_SOCK_CONNECT_UNIX_SOCKET_H
