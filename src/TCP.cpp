#include "TCP.h"

#ifndef NDEBUG
#include <iostream>
#endif

TCP::TCP(uint32_t address, uint16_t port)
		: Connection(_TCP, address, port) {
#ifndef NDEBUG
	debug_mutex.lock();
	std::clog << "[SOCK_CONNECT] TCP::TCP("
			  << "uint32_t address: " << address << ", "
			  << "uint16_t port: " << port << ")" << '\n' << std::flush;
	debug_mutex.unlock();
#endif
}

TCP::TCP(const char *address, uint16_t port)
		: Connection(_TCP, address, port) {
#ifndef NDEBUG
	debug_mutex.lock();
	std::clog << "[SOCK_CONNECT] TCP::TCP("
			  << "const char *address: " << address << ", "
			  << "uint16_t port: " << port << ")" << '\n' << std::flush;
	debug_mutex.unlock();
#endif
}

TCP::TCP(const std::string &address, uint16_t port)
		: Connection(_TCP, address.data(), port) {
#ifndef NDEBUG
	debug_mutex.lock();
	std::clog << "[SOCK_CONNECT] TCP::TCP("
			  << "const std::string &address: " << address << ", "
			  << "uint16_t port: " << port << ")" << '\n' << std::flush;
	debug_mutex.unlock();
#endif
}

TCP::~TCP() {
#ifndef NDEBUG
	debug_mutex.lock();
	std::clog << "[SOCK_CONNECT] TCP::~TCP()\n" << std::flush;
	debug_mutex.unlock();
#endif
}

template <>
ssize_t TCP::Receive(uint8_t *value, const std::size_t tu_size) {
	msg_sz = recv(get_descriptor(), value, tu_size, MSG_NOSIGNAL);
	if (msg_sz < 1) {
		this->state = false;
#ifndef NDEBUG
		debug_mutex.lock();
		std::clog << "[SOCK_CONNECT] TCP<uint8_t>::Receive(fd: " << get_descriptor()
				  << "): FAILED. Status - DISCONNECTED\n" << std::flush;
		debug_mutex.unlock();
#endif
		return 0;
	}
#ifndef NDEBUG
	debug_mutex.lock();
	std::clog << "[SOCK_CONNECT] TCP<uint8_t>::Receive: received <";
	print_values(value, tu_size);
	std::clog << " [+" << tu_size << "]>\n" << std::flush;
	debug_mutex.unlock();
#endif
	return msg_sz;
}

template <>
ssize_t TCP::Receive(uint16_t *value, const std::size_t tu_size) {
	msg_sz = recv(get_descriptor(), value, tu_size, MSG_NOSIGNAL);
	if (msg_sz < 1) {
		this->state = false;
#ifndef NDEBUG
		debug_mutex.lock();
		std::clog << "[SOCK_CONNECT] TCP<uint16_t>::Receive(fd: " << get_descriptor()
				  << "): FAILED. Status - DISCONNECTED\n" << std::flush;
		debug_mutex.unlock();
#endif
		return 0;
	}
#ifndef NDEBUG
	debug_mutex.lock();
	std::clog << "[SOCK_CONNECT] TCP<uint16_t>::Receive: received <";
	print_values(value, tu_size);
	std::clog << " [+" << tu_size << "]>\n" << std::flush;
	debug_mutex.unlock();
#endif
	return msg_sz;
}

template <>
ssize_t TCP::Receive(uint32_t *value, const std::size_t tu_size) {
	msg_sz = recv(get_descriptor(), value, tu_size, MSG_NOSIGNAL);
	if (msg_sz < 1) {
		this->state = false;
#ifndef NDEBUG
		debug_mutex.lock();
		std::clog << "[SOCK_CONNECT] TCP<uint32_t>::Receive(fd: " << get_descriptor()
				  << "): FAILED. Status - DISCONNECTED\n" << std::flush;
		debug_mutex.unlock();
#endif
		return 0;
	}
#ifndef NDEBUG
	debug_mutex.lock();
	std::clog << "[SOCK_CONNECT] TCP<uint32_t>::Receive: received <";
	print_values(value, tu_size);
	std::clog << " [+" << tu_size << "]>\n" << std::flush;
	debug_mutex.unlock();
#endif
	return msg_sz;
}

template <>
ssize_t TCP::Receive(std::size_t *value, const std::size_t tu_size) {
	msg_sz = recv(get_descriptor(), value, tu_size, MSG_NOSIGNAL);
	if (msg_sz < 1) {
		this->state = false;
#ifndef NDEBUG
		debug_mutex.lock();
		std::clog << "[SOCK_CONNECT] TCP<size_t>::Receive(fd: " << get_descriptor()
				  << "): FAILED. Status - DISCONNECTED\n" << std::flush;
		debug_mutex.unlock();
#endif
		return 0;
	}
#ifndef NDEBUG
	debug_mutex.lock();
	std::clog << "[SOCK_CONNECT] TCP<size_t>::Receive: received <";
	print_values(value, tu_size);
	std::clog << " [+" << tu_size << "]>\n" << std::flush;
	debug_mutex.unlock();
#endif
	return msg_sz;
}

template <>
ssize_t TCP::Receive(char *value, const std::size_t tu_size) {
	auto recv_left = tu_size;
	std::size_t total = 0;
	while (total < tu_size) {
		msg_sz = recv(get_descriptor(), value + total, recv_left, MSG_NOSIGNAL);
		if (msg_sz < 1) {
			this->state = false;
#ifndef NDEBUG
			debug_mutex.lock();
			std::clog << "[SOCK_CONNECT] TCP<char*>::Receive(fd: " << get_descriptor()
					  << "): FAILED. Status - DISCONNECTED\n" << std::flush;
			debug_mutex.unlock();
#endif
			value[total] = '\0';
			return 0;
		}
		recv_left -= msg_sz;
		total += msg_sz;
	}
#ifndef NDEBUG
	debug_mutex.lock();
	std::clog << "[SOCK_CONNECT] TCP<char*>::Receive: received <[" << (total < 32 ? value : "DATA SHORTENED")
			  << "] [+" << total << "]>\n" << std::flush;
	debug_mutex.unlock();
#endif
	value[total] = '\0';
	return total;
}

template <>
ssize_t TCP::Receive(std::string *value, const std::size_t tu_size) {
	if (!tu_size)
		return 0;
	std::unique_ptr<char[]> ptr_buffer(new char[tu_size * 2]());
	msg_sz = TCP::Receive(ptr_buffer.get(), tu_size);
	ptr_buffer[msg_sz] = '\0';
	*value = std::string(ptr_buffer.get(), static_cast<std::size_t>(msg_sz));
	value->shrink_to_fit();

	return msg_sz;
}

template <>
ssize_t TCP::Send(const uint8_t *value, const std::size_t tu_size) {
	if ((msg_sz = send(get_descriptor(), value, tu_size, MSG_NOSIGNAL)) < 1) {
		this->state = false;
#ifndef NDEBUG
		debug_mutex.lock();
		std::clog << "[SOCK_CONNECT] TCP<uint8_t>::Send(fd: " << get_descriptor()
				  << "): FAILED. Status - DISCONNECTED\n" << std::flush;
		debug_mutex.unlock();
#endif
		return 0;
	}
#ifndef NDEBUG
	debug_mutex.lock();
	std::clog << "[SOCK_CONNECT] TCP<uint8_t>::Send: sent <";
	print_values(value, tu_size);
	std::clog << " [+" << tu_size << "]>\n" << std::flush;
	debug_mutex.unlock();
#endif
	return msg_sz;
}

template <>
ssize_t TCP::Send(const uint16_t *value, const std::size_t tu_size) {
	if ((msg_sz = send(get_descriptor(), value, tu_size, MSG_NOSIGNAL)) < 1) {
		this->state = false;
#ifndef NDEBUG
		debug_mutex.lock();
		std::clog << "[SOCK_CONNECT] TCP<uint16_t>::Send(fd: " << get_descriptor()
				  << "): FAILED. Status - DISCONNECTED\n" << std::flush;
		debug_mutex.unlock();
#endif
		return 0;
	}
#ifndef NDEBUG
	debug_mutex.lock();
	std::clog << "[SOCK_CONNECT] TCP<uint16_t>::Send: sent <";
	print_values(value, tu_size);
	std::clog << " [+" << tu_size << "]>\n" << std::flush;
	debug_mutex.unlock();
#endif
	return msg_sz;
}

template <>
ssize_t TCP::Send(const uint32_t *value, const std::size_t tu_size) {
	if ((msg_sz = send(get_descriptor(), value, tu_size, MSG_NOSIGNAL)) < 1) {
		this->state = false;
#ifndef NDEBUG
		debug_mutex.lock();
		std::clog << "[SOCK_CONNECT] TCP<uint32_t>::Send(fd: " << get_descriptor()
				  << "): FAILED. Status - DISCONNECTED\n" << std::flush;
		debug_mutex.unlock();
#endif
		return 0;
	}
#ifndef NDEBUG
	debug_mutex.lock();
	std::clog << "[SOCK_CONNECT] TCP<uint32_t>::Send: sent <";
	print_values(value, tu_size);
	std::clog << " [+" << tu_size << "]>\n" << std::flush;
	debug_mutex.unlock();
#endif
	return msg_sz;
}

template <>
ssize_t TCP::Send(const std::size_t *value, const std::size_t tu_size) {
	if ((msg_sz = send(get_descriptor(), value, tu_size, MSG_NOSIGNAL)) < 1) {
		this->state = false;
#ifndef NDEBUG
		debug_mutex.lock();
		std::clog << "[SOCK_CONNECT] TCP<size_t>::Send(fd: " << get_descriptor()
				  << "): FAILED. Status - DISCONNECTED\n" << std::flush;
		debug_mutex.unlock();
#endif
		return 0;
	}
#ifndef NDEBUG
	debug_mutex.lock();
	std::clog << "[SOCK_CONNECT] TCP<size_t>::Send: sent <";
	print_values(value, tu_size);
	std::clog << " [+" << tu_size << "]>\n" << std::flush;
	debug_mutex.unlock();
#endif
	return msg_sz;
}

template <>
ssize_t TCP::Send(char const *value, std::size_t const tu_size) {
	/**
	 * TCP MTU = 1460
	 * net.core.rmem_max = 212992
	 */
	auto send_left = tu_size;
	std::size_t total = 0;
	while (send_left > 0) {
		if ((msg_sz = send(get_descriptor(), value + total, send_left, MSG_NOSIGNAL)) < 1) {
			this->state = false;
#ifndef NDEBUG
			debug_mutex.lock();
			std::clog << "[SOCK_CONNECT] TCP<char*>::Send(fd: " << get_descriptor()
					  << "): FAILED. Status - DISCONNECTED\n" << std::flush;
			debug_mutex.unlock();
#endif
			return 0;
		}
		send_left -= msg_sz;
		total += msg_sz;
	}
#ifndef NDEBUG
	debug_mutex.lock();
	std::clog << "[SOCK_CONNECT] TCP<char*>::Send: sent <[" << (total < 32 ? value : "DATA SHORTENED")
			  << "] [+" << total << "]>\n" << std::flush;
	debug_mutex.unlock();
#endif
	return total;
}

template <>
ssize_t TCP::Send(std::string const *value, std::size_t const tu_size) {
	return TCP::Send(value->c_str(), tu_size);
}