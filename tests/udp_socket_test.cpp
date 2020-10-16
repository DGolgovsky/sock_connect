#include "sock_connect.h"
#include <fstream>

void server() {
	std::ifstream fstream("test_udp_socket", std::ios::binary);
	std::string file;
	fstream.seekg(0, std::ios::end);
	file.reserve(static_cast<unsigned long>(fstream.tellg()));
	fstream.seekg(0, std::ios::beg);
	file.assign((std::istreambuf_iterator<char>(fstream)),
				std::istreambuf_iterator<char>());

	auto socket = std::make_shared<Connector<UDP>>(INADDR_LOOPBACK, 8010);

	unsigned long sz = file.length();
	socket->Send(&sz, sizeof(sz));
	auto msg_sz = socket->Send(&file, sz);
	if (msg_sz < sz)
		std::clog << "[TEST] File doesn't sent: msg_size = " << msg_sz << std::endl;
	std::clog << "[TEST] Server finished work" << std::endl;
}

int main() {
	auto socket = std::make_shared<Connector<UDP>>(INADDR_LOOPBACK, 8010);
	if (!socket->Bind(false)) return 0;

	system("rm -f test_udp_socket.received");
	std::ofstream ofstream("test_udp_socket.received", std::ios::binary);
	std::string file;

	std::thread t(server);
	t.detach();

	auto size = file.length();
	socket->Receive(&size, sizeof(size));
	socket->Receive(&file, size);
	//socket->Shutdown(); //TODO Fix exception

	if (!file.empty())
		ofstream << file;

	system("md5sum test_udp_socket test_udp_socket.received");

	return 0;
}
