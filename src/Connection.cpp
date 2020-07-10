#include "Connection.h"
#include "type_name.h"

/**
 * Transform <const char*> IP-address to uint32_t
 * "127.0.0.1" -> 0x7f000001
 */
unsigned int ip_to_int(std::string const &str) {
#ifndef NDEBUG
	debug_mutex.lock();
	std::clog << "[SOCK_CONNECT] Connection::ip_to_int("
			  << "const char *ip: " << str << ")\n" << std::flush;
	debug_mutex.unlock();
#endif
	uint32_t ip = 0;
	uint32_t part = 0;
	uint32_t part_count = 0;

	for (const auto &c : str) {
		if (std::isdigit(c)) {
			part = (part * 10) + (c - '0');
		} else if (c == '.') {
			if (++part_count == 4) break;
			ip = (ip << 8) + part;
			part = 0;
		} else { break; }
	}
	ip = (ip << 8) + part;
	if (++part_count < 4)
		ip = ip << ((4 - part_count) << 3);

	return ip;
}

Connection::Connection(conn_type cp, uint32_t addr, uint16_t port)
		: socket_(cp), address_(addr), port_(port) {
	conn_memset();
	socket_addr.sin_family = AF_INET;
	socket_addr.sin_addr.s_addr = htonl(address_);
	socket_addr.sin_port = htons(port_);
	ptr_addr = reinterpret_cast<sockaddr *>(&socket_addr);
	size_addr = sizeof(socket_addr);
	clients = new storage_t(32);
	timeval set = {60, 0};
	int reuse = 1;
	setsockopt(socket_.id(), SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
	setsockopt(socket_.id(), SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(reuse));
	setsockopt(socket_.id(), SOL_SOCKET, SO_SNDTIMEO, reinterpret_cast<char *>(&set), sizeof(set));

#ifndef NDEBUG
	debug_mutex.lock();
	std::clog << "[SOCK_CONNECT] Connection::Connection<" << socket_.c_type() << ">("
			  << type_name<decltype(addr)>() << " addr: " << addr << ", "
			  << type_name<decltype(port)>() << " port: " << port << ") ["
			  << "SOCK_ID = " << socket_.id() << "]\n" << std::flush;
	debug_mutex.unlock();
#endif
	this->state = true;
}

Connection::Connection(conn_type cp, char const *addr, uint16_t port)
		: Connection(cp, ip_to_int(addr), port) {}

Connection::Connection(conn_type cp, std::string const &addr, uint16_t port)
		: Connection(cp, ip_to_int(addr), port) {}

Connection::Connection(conn_type cp, const char &&addr, uint16_t port)
		: Connection(cp, ip_to_int(&addr), port) {}

Connection::Connection(conn_type cp, std::string const &socket_path)
		: socket_(cp), m_path(socket_path) {
	conn_memset();
	unix_addr.sun_family = AF_UNIX;
	strcpy(unix_addr.sun_path, socket_path.c_str());
	ptr_addr = reinterpret_cast<sockaddr *>(&unix_addr);
	size_addr = sizeof(unix_addr);
	clients = new storage_t(32);
#ifndef NDEBUG
	debug_mutex.lock();
	std::clog << "[SOCK_CONNECT] Connection::Connection<" << socket_.c_type() << ">("
			  << "std::string socket_path: " << socket_path << ")" << '\n' << std::flush;
	debug_mutex.unlock();
#endif
	this->state = true;
}

Connection::Connection(conn_type cp, char const *path)
		: Connection(cp, static_cast<std::string>(path)) {}

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
	if (!id) {
		id = this->get_descriptor();
		this->state = false;
	}

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
		this->state = false;
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
		std::clog << "[SOCK_CONNECT] " << socket_.c_type() << "::Bind(" << inet_ntoa(socket_addr.sin_addr)
				  << ":" << ntohs(socket_addr.sin_port) << ")\n" << std::flush;
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
		std::clog << "[SOCK_CONNECT] " << socket_.c_type() << "::Listen(" << inet_ntoa(socket_addr.sin_addr)
				  << ":" << ntohs(socket_addr.sin_port) << ")\n" << std::flush;
		debug_mutex.unlock();
	}
#endif

	return true;
}

int Connection::Accept(std::string *client_address) {
	socklen_t sz = sizeof(client_addr);
	int transmission = accept(socket_.id(), reinterpret_cast<sockaddr *>(&client_addr), &sz);
	if (transmission < 0) {
		throw std::runtime_error(
				"[SOCK_CONNECT] " + socket_.c_type() + " Accept failed, error number: " + std::to_string(errno));
	}
#ifndef NDEBUG
	debug_mutex.lock();
	std::clog << "[SOCK_CONNECT] " << socket_.c_type() << "::Accept("
			  << (socket_.c_type() != "UNIX" ? inet_ntoa(client_addr.sin_addr) : this->m_path)
			  << ":" << ntohs(client_addr.sin_port) << ") | fd: " << transmission << '\n' << std::flush;
	debug_mutex.unlock();
#endif
	if (client_address) {
        client_address->append((socket_.c_type() != "UNIX" ? inet_ntoa(client_addr.sin_addr) : this->m_path));
	}
	// Must be called also at dev-side
    this->assign_thread(transmission);
	return transmission;
}

bool Connection::Connect() {
	if (connect(socket_.id(), ptr_addr, sizeof(*ptr_addr)) < 0) {
		return false;
	}
	timeval set = {60, 0};
	setsockopt(socket_.id(), SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<char *>(&set), sizeof(set));
#ifndef NDEBUG
	debug_mutex.lock();
	std::clog << "[SOCK_CONNECT] " << socket_.c_type() << "::Connect("
			  << (socket_.c_type() != "UNIX" ? inet_ntoa(socket_addr.sin_addr) : this->m_path)
			  << ":" << ntohs(socket_addr.sin_port) << ")\n" << std::flush;
	debug_mutex.unlock();
#endif
	return this->state = true;
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
