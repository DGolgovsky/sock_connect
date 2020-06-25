#include "TCP.h"
#include "type_name.h"

TCP::TCP(uint32_t address, uint16_t port)
		: Connection(_TCP, address, port) {
#ifndef NDEBUG
	debug_mutex.lock();
	std::clog << "[SOCK_CONNECT] TCP::TCP("
			<< type_name<decltype(address)>() << " address: " << address << ", "
			<< type_name<decltype(port)>() << " port: " << port << ")" << '\n' << std::flush;
	debug_mutex.unlock();
#endif
}

TCP::TCP(const char *address, uint16_t port)
		: Connection(_TCP, address, port) {
#ifndef NDEBUG
	debug_mutex.lock();
	std::clog << "[SOCK_CONNECT] TCP::TCP("
			<< type_name<decltype(address)>() << " address: " << address << ", "
			<< type_name<decltype(port)>() << " port: " << port << ")" << '\n' << std::flush;
	debug_mutex.unlock();
#endif
}

TCP::TCP(const std::string &address, uint16_t port)
		: Connection(_TCP, address.data(), port) {
#ifndef NDEBUG
	debug_mutex.lock();
	std::clog << "[SOCK_CONNECT] TCP::TCP("
			<< type_name<decltype(address)>() << " address: " << address << ", "
			<< type_name<decltype(port)>() << " port: " << port << ")" << '\n' << std::flush;
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

template <typename T>
ssize_t TCP::Receive(T *value, std::size_t const tu_size) {
	auto recv_left = tu_size;
	std::size_t total = 0;
	while (total < tu_size) {
		msg_sz = recv(get_descriptor(), value + total, recv_left, MSG_NOSIGNAL);
		if (msg_sz < 1) {
			this->state = false;
#ifndef NDEBUG
			debug_mutex.lock();
			std::clog << "[SOCK_CONNECT] TCP::Receive<" << type_name<decltype(value)>() << ">(fd: " << get_descriptor()
					  << "): FAILED. Status - DISCONNECTED\n" << std::flush;
			debug_mutex.unlock();
#endif
			return 0;
		}
		recv_left -= msg_sz;
		total += msg_sz;
	}
#ifndef NDEBUG
	debug_mutex.lock();
	std::clog << "[SOCK_CONNECT] TCP::Receive<" << type_name<decltype(value)>() << ">: <" << print_values(value, tu_size)
			  << " [+" << total << "]>\n" << std::flush;
	debug_mutex.unlock();
#endif
	return total;
}

template <typename T>
ssize_t TCP::Send(T const *value, std::size_t const tu_size) {
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
		std::clog << "[SOCK_CONNECT] TCP::Send<" << type_name<decltype(value)>() << ">(fd: " << get_descriptor()
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
	std::clog << "[SOCK_CONNECT] TCP::Send<" << type_name<decltype(value)>() << ">: <" << print_values(value, tu_size)
			  << " [+" << total << "]>\n" << std::flush;
	debug_mutex.unlock();
#endif
	return total;
}

template ssize_t TCP::Receive(char *, std::size_t);
template ssize_t TCP::Receive(unsigned char *, std::size_t);
template ssize_t TCP::Receive(short int *, std::size_t);
template ssize_t TCP::Receive(unsigned short int *, std::size_t);
template ssize_t TCP::Receive(int *, std::size_t);
template ssize_t TCP::Receive(unsigned int *, std::size_t);
template ssize_t TCP::Receive(long int *, std::size_t);
template ssize_t TCP::Receive(unsigned long int *, std::size_t);
template ssize_t TCP::Receive(long long int *, std::size_t);
template ssize_t TCP::Receive(unsigned long long int *, std::size_t);
template ssize_t TCP::Receive(float *, std::size_t);
template ssize_t TCP::Receive(double *, std::size_t);
template ssize_t TCP::Receive(long double *, std::size_t);
template ssize_t TCP::Receive(bool *, std::size_t);
template ssize_t TCP::Send(char const *, std::size_t);
template ssize_t TCP::Send(unsigned char const *, std::size_t);
template ssize_t TCP::Send(short int const *, std::size_t);
template ssize_t TCP::Send(unsigned short int const *, std::size_t);
template ssize_t TCP::Send(int const *, std::size_t);
template ssize_t TCP::Send(unsigned int const *, std::size_t);
template ssize_t TCP::Send(long int const *, std::size_t);
template ssize_t TCP::Send(unsigned long int const *, std::size_t);
template ssize_t TCP::Send(long long int const *, std::size_t);
template ssize_t TCP::Send(unsigned long long int const *, std::size_t);
template ssize_t TCP::Send(float const *, std::size_t);
template ssize_t TCP::Send(double const *, std::size_t);
template ssize_t TCP::Send(long double const *, std::size_t);
template ssize_t TCP::Send(bool const *, std::size_t);

template <>
ssize_t TCP::Receive(std::string *value, std::size_t const tu_size) {
	if (value->size() < tu_size && tu_size < value->max_size())
		value->resize(tu_size, '\0');
	msg_sz = TCP::Receive(&value->front(), tu_size);
	value->shrink_to_fit();
	return msg_sz;
}

template <>
ssize_t TCP::Send(std::string const *value, std::size_t const tu_size) {
	return TCP::Send(value->c_str(), tu_size);
}