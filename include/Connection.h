/**
 * @file Connection.h
 * @author DGolgovsky
 * @date 2018
 * @brief Socket Connection Class
 *
 * Class realize socket connection
 */

#ifndef SOCK_CONNECT_CONNECTION_H
#define SOCK_CONNECT_CONNECTION_H

#include <utility>
#include <map>
#include <vector>
#include <memory>
#include <thread>
#include <algorithm>

#include "SocketIp.h"

class Connection
{
	using storage_t = std::vector<std::pair<std::thread::id, int>>; // state for each connection

protected:
	SocketIp socket_;

	uint32_t address_{};
	uint16_t port_{};
	std::string m_path{};

	sockaddr_in socket_addr{};
	sockaddr_in client_addr{};
	sockaddr_un unix_addr{};
	sockaddr *ptr_addr;
	socklen_t size_addr;

	storage_t *clients{};
	bool state{false};

public:
	Connection(conn_type cp, uint32_t addr, uint16_t port);
	Connection(conn_type cp, char const *addr, uint16_t port);
	Connection(conn_type cp, std::string const &addr, uint16_t port);
	Connection(conn_type cp, char const &&addr, uint16_t port);
	Connection(conn_type cp, std::string const &path);
	Connection(conn_type cp, char const *path);

	/**
	 * @brief Deleted unused constructors and operator=
	 */
	Connection(Connection &) = delete;
	Connection(Connection &&) = delete;
	Connection(const Connection &) = delete;
	Connection &operator=(Connection &) = delete;
	Connection &operator=(const Connection &) = delete;
	Connection &operator=(Connection &&) = delete;

	virtual ~Connection();

	bool Bind(bool listen) const;
	bool Listen() const;

	int Accept(std::string *client_address);
	bool Connect();

	void Shutdown(int id);

	int id() noexcept;
	bool status() const;

	void assign_thread(int id);

protected:
    void conn_memset();
    int get_descriptor();
};

#endif // SOCK_CONNECT_CONNECTION_H