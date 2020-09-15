#ifndef SOCK_CONNECT_UNIX_H
#define SOCK_CONNECT_UNIX_H

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
	 * @param value Address of value to receive
	 * @param tu_size Bytes to receive
	 * @return Count of received bytes
	 */
	template <typename T>
	std::size_t Receive(T *value, std::size_t tu_size);

	/**
	 * @brief Send value
	 * Virtual sending function for values
	 * @param value Address of value to send
	 * @param tu_size Bytes to send
	 * @return Count of sent bytes
 	 */
	template <typename T>
	std::size_t Send(T const *value, std::size_t tu_size);
};

#endif // SOCK_CONNECT_UNIX_H
