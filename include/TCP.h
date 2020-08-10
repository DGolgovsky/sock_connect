#ifndef SOCK_CONNECT_TCP_H
#define SOCK_CONNECT_TCP_H

#include "Connection.h"

class TCP : public Connection
{
private:
	ssize_t msg_sz{};

public:
	explicit TCP(uint32_t address, 	  uint16_t port);
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
	template <typename T>
	std::size_t Receive(T *value, std::size_t tu_size);

	/**
	 * @brief Send value
	 * Virtual sending function for values
	 * @param value
	 * @param tu_size
	 * @return message size
	 */
	template <typename T>
	std::size_t Send(T const *value, std::size_t tu_size);
};

#endif // SOCK_CONNECT_TCP_H
