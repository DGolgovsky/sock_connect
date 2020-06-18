#include "UNIX.h"

#ifndef NDEBUG
#include <iostream>
#endif

UNIX::UNIX(std::string &path)
		: Connection(_UNIX, path) {
#ifndef NDEBUG
	debug_mutex.lock();
	std::clog << "[SOCK_CONNECT] UNIX::UNIX("
			  << "std::string path: " << path << ")" << '\n' << std::flush;
	debug_mutex.unlock();
#endif
}

UNIX::~UNIX() {
#ifndef NDEBUG
	debug_mutex.lock();
	std::clog << "[SOCK_CONNECT] UNIX::~UNIX()" << '\n' << std::flush;
	debug_mutex.unlock();
#endif
	if (get_descriptor() >= 0)
		close(get_descriptor());
}

template <>
ssize_t UNIX::Receive(uint8_t *value, const std::size_t tu_size) {
	msg_sz = read(get_descriptor(), value, tu_size);
	if (msg_sz < 1) {
		this->state = false;
#ifndef NDEBUG
		debug_mutex.lock();
		std::clog << "[SOCK_CONNECT] UNIX<uint8_t>::Receive: status <DISCONNECTED>\n" << std::flush;
		debug_mutex.unlock();
#endif
		return 0;
	}
#ifndef NDEBUG
	debug_mutex.lock();
	std::clog << "[SOCK_CONNECT] UNIX<uint8_t>::Receive: received <";
	print_values(value, tu_size);
	std::clog << " [+" << tu_size << "]>\n" << std::flush;
	debug_mutex.unlock();
#endif
	return msg_sz;
}

template <>
ssize_t UNIX::Receive(uint16_t *value, const std::size_t tu_size) {
	msg_sz = read(get_descriptor(), value, tu_size);
	if (msg_sz < 1) {
		this->state = false;
#ifndef NDEBUG
		debug_mutex.lock();
		std::clog << "[SOCK_CONNECT] UNIX<uint16_t>::Receive: status <DISCONNECTED>\n" << std::flush;
		debug_mutex.unlock();
#endif
		return 0;
	}
#ifndef NDEBUG
	debug_mutex.lock();
	std::clog << "[SOCK_CONNECT] UNIX<uint16_t>::Receive: received <";
	print_values(value, tu_size);
	std::clog << " [+" << tu_size << "]>\n" << std::flush;
	debug_mutex.unlock();
#endif
	return msg_sz;
}

template <>
ssize_t UNIX::Receive(uint32_t *value, const std::size_t tu_size) {
	msg_sz = read(get_descriptor(), value, tu_size);
	if (msg_sz < 1) {
		this->state = false;
#ifndef NDEBUG
		debug_mutex.lock();
		std::clog << "[SOCK_CONNECT] UNIX<uint32_t>::Receive: status <DISCONNECTED>\n" << std::flush;
		debug_mutex.unlock();
#endif
		return 0;
	}
#ifndef NDEBUG
	debug_mutex.lock();
	std::clog << "[SOCK_CONNECT] UNIX<uint32_t>::Receive: received <";
	print_values(value, tu_size);
	std::clog << " [+" << tu_size << "]>\n" << std::flush;
	debug_mutex.unlock();
#endif
	return msg_sz;
}

template <>
ssize_t UNIX::Receive(std::string *value, const std::size_t tu_size) {
	msg_sz = read(get_descriptor(), buffer, tu_size);
	if (msg_sz < 1) {
		this->state = false;
#ifndef NDEBUG
		debug_mutex.lock();
		std::clog << "[SOCK_CONNECT] UNIX<char*>::Receive: status <DISCONNECTED>\n" << std::flush;
		debug_mutex.unlock();
#endif
		return 0;
	}
	buffer[msg_sz] = '\0';
	*value = std::string(buffer, static_cast<std::size_t>(msg_sz));
	//value.erase(1, std::min(value.find_first_not_of('0'), value.size()));
#ifndef NDEBUG
	debug_mutex.lock();
	std::clog << "[SOCK_CONNECT] UNIX<char*>::Receive: received <" << value->data() << " [+"
			  << value->size() << "]>\n" << std::flush;
	debug_mutex.unlock();
#endif
	return msg_sz;
}

template <>
ssize_t UNIX::Receive(char *value, const std::size_t tu_size) {
	std::string msg{value};
	msg_sz = UNIX::Receive(&msg, tu_size);
	strcpy(value, msg.c_str());
	return msg_sz;
}

template <>
ssize_t UNIX::Send(const uint8_t *value, const std::size_t tu_size) {
	if ((msg_sz = write(get_descriptor(), value, tu_size)) <
		1) {
		this->state = false;
#ifndef NDEBUG
		debug_mutex.lock();
		std::clog << "[SOCK_CONNECT] UNIX<uint8_t>::Send: status <DISCONNECTED>\n" << std::flush;
		debug_mutex.unlock();
#endif
		return 0;
	}
#ifndef NDEBUG
	debug_mutex.lock();
	std::clog << "[SOCK_CONNECT] UNIX<uint8_t>::Send: sent <";
	print_values(value, tu_size);
	std::clog << " [+" << tu_size << "]>\n" << std::flush;
	debug_mutex.unlock();
#endif
	return msg_sz;
}

template <>
ssize_t UNIX::Send(const uint16_t *value, const std::size_t tu_size) {
	if ((msg_sz = write(get_descriptor(), value, tu_size)) < 1) {
		this->state = false;
#ifndef NDEBUG
		debug_mutex.lock();
		std::clog << "[SOCK_CONNECT] UNIX<uint16_t>::Send: status <DISCONNECTED>\n" << std::flush;
		debug_mutex.unlock();
#endif
		return 0;
	}
#ifndef NDEBUG
	debug_mutex.lock();
	std::clog << "[SOCK_CONNECT] UNIX<uint16_t>::Send: sent <";
	print_values(value, tu_size);
	std::clog << " [+" << tu_size << "]>\n" << std::flush;
	debug_mutex.unlock();
#endif
	return msg_sz;
}

template <>
ssize_t UNIX::Send(const uint32_t *value, const std::size_t tu_size) {
	if ((msg_sz = write(get_descriptor(), value, tu_size)) < 1) {
		this->state = false;
#ifndef NDEBUG
		debug_mutex.lock();
		std::clog << "[SOCK_CONNECT] UNIX<uint32_t>::Send: status <DISCONNECTED>\n" << std::flush;
		debug_mutex.unlock();
#endif
		return 0;
	}
#ifndef NDEBUG
	debug_mutex.lock();
	std::clog << "[SOCK_CONNECT] UNIX<uint32_t>::Send: sent <";
	print_values(value, tu_size);
	std::clog << " [+" << tu_size << "]>\n" << std::flush;
	debug_mutex.unlock();
#endif
	return msg_sz;
}

template <>
ssize_t UNIX::Send(const std::string *value, const std::size_t tu_size) {
	if ((msg_sz = write(get_descriptor(), value->data(), tu_size)) < 1) {
		this->state = false;
#ifndef NDEBUG
		debug_mutex.lock();
		std::clog << "[SOCK_CONNECT] UNIX<char*>::Send: status <DISCONNECTED>\n" << std::flush;
		debug_mutex.unlock();
#endif
		return 0;
	}
#ifndef NDEBUG
	debug_mutex.lock();
	std::clog << "[SOCK_CONNECT] UNIX<char*>::Send: sent <" << value->data() << " [+"
			  << value->size() << "]>\n" << std::flush;
	debug_mutex.unlock();
#endif
	return msg_sz;
}

template <>
ssize_t UNIX::Send(const char *value, const std::size_t tu_size) {
	std::string msg{value};
	return UNIX::Send(&msg, tu_size);
}
