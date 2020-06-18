#include "UDP.h"

#ifndef NDEBUG
#include <iostream>
#endif

UDP::UDP(uint32_t address, uint16_t port)
		: Connection(_UDP, address, port) {
#ifndef NDEBUG
	debug_mutex.lock();
	std::clog << "[SOCK_CONNECT] UDP::UDP("
			  << "uint32_t address: " << address << ", "
			  << "uint16_t port: " << port << ")" << '\n' << std::flush;
	debug_mutex.unlock();
#endif
}

UDP::UDP(const char *address, uint16_t port)
		: Connection(_UDP, address, port) {
#ifndef NDEBUG
	debug_mutex.lock();
	std::clog << "[SOCK_CONNECT] UDP::UDP("
			  << "const char *address: " << address << ", "
			  << "uint16_t port: " << port << ")" << '\n' << std::flush;
	debug_mutex.unlock();
#endif
}

UDP::UDP(const std::string &address, uint16_t port)
		: Connection(_UDP, address.data(), port) {
#ifndef NDEBUG
	debug_mutex.lock();
	std::clog << "[SOCK_CONNECT] UDP::UDP("
			  << "const std::string &address: " << address << ", "
			  << "uint16_t port: " << port << ")" << '\n' << std::flush;
	debug_mutex.unlock();
#endif
}

UDP::~UDP() {
#ifndef NDEBUG
	debug_mutex.lock();
	std::clog << "[SOCK_CONNECT] UDP::~UDP()\n" << std::flush;
	debug_mutex.unlock();
#endif
}

template <>
ssize_t UDP::Receive(uint8_t *value, const std::size_t tu_size) {
	msg_sz = recvfrom(get_descriptor(), value, tu_size, 0,
					  ptr_addr, &size_addr);
#ifndef NDEBUG
	debug_mutex.lock();
	std::clog << "[SOCK_CONNECT] UDP<uint8_t>::Receive: received <";
	print_values(value, tu_size);
	std::clog << " [+" << tu_size << "]>\n" << std::flush;
	debug_mutex.unlock();
#endif
	return msg_sz;
}

template <>
ssize_t UDP::Receive(uint16_t *value, const std::size_t tu_size) {
	msg_sz = recvfrom(get_descriptor(), value, tu_size, 0,
					  ptr_addr, &size_addr);

#ifndef NDEBUG
	debug_mutex.lock();
	std::clog << "[SOCK_CONNECT] UDP<uint16_t>::Receive: received <";
	print_values(value, tu_size);
	std::clog << " [+" << tu_size << "]>\n" << std::flush;
	debug_mutex.unlock();
#endif
	return msg_sz;
}

template <>
ssize_t UDP::Receive(uint32_t *value, const std::size_t tu_size) {
	msg_sz = recvfrom(get_descriptor(), value, tu_size, 0,
					  ptr_addr, &size_addr);

#ifndef NDEBUG
	debug_mutex.lock();
	std::clog << "[SOCK_CONNECT] UDP<uint32_t>::Receive: received <";
	print_values(value, tu_size);
	std::clog << " [+" << tu_size << "]>\n" << std::flush;
	debug_mutex.unlock();
#endif
	return msg_sz;
}

template <>
ssize_t UDP::Receive(char *value, const std::size_t tu_size) {
	auto recv_left = tu_size;
	std::size_t total = 0;
	while (total < tu_size) {
		msg_sz = recvfrom(get_descriptor(), value + total, recv_left, 0,
						  ptr_addr, &size_addr);
		recv_left -= msg_sz;
		total += msg_sz;
	}
#ifndef NDEBUG
	debug_mutex.lock();
	std::clog << "[SOCK_CONNECT] UDP<char*>::Receive: received <" << value
			  << " [+" << total << "]>\n" << std::flush;
	debug_mutex.unlock();
#endif
	return total;
}

template <>
ssize_t UDP::Receive(std::string *value, const std::size_t tu_size) {
	buffer = new char[tu_size];
	msg_sz = UDP::Receive(buffer, tu_size);
	buffer[msg_sz] = '\0';
	*value = std::string(buffer, static_cast<std::size_t>(msg_sz));
	value->shrink_to_fit();
	delete buffer;

	return msg_sz;
}

template <>
ssize_t UDP::Send(const uint8_t *value, const std::size_t tu_size) {
	msg_sz = sendto(get_descriptor(), value, tu_size, 0,
					ptr_addr, size_addr);
#ifndef NDEBUG
	if (msg_sz) {
		debug_mutex.lock();
		std::clog << "[SOCK_CONNECT] UDP<uint8_t>::Send: sent <";
		print_values(value, tu_size);
		std::clog << " [+" << tu_size << "]>\n" << std::flush;
		debug_mutex.unlock();
	}
#endif
	return msg_sz;
}

template <>
ssize_t UDP::Send(const uint16_t *value, const std::size_t tu_size) {
	msg_sz = sendto(get_descriptor(), value, tu_size, 0,
					ptr_addr, size_addr);
#ifndef NDEBUG
	if (msg_sz) {
		debug_mutex.lock();
		std::clog << "[SOCK_CONNECT] UDP<uint16_t>::Send: sent <";
		print_values(value, tu_size);
		std::clog << " [+" << tu_size << "]>\n" << std::flush;
		debug_mutex.unlock();
	}
#endif
	return msg_sz;
}

template <>
ssize_t UDP::Send(const uint32_t *value, const std::size_t tu_size) {
	msg_sz = sendto(get_descriptor(), value, tu_size, 0,
					ptr_addr, size_addr);
#ifndef NDEBUG
	if (msg_sz) {
		debug_mutex.lock();
		std::clog << "[SOCK_CONNECT] UDP<uint32_t>::Send: sent <";
		print_values(value, tu_size);
		std::clog << " [+" << tu_size << "]>\n" << std::flush;
		debug_mutex.unlock();
	}
#endif
	return msg_sz;
}

template <>
ssize_t UDP::Send(const std::size_t *value, const std::size_t tu_size) {
	msg_sz = sendto(get_descriptor(), value, tu_size, 0,
					ptr_addr, size_addr);
#ifndef NDEBUG
	if (msg_sz) {
		debug_mutex.lock();
		std::clog << "[SOCK_CONNECT] UDP<size_t>::Send: sent <";
		print_values(value, tu_size);
		std::clog << " [+" << tu_size << "]>\n" << std::flush;
		debug_mutex.unlock();
	}
#endif
	return msg_sz;
}

template <>
ssize_t UDP::Send(char const *value, std::size_t const tu_size) {
	/**
	 * TCP MTU = 1460
	 * net.core.rmem_max = 212992
	 */
	auto send_left = tu_size;
	std::size_t total = 0;
	while (send_left > 0) {
		msg_sz = sendto(get_descriptor(), value + total, send_left,
						0, ptr_addr, size_addr);
		send_left -= msg_sz;
		total += msg_sz;
	}
#ifndef NDEBUG
	if (total) {
		debug_mutex.lock();
		std::clog << "[SOCK_CONNECT] UDP<char*>::Send: sent <" << value
				  << " [+" << total << "]>\n" << std::flush;
		debug_mutex.unlock();
	}
#endif
	return total;
}

template <>
ssize_t UDP::Send(std::string const *value, std::size_t const tu_size) {
	return UDP::Send(value->c_str(), tu_size);
}
