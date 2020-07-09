#ifndef SOCK_CONNECT_SOCK_CONNECT_H
#define SOCK_CONNECT_SOCK_CONNECT_H

#include "TCP.h"
#include "UDP.h"
#include "UNIX.h"

template <class Type>
class Connector final
{
private:
	Type connection;

public:
	template <typename T>
	explicit Connector(T sun_path) : connection(sun_path) {}
	template <typename T, typename U>
	Connector(T addr, U port) : connection(addr, static_cast<uint16_t>(port)) {}
	~Connector() = default;

	int Accept() { return connection.Accept(); }
	bool Listen() const { return connection.Listen(); }
	bool Bind(bool listen = true) const { return connection.Bind(listen); }
	bool Connect() { return connection.Connect(); }
	void Shutdown(int id = 0) { return connection.Shutdown(id); }

	template <typename T>
	ssize_t Send(const T *value, std::size_t size) {
		return connection.Send(value, size);
	}

	template <typename T>
	ssize_t Receive(T *value, std::size_t size) {
		return connection.Receive(value, size);
	}

	int id() { return connection.id(); }
	bool Status() const { return connection.status(); }
	void assign_thread(int id) { return connection.assign_thread(id); }
};

#endif // SOCK_CONNECT_SOCK_CONNECT_H
