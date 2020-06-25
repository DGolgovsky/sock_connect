#include "UNIX.h"
#include "type_name.h"

UNIX::UNIX(std::string const &path)
		: Connection(_UNIX, path) {
#ifndef NDEBUG
	debug_mutex.lock();
	std::clog << "[SOCK_CONNECT] UNIX::UNIX("
			  << "std::string const &path: " << path << ")" << '\n' << std::flush;
	debug_mutex.unlock();
#endif
}

UNIX::UNIX(char const *path)
		: Connection(_UNIX, path) {
#ifndef NDEBUG
	debug_mutex.lock();
	std::clog << "[SOCK_CONNECT] UNIX::UNIX("
			  << "char const *path: " << path << ")" << '\n' << std::flush;
	debug_mutex.unlock();
#endif
}

UNIX::~UNIX() {
#ifndef NDEBUG
	debug_mutex.lock();
	std::clog << "[SOCK_CONNECT] UNIX::~UNIX()\n" << std::flush;
	debug_mutex.unlock();
#endif
}

template <typename T>
ssize_t UNIX::Receive(T *value, std::size_t const tu_size) {
	auto recv_left = tu_size;
	std::size_t total = 0;
	while (total < tu_size) {
		msg_sz = read(get_descriptor(), value + total, recv_left);
		if (msg_sz < 1) {
			this->state = false;
#ifndef NDEBUG
			debug_mutex.lock();
			std::clog << "[SOCK_CONNECT] UNIX::Receive<" << type_name<decltype(value)>() << ">(fd: " << get_descriptor()
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
	std::clog << "[SOCK_CONNECT] UNIX::Receive<" << type_name<decltype(value)>() << ">: <" << print_values(value, tu_size)
			  << " [+" << total << "]>\n" << std::flush;
	debug_mutex.unlock();
#endif
	return total;
}

template <typename T>
ssize_t UNIX::Send(T const *value, std::size_t const tu_size) {
	auto send_left = tu_size;
	std::size_t total = 0;
	while (send_left > 0) {
		if ((msg_sz = write(get_descriptor(), value + total, send_left)) < 1) {
			this->state = false;
#ifndef NDEBUG
			debug_mutex.lock();
			std::clog << "[SOCK_CONNECT] UNIX::Send<" << type_name<decltype(value)>() << ">(fd: " << get_descriptor()
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
	std::clog << "[SOCK_CONNECT] UNIX::Send<" << type_name<decltype(value)>() << ">: <" << print_values(value, tu_size)
			  << " [+" << total << "]>\n" << std::flush;
	debug_mutex.unlock();
#endif
	return total;
}

template ssize_t UNIX::Receive(char *, std::size_t);
template ssize_t UNIX::Receive(unsigned char *, std::size_t);
template ssize_t UNIX::Receive(short int *, std::size_t);
template ssize_t UNIX::Receive(unsigned short int *, std::size_t);
template ssize_t UNIX::Receive(int *, std::size_t);
template ssize_t UNIX::Receive(unsigned int *, std::size_t);
template ssize_t UNIX::Receive(long int *, std::size_t);
template ssize_t UNIX::Receive(unsigned long int *, std::size_t);
template ssize_t UNIX::Receive(long long int *, std::size_t);
template ssize_t UNIX::Receive(unsigned long long int *, std::size_t);
template ssize_t UNIX::Receive(float *, std::size_t);
template ssize_t UNIX::Receive(double *, std::size_t);
template ssize_t UNIX::Receive(long double *, std::size_t);
template ssize_t UNIX::Receive(bool *, std::size_t);
template ssize_t UNIX::Send(char const *, std::size_t);
template ssize_t UNIX::Send(unsigned char const *, std::size_t);
template ssize_t UNIX::Send(short int const *, std::size_t);
template ssize_t UNIX::Send(unsigned short int const *, std::size_t);
template ssize_t UNIX::Send(int const *, std::size_t);
template ssize_t UNIX::Send(unsigned int const *, std::size_t);
template ssize_t UNIX::Send(long int const *, std::size_t);
template ssize_t UNIX::Send(unsigned long int const *, std::size_t);
template ssize_t UNIX::Send(long long int const *, std::size_t);
template ssize_t UNIX::Send(unsigned long long int const *, std::size_t);
template ssize_t UNIX::Send(float const *, std::size_t);
template ssize_t UNIX::Send(double const *, std::size_t);
template ssize_t UNIX::Send(long double const *, std::size_t);
template ssize_t UNIX::Send(bool const *, std::size_t);

template <>
ssize_t UNIX::Receive(std::string *value, std::size_t const tu_size) {
	if (value->size() < tu_size && tu_size < value->max_size())
		value->resize(tu_size, '\0');
	msg_sz = UNIX::Receive(&value->front(), tu_size);
	value->shrink_to_fit();
	return msg_sz;
}

template <>
ssize_t UNIX::Send(std::string const *value, std::size_t const tu_size) {
	return UNIX::Send(value->c_str(), tu_size);
}