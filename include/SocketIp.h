/**
 * @file SocketIp.h
 * @author DGolgovsky
 * @date 2018
 * @brief Socket Connection support Class
 *
 * Class realize socket connection
 */

#ifndef SOCK_CONNECT_SOCKETIP_H
#define SOCK_CONNECT_SOCKETIP_H

#include <arpa/inet.h>
#include <netinet/in.h>

#include <sys/socket.h>
#include <sys/un.h>

#include <unistd.h>
#include <stdexcept>

#ifndef NDEBUG
#include <mutex>
#include <iostream>

static std::mutex debug_mutex;

template <typename T>
std::string print_values(T *val, std::size_t sz) {
	std::string os{};
	sz = sz / sizeof(T);
	if (sz < 17) {
		os.append("[");
		for (std::size_t i = 0; i < sz; i++) {
			if (i != 0 && i != sz)
				os.append("; ");
			os += std::to_string(*(val + i));
		}
		os.append("]");
	} else {
		os = "[DATA SHORTENED]";
	}
	return os;
}
#endif

enum conn_type : char
{
	_TCP, _UDP, _UNIX
};

class SocketIp
{
private:
	int m_id{};
	std::string conn_text{};

public:
	explicit SocketIp(conn_type ct) {
		switch (ct) {
			case _TCP  :
				m_id = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
				conn_text = "TCP";
				break;
			case _UDP  :
				m_id = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
				conn_text = "UDP";
				break;
			case _UNIX :
				m_id = socket(AF_UNIX, SOCK_STREAM, 0);
				conn_text = "UNIX";
				break;
		}

		if (m_id < 0) {
			throw std::runtime_error("Socket was not created, error number: "
									 + std::to_string(errno));
		}
	}

	explicit SocketIp(int socket_id)
			: m_id(socket_id) {}

	SocketIp(SocketIp &&other) noexcept
			: m_id(other.m_id) {
		other.m_id = -1;
	}

	SocketIp(const SocketIp &) = delete;

	SocketIp &operator=(const SocketIp &) = delete;

	SocketIp &operator=(SocketIp &&) = delete;

	~SocketIp() {
		if (m_id >= 0) {
			shutdown(m_id, SHUT_RDWR);
		}
	}

	int id() const noexcept {
		return m_id;
	}

	std::string c_type() const noexcept {
		return conn_text;
	}
};

#endif // SOCK_CONNECT_SOCKETIP_H
