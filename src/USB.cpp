#include "USB.h"
#include <sys/ioctl.h>
#include <fcntl.h>
#include "type_name.h"

USB::USB(std::string address, speed_t speed)
		: m_address(std::move(address)), m_speed(speed) {}

USB::~USB() {
	Shutdown();
}

bool USB::Connect() {
	fd = open(m_address.data(), O_RDWR | O_NOCTTY | O_NDELAY);

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
	 * CLOCAL - Ignore modem Status lines
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
	cfsetspeed(&tio, m_speed);
	int err = ::tcsetattr(fd, TCSAFLUSH, &tio);
	if (err != 0) {
		this->Shutdown();
		return (this->state = false);
	}

#ifndef NDEBUG
	debug_mutex.lock();
	std::clog << "[SOCK_CONNECT] USB Connected to: " << m_address.data()
			  << ":" << m_speed << '\n' << std::flush;
	debug_mutex.unlock();
#endif
	return (this->state = true);
}

template <typename T>
ssize_t USB::Receive(T *value, std::size_t const tu_size) {
	auto recv_left = tu_size;
	std::size_t total = 0;
	while (total < tu_size) {
		msg_sz = read(fd, value + total, recv_left);
		if (msg_sz < 0) {
			this->state = false;
#ifndef NDEBUG
			debug_mutex.lock();
			std::clog << "[SOCK_CONNECT] USB::Receive<" << type_name<decltype(value)>() << ">(fd: " << fd
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
	std::clog << "[SOCK_CONNECT] USB::Receive<" << type_name<decltype(value)>() << ">: <" << print_values(value, tu_size)
			  << " [+" << total << "]>\n" << std::flush;
	debug_mutex.unlock();
#endif
	return total;
}

template <typename T>
ssize_t USB::Send(T const *value, std::size_t const tu_size) {
	auto send_left = tu_size;
	std::size_t total = 0;
	while (send_left > 0) {
		if ((msg_sz = write(fd, value + total, send_left)) < 0) {
			this->state = false;
#ifndef NDEBUG
			debug_mutex.lock();
			std::clog << "[SOCK_CONNECT] USB::Send<" << type_name<decltype(value)>() << ">(fd: " << fd
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
	std::clog << "[SOCK_CONNECT] USB::Send<" << type_name<decltype(value)>() << ">: <" << print_values(value, tu_size)
			  << " [+" << total << "]>\n" << std::flush;
	debug_mutex.unlock();
#endif
	return total;
}

template ssize_t USB::Receive(char *, std::size_t);
template ssize_t USB::Receive(unsigned char *, std::size_t);
template ssize_t USB::Receive(short int *, std::size_t);
template ssize_t USB::Receive(unsigned short int *, std::size_t);
template ssize_t USB::Receive(int *, std::size_t);
template ssize_t USB::Receive(unsigned int *, std::size_t);
template ssize_t USB::Receive(long int *, std::size_t);
template ssize_t USB::Receive(unsigned long int *, std::size_t);
template ssize_t USB::Receive(long long int *, std::size_t);
template ssize_t USB::Receive(unsigned long long int *, std::size_t);
template ssize_t USB::Receive(float *, std::size_t);
template ssize_t USB::Receive(double *, std::size_t);
template ssize_t USB::Receive(long double *, std::size_t);
template ssize_t USB::Receive(bool *, std::size_t);
template ssize_t USB::Send(char const *, std::size_t);
template ssize_t USB::Send(unsigned char const *, std::size_t);
template ssize_t USB::Send(short int const *, std::size_t);
template ssize_t USB::Send(unsigned short int const *, std::size_t);
template ssize_t USB::Send(int const *, std::size_t);
template ssize_t USB::Send(unsigned int const *, std::size_t);
template ssize_t USB::Send(long int const *, std::size_t);
template ssize_t USB::Send(unsigned long int const *, std::size_t);
template ssize_t USB::Send(long long int const *, std::size_t);
template ssize_t USB::Send(unsigned long long int const *, std::size_t);
template ssize_t USB::Send(float const *, std::size_t);
template ssize_t USB::Send(double const *, std::size_t);
template ssize_t USB::Send(long double const *, std::size_t);
template ssize_t USB::Send(bool const *, std::size_t);

template <>
ssize_t USB::Receive(std::string *value, std::size_t const tu_size) {
	if (value->size() < tu_size && tu_size < value->max_size())
		value->resize(tu_size, '\0');
	msg_sz = USB::Receive(&value->front(), tu_size);
	value->shrink_to_fit();
	return msg_sz;
}

template <>
ssize_t USB::Send(std::string const *value, std::size_t const tu_size) {
	return USB::Send(value->c_str(), tu_size);
}

void USB::Shutdown() {
	if (fd >= 0)
		close(fd);
	fd = -1;
}

void USB::setRTS() const {
	int status = 0;
	ioctl(fd, TIOCMGET, &status);
	status |= TIOCM_RTS;
	ioctl(fd, TIOCMSET, &status);
}

void USB::clrRTS() const {
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
