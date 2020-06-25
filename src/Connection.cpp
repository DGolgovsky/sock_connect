#include "Connection.h"
#include "type_name.h"

/**
 * Transform <const char*> IP-address to uint32_t
 * "127.0.0.1" -> 0x7f000001
 */
unsigned int ip_to_int(const char *ip) {
#ifndef NDEBUG
	debug_mutex.lock();
	std::clog << "[SOCK_CONNECT] Connection::ip_to_int("
			  << "const char *ip: " << ip << ")\n" << std::flush;
	debug_mutex.unlock();
#endif
	/* The return value. */
	unsigned v = 0;
	/* The count of the number of bytes processed. */
	int i = 0;
	/* A pointer to the next digit to process. */
	const char *start = ip;

	for (i = 0; i < 4; ++i) {
		/* The digit being processed. */
		char c;
		/* The value of this byte. */
		int n = 0;
		while (true) {
			c = *start;
			start++;
			if (c >= '0' && c <= '9') {
				n *= 10;
				n += c - '0';
			} else if ((i < 3 && c == '.') || i == 3) {
				/* We insist on stopping at "." if we are still parsing
				 * the first, second, or third numbers. If we have reached
				 * the end of the numbers, we will allow any character.
				 */
				break;
			} else {
				return 0;
			}
		}

		if (n >= 256)
			return 0;

		v *= 256;
		v += n;
	}
	return v;
}

Connection::Connection(conn_type cp, uint32_t addr, uint16_t port)
		: socket_(cp), address_(addr), port_(port) {
	conn_memset();
	socket_addr.sin_family = AF_INET;
	socket_addr.sin_addr.s_addr = htonl(address_);
	socket_addr.sin_port = htons(port_);
	ptr_addr = (sockaddr *) &socket_addr;
	size_addr = sizeof(socket_addr);
	clients = new storage_t(32);
	timeval set = {0, 0};
	set.tv_sec = 10;
	set.tv_usec = 0;
	setsockopt(socket_.id(), SOL_SOCKET, SO_RCVTIMEO, (char *)&set, sizeof(set)); //SO_REUSEADDR
#ifndef NDEBUG
	debug_mutex.lock();
	std::clog << "[SOCK_CONNECT] Connection::Connection<" << socket_.c_type() << ">("
			  << type_name<decltype(addr)>() << " addr: " << addr << ", "
			  << type_name<decltype(port)>() << " port: " << port << ") ["
			  << "SOCK_ID = " << socket_.id() << "]\n" << std::flush;
	debug_mutex.unlock();
#endif
}

Connection::Connection(conn_type cp, char const *addr, uint16_t port)
		: Connection(cp, ip_to_int(addr), port) {}

Connection::Connection(conn_type cp, std::string const &addr, uint16_t port)
		: Connection(cp, ip_to_int(addr.data()), port) {}

Connection::Connection(conn_type cp, const char &&addr, uint16_t port)
		: Connection(cp, ip_to_int(&addr), port) {}

Connection::Connection(conn_type cp, std::string const &socket_path)
		: socket_(cp), m_path(socket_path) {
	conn_memset();
	unix_addr.sun_family = AF_UNIX;
	strcpy(unix_addr.sun_path, socket_path.c_str());
	ptr_addr = (sockaddr *) &unix_addr;
	size_addr = sizeof(unix_addr);
	clients = new storage_t(32);
#ifndef NDEBUG
	debug_mutex.lock();
	std::clog << "[SOCK_CONNECT] Connection::Connection<" << socket_.c_type() << ">("
			  << "std::string socket_path: " << socket_path << ")" << '\n' << std::flush;
	debug_mutex.unlock();
#endif
}

Connection::Connection(conn_type cp, char const *path)
		: Connection (cp, (std::string) path) {}

void Connection::conn_memset() {
	memset(&socket_addr, '\0', sizeof(socket_addr));
	memset(&client_addr, '\0', sizeof(client_addr));
	memset(&unix_addr, '\0', sizeof(unix_addr));
}

Connection::~Connection() {
#ifndef NDEBUG
	debug_mutex.lock();
	std::clog << "[SOCK_CONNECT] Connection::~Connection<" << socket_.c_type() << ">()\n" << std::flush;
	debug_mutex.unlock();
#endif
	this->Shutdown(socket_.id());
	delete clients;
}

void Connection::Shutdown(int id) {
	if (shutdown(id, SHUT_RDWR) < 0) {
		if (close(id) < 0)
			throw std::runtime_error("[SOCK_CONNECT] Shutdown failed, error number: "
									 + std::to_string(errno));
#ifndef NDEBUG
		debug_mutex.lock();
		std::clog << "[SOCK_CONNECT] Connection::Shutdown<" << socket_.c_type() << ">(): " << id << '\n'
				  << std::flush;
		debug_mutex.unlock();
#endif
	}
	if (socket_.c_type() == "UNIX") {
		unlink(m_path.c_str());
#ifndef NDEBUG
		debug_mutex.lock();
		std::clog << "[SOCK_CONNECT] Connection::Shutdown<" << socket_.c_type() << ">(): " << m_path << '\n'
				  << std::flush;
		debug_mutex.unlock();
#endif
	}
}

bool Connection::Bind(bool listen) const {
	if (bind(socket_.id(), ptr_addr, size_addr) < 0) {
		close(socket_.id());
		throw std::runtime_error(
				"[SOCK_CONNECT] " + socket_.c_type() + " Bind failed, error number: "
				+ std::to_string(errno));
	}
#ifndef NDEBUG
	if (socket_.c_type() != "UNIX") {
		debug_mutex.lock();
		std::clog << "[SOCK_CONNECT] " << socket_.c_type() << " Address Binded: " << inet_ntoa(socket_addr.sin_addr)
				  << ":" << ntohs(socket_addr.sin_port) << '\n' << std::flush;
		debug_mutex.unlock();
	}
#endif
	if (listen)
		return this->Listen();

	return true;
}

bool Connection::Listen() const {
	if (socket_.c_type() == "UDP") return true;
	if (listen(socket_.id(), SOMAXCONN) < 0) {
		throw std::runtime_error(
				"[SOCK_CONNECT] " + socket_.c_type() + " Listen failed, error number: "
				+ std::to_string(errno));
	}
#ifndef NDEBUG
	if (socket_.c_type() != "UNIX") {
		debug_mutex.lock();
		std::clog << "[SOCK_CONNECT] " << socket_.c_type() << " Address Listened: " << inet_ntoa(socket_addr.sin_addr)
				  << ":" << ntohs(socket_addr.sin_port) << '\n' << std::flush;
		debug_mutex.unlock();
	}
#endif

	return true;
}

int Connection::Accept() {
	socklen_t sz = sizeof(client_addr);
	int transmission = accept(socket_.id(), (sockaddr *) &client_addr, &sz);
	if (transmission < 0) {
		throw std::runtime_error(
				"[SOCK_CONNECT] " + socket_.c_type() + " Accept failed, error number: " + std::to_string(errno));
	}
#ifndef NDEBUG
	debug_mutex.lock();
	std::clog << "[SOCK_CONNECT] " << socket_.c_type() << " Client Connected: " << inet_ntoa(client_addr.sin_addr)
			  << ":" << ntohs(client_addr.sin_port) << " | fd: " << transmission << '\n' << std::flush;
	debug_mutex.unlock();
#endif
	// Must be called at dev-side
	this->assign_thread(transmission);
	return transmission;
}

bool Connection::Connect() {
	if (connect(socket_.id(), ptr_addr, sizeof(*ptr_addr)) < 0) {
		this->state = false;
		return false;
	}
#ifndef NDEBUG
	debug_mutex.lock();
	std::clog << "[SOCK_CONNECT] " << socket_.c_type() << " Connected to: " << inet_ntoa(socket_addr.sin_addr)
			  << ":" << ntohs(socket_addr.sin_port) << '\n' << std::flush;
	debug_mutex.unlock();
#endif
	this->state = true;
	//this->assign_thread(socket_.id());
	return true;
}

int Connection::id() noexcept {
	return socket_.id();
}

bool Connection::status() const {
	return state;
}

void Connection::assign_thread(int id) {
	auto thread_id = std::this_thread::get_id();
	auto it = std::find_if(clients->begin(), clients->end(),
						   [&thread_id](const std::pair<std::thread::id, int> &element) {
							   return element.first == thread_id;
						   });
	if (it != clients->end())
		it->second = id;
	else
		clients->emplace_back(std::this_thread::get_id(), id);
	//clients[std::this_thread::get_id()] = id;
}

int Connection::get_descriptor() {
	auto thread_id = std::this_thread::get_id();
	auto it = std::find_if(clients->begin(), clients->end(),
						   [&thread_id](const std::pair<std::thread::id, int> &element) {
							   return element.first == thread_id;
						   });
	if (it != clients->end())
		return it->second;
	else
		return this->id();
}


