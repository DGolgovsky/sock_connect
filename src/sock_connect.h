#ifndef _LIB_SOCK_CONNECT_SOCK_CONNECT_H
#define _LIB_SOCK_CONNECT_SOCK_CONNECT_H

#include "TCP.h"
#include "UDP.h"
#include "UNIX.h"

template <class Type>
class Connector
{
private:
	Type connection;

public:
	template <typename T, typename U>
	Connector(const T addr, U port) : connection(addr, port) {}

	~Connector() = default;

	int Accept() { return connection.Accept(); }

	bool Listen() const { return connection.Listen(); }

	bool Bind(bool listen = false) const { return connection.Bind(listen); }

	template <typename T>
	ssize_t Send(const T *value, std::size_t size) {
		return connection.Send(value, size);
	}

	template <typename T>
	ssize_t Receive(T *value, std::size_t size) {
		return connection.Receive(value, size);
	}

	bool Connect() { return connection.Connect(); }

	void Shutdown(int id = 0) { return connection.Shutdown(id); }

	int id() { return connection.id(); }

	bool status() const { return connection.status(); }

	void assign_thread(int id) { return connection.assign_thread(id); }
};

#endif // _LIB_SOCK_CONNECT_SOCK_CONNECT_H
