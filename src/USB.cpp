#include "USB.h"
#include <utility>

#ifndef NDEBUG
#include <iostream>
#endif

USB::USB(std::string address, speed_t speed)
		: address(std::move(address)), speed(speed) {}

USB::~USB() {
	Shutdown();
}

bool USB::Connect() {
	fd = open(address.data(), O_RDWR | O_NOCTTY | O_NDELAY);

	if (fd == -1) {
		return (this->state = false);
	}

	/**
	 * CONFIGURE THE UART
	 * The flags (defined in /usr/include/termios.h - see http://pubs.opengroup.org/onlinepubs/007908799/xsh/termios.h.html):
	 *
	 * Baud rate:- B1200, B2400, B4800, B9600, B19200, B38400, B57600,
	 * B115200, B230400, B460800, B500000, B576000, B921600, B1000000,
	 * B1152000, B1500000, B2000000, B2500000, B3000000, B3500000, B4000000
	 *
	 * CSIZE:- CS5, CS6, CS7, CS8
	 * CLOCAL - Ignore modem status lines
	 * CREAD - Enable receiver
	 * IGNPAR = Ignore characters with parity errors
	 * ICRNL - Map CR to NL on input (Use for ASCII comms where you want to auto correct end of line characters - don't use for bianry comms!)
	 * PARENB - Parity enable
	 * PARODD - Odd parity (else even)
	 */
	termios tio{};
	::tcgetattr(fd, &tio);
	cfmakeraw(&tio);
	tio.c_iflag &= ~(IXON | IXOFF);
	tio.c_oflag = 0;
	tio.c_lflag = 0;       //ICANON;
	tio.c_cc[VTIME] = 0;
	tio.c_cc[VMIN] = 0;
	cfsetspeed(&tio, speed);
	int err = ::tcsetattr(fd, TCSAFLUSH, &tio);
	if (err != 0) {
		this->Shutdown();
		return (this->state = false);
	}

#ifndef NDEBUG
	debug_mutex.lock();
	std::clog << "[SOCK_CONNECT] USB Connected to: " << address.data()
			  << ":" << speed << '\n' << std::flush;
	debug_mutex.unlock();
#endif
	return (this->state = true);
}

template <>
ssize_t USB::Receive(uint8_t *value, const std::size_t tu_size) {
	msg_sz = read(fd, value, tu_size);

	if (msg_sz < 0) {
		this->state = false;
#ifndef NDEBUG
		debug_mutex.lock();
		std::clog << "[SOCK_CONNECT] USB<uint8_t>::Receive: status <DISCONNECTED>\n" << std::flush;
		debug_mutex.unlock();
#endif
		return 0;
	}
#ifndef NDEBUG
	debug_mutex.lock();
	std::clog << "[SOCK_CONNECT] USB<uint8_t>::Receive: received <";
	print_values(value, tu_size);
	std::clog << " [+" << tu_size << "]>\n" << std::flush;
	debug_mutex.unlock();
#endif
	return msg_sz;
}

template <>
ssize_t USB::Receive(uint16_t *value, const std::size_t tu_size) {
	msg_sz = read(fd, value, tu_size);

	if (msg_sz < 0) {
		this->state = false;
#ifndef NDEBUG
		debug_mutex.lock();
		std::clog << "[SOCK_CONNECT] USB<uint16_t>::Receive: status <DISCONNECTED>\n" << std::flush;
		debug_mutex.unlock();
#endif
		return 0;
	}
#ifndef NDEBUG
	debug_mutex.lock();
	std::clog << "[SOCK_CONNECT] USB<uint16_t>::Receive: received <";
	print_values(value, tu_size);
	std::clog << " [+" << tu_size << "]>\n" << std::flush;
	debug_mutex.unlock();
#endif
	return msg_sz;
}

template <>
ssize_t USB::Receive(uint32_t *value, const std::size_t tu_size) {
	msg_sz = read(fd, value, tu_size);

	if (msg_sz < 0) {
		this->state = false;
#ifndef NDEBUG
		debug_mutex.lock();
		std::clog << "[SOCK_CONNECT] USB<uint32_t>::Receive: status <DISCONNECTED>\n" << std::flush;
		debug_mutex.unlock();
#endif
		return 0;
	}
#ifndef NDEBUG
	debug_mutex.lock();
	std::clog << "[SOCK_CONNECT] USB<uint32_t>::Receive: received <";
	print_values(value, tu_size);
	std::clog << " [+" << tu_size << "]>\n" << std::flush;
	debug_mutex.unlock();
#endif
	return msg_sz;
}

template <>
ssize_t USB::Receive(std::string *value, std::size_t tu_size) {
	msg_sz = read(fd, buffer, tu_size);

	if (msg_sz < 0) {
		this->state = false;
#ifndef NDEBUG
		debug_mutex.lock();
		std::clog << "[SOCK_CONNECT] USB<char*>::Receive: status <DISCONNECTED>\n" << std::flush;
		debug_mutex.unlock();
#endif
		return 0;
	}

	buffer[msg_sz] = '\0';
	*value = std::string(buffer, static_cast<std::size_t>(msg_sz));
#ifndef NDEBUG
	debug_mutex.lock();
	std::clog << "[SOCK_CONNECT] USB<char*>::Receive: received <" << value->data() << " [+"
			  << value->size() << "]>\n" << std::flush;
	debug_mutex.unlock();
#endif
	return msg_sz;
}

template <>
ssize_t USB::Receive(char *value, const std::size_t tu_size) {
	std::string msg{value};
	msg_sz = USB::Receive(&msg, tu_size);
	strcpy(value, msg.c_str());
	return msg_sz;
}

template <>
ssize_t USB::Send(const uint8_t *value, const std::size_t tu_size) {
	msg_sz = write(fd, value, tu_size);

	if (msg_sz == -1) {
		this->state = false;
#ifndef NDEBUG
		debug_mutex.lock();
		std::clog << "[SOCK_CONNECT] USB<uint8_t>::Send: status <DISCONNECTED>\n" << std::flush;
		debug_mutex.unlock();
#endif
		return 0;
	}
#ifndef NDEBUG
	debug_mutex.lock();
	std::clog << "[SOCK_CONNECT] USB<uint8_t>::Send: sent <";
	print_values(value, tu_size);
	std::clog << " [+" << tu_size << "]>\n" << std::flush;
	debug_mutex.unlock();
#endif
	return msg_sz;
}

template <>
ssize_t USB::Send(const uint16_t *value, const std::size_t tu_size) {
	msg_sz = write(fd, value, tu_size);

	if (msg_sz == -1) {
		this->state = false;
#ifndef NDEBUG
		debug_mutex.lock();
		std::clog << "[SOCK_CONNECT] USB<uint16_t>::Send: status <DISCONNECTED>\n" << std::flush;
		debug_mutex.unlock();
#endif
		return 0;
	}
#ifndef NDEBUG
	debug_mutex.lock();
	std::clog << "[SOCK_CONNECT] USB<uint16_t>::Send: sent <";
	print_values(value, tu_size);
	std::clog << " [+" << tu_size << "]>\n" << std::flush;
	debug_mutex.unlock();
#endif
	return msg_sz;
}

template <>
ssize_t USB::Send(const uint32_t *value, const std::size_t tu_size) {
	msg_sz = write(fd, value, tu_size);

	if (msg_sz == -1) {
		this->state = false;
#ifndef NDEBUG
		debug_mutex.lock();
		std::clog << "[SOCK_CONNECT] USB<uint32_t>::Send: status <DISCONNECTED>\n" << std::flush;
		debug_mutex.unlock();
#endif
		return 0;
	}
#ifndef NDEBUG
	debug_mutex.lock();
	std::clog << "[SOCK_CONNECT] USB<uint32_t>::Send: sent <";
	print_values(value, tu_size);
	std::clog << " [+" << tu_size << "]>\n" << std::flush;
	debug_mutex.unlock();
#endif
	return msg_sz;
}

template <>
ssize_t USB::Send(const std::string *value, const std::size_t tu_size) {
	msg_sz = write(fd, value->data(), tu_size);

	if (msg_sz == -1) {
		this->state = false;
#ifndef NDEBUG
		debug_mutex.lock();
		std::clog << "[SOCK_CONNECT] USB<char*>::Send: status <DISCONNECTED>\n" << std::flush;
		debug_mutex.unlock();
#endif
		return 0;
	}
#ifndef NDEBUG
	debug_mutex.lock();
	std::clog << "[SOCK_CONNECT] USB<char*>::Send: sent <" << value->data() << " [+"
			  << value->size() << "]>\n" << std::flush;
	debug_mutex.unlock();
#endif
	return msg_sz;
}

template <>
ssize_t USB::Send(const char *value, const std::size_t tu_size) {
	std::string msg{value};
	return USB::Send(&msg, tu_size);
}

void USB::Shutdown() {
	if (fd >= 0)
		close(fd);

	fd = -1;
}

void USB::setRTS() {
	int status = 0;
	ioctl(fd, TIOCMGET, &status);
	status |= TIOCM_RTS;
	ioctl(fd, TIOCMSET, &status);
}

void USB::clrRTS() {
	int status = 0;
	ioctl(fd, TIOCMGET, &status);
	status &= ~TIOCM_RTS;
	ioctl(fd, TIOCMSET, &status);
}

bool USB::status() const {
	return fd > -1;
}

int USB::id() const {
	return fd;
}
