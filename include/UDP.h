#ifndef SOCK_CONNECT_UDP_H
#define SOCK_CONNECT_UDP_H

#include "Connection.h"

class UDP : public Connection
{
private:
	ssize_t msg_sz{};

public:
	explicit UDP(uint32_t address, uint16_t port);
	explicit UDP(const char *address, uint16_t port);
	explicit UDP(std::string const &address, uint16_t port);
	~UDP() override;

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

#endif // SOCK_CONNECT_UDP_H
