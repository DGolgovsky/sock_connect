#include "sock_connect.h"
#include <fstream>
#include <iostream>

void server() {
	std::ifstream fstream("libsock_connect.so", std::ios::binary);
	std::string file;
	fstream.seekg(0, std::ios::end);
	file.reserve(fstream.tellg());
	fstream.seekg(0, std::ios::beg);
	file.assign((std::istreambuf_iterator<char>(fstream)),
				std::istreambuf_iterator<char>());

	auto socket = new Connector<UDP>(INADDR_LOOPBACK, 8010);

	unsigned long sz = file.length();
	socket->Send(&sz, sizeof(sz));
	unsigned long msg_sz = socket->Send(&file, sz);
	if (msg_sz < sz)
		std::cout << "File doesn't sent: msg_size = " << msg_sz << std::endl;
	std::cout << "Server finished work" << std::endl;
}

int main() {
	auto socket = new Connector<UDP>(INADDR_LOOPBACK, 8010);
	if (!socket->Bind(false)) return 0;

	system ("rm -f libsock_connect.so.received");
	std::ofstream ofstream("libsock_connect.so.received", std::ios::binary);
	std::string file;

	std::thread t(server);
	t.detach();

	auto size = file.length();
	socket->Receive(&size, sizeof(size));
	socket->Receive(&file, size);
	socket->Shutdown();

	if (!file.empty())
		ofstream << file;

	system("md5sum libsock_connect.so libsock_connect.so.received");

	return 0;
}
