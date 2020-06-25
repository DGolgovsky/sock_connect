#include "UDP.h"
#include "type_name.h"

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

template <typename T>
ssize_t UDP::Receive(T *value, std::size_t const tu_size) {
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
	std::clog << "[SOCK_CONNECT] UDP::Receive<" << type_name<decltype(value)>() << ">: <" << print_values(value, tu_size)
			  << " [+" << total << "]>\n" << std::flush;
	debug_mutex.unlock();
#endif
	return total;
}

template <typename T>
ssize_t UDP::Send(T const *value, std::size_t const tu_size) {
	/**
	 * UDP MTU = 576
	 * net.core.rmem_max = 212992
	 */
	auto send_left = tu_size;
	std::size_t total = 0;
	unsigned int frames = tu_size / 576;
	unsigned int last_packet = tu_size - frames * 576;
	while (send_left > 0) {
		if ((msg_sz = sendto(get_descriptor(), value + total, frames ? 576 : last_packet, 0, ptr_addr, size_addr)) < 0) {
			return total;
		}
		frames--;
		send_left -= msg_sz;
		total += msg_sz;
	}
#ifndef NDEBUG
	debug_mutex.lock();
	std::clog << "[SOCK_CONNECT] UDP::Send<" << type_name<decltype(value)>() << ">: <" << print_values(value, tu_size)
			  << " [+" << total << "]>\n" << std::flush;
	debug_mutex.unlock();
#endif
	return total;
}

template ssize_t UDP::Receive(char *, std::size_t);
template ssize_t UDP::Receive(unsigned char *, std::size_t);
template ssize_t UDP::Receive(short int *, std::size_t);
template ssize_t UDP::Receive(unsigned short int *, std::size_t);
template ssize_t UDP::Receive(int *, std::size_t);
template ssize_t UDP::Receive(unsigned int *, std::size_t);
template ssize_t UDP::Receive(long int *, std::size_t);
template ssize_t UDP::Receive(unsigned long int *, std::size_t);
template ssize_t UDP::Receive(long long int *, std::size_t);
template ssize_t UDP::Receive(unsigned long long int *, std::size_t);
template ssize_t UDP::Receive(float *, std::size_t);
template ssize_t UDP::Receive(double *, std::size_t);
template ssize_t UDP::Receive(long double *, std::size_t);
template ssize_t UDP::Receive(bool *, std::size_t);
template ssize_t UDP::Send(char const *, std::size_t);
template ssize_t UDP::Send(unsigned char const *, std::size_t);
template ssize_t UDP::Send(short int const *, std::size_t);
template ssize_t UDP::Send(unsigned short int const *, std::size_t);
template ssize_t UDP::Send(int const *, std::size_t);
template ssize_t UDP::Send(unsigned int const *, std::size_t);
template ssize_t UDP::Send(long int const *, std::size_t);
template ssize_t UDP::Send(unsigned long int const *, std::size_t);
template ssize_t UDP::Send(long long int const *, std::size_t);
template ssize_t UDP::Send(unsigned long long int const *, std::size_t);
template ssize_t UDP::Send(float const *, std::size_t);
template ssize_t UDP::Send(double const *, std::size_t);
template ssize_t UDP::Send(long double const *, std::size_t);
template ssize_t UDP::Send(bool const *, std::size_t);

template <>
ssize_t UDP::Receive(std::string *value, std::size_t const tu_size) {
	if (value->size() < tu_size)
		value->resize(tu_size, '\0');
	msg_sz = UDP::Receive(&value->front(), tu_size);
	value->shrink_to_fit();
	return msg_sz;
}

template <>
ssize_t UDP::Send(std::string const *value, std::size_t const tu_size) {
	return UDP::Send(value->c_str(), tu_size);
}
