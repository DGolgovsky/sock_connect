#include "sock_connect.h"
#include <fstream>
#include <iostream>
#include <random>

void server() {
	std::ifstream fstream("libsock_connect.so", std::ios::binary);
	std::string file;
	fstream.seekg(0, std::ios::end);
	file.reserve(fstream.tellg());
	fstream.seekg(0, std::ios::beg);
	file.assign((std::istreambuf_iterator<char>(fstream)),
				std::istreambuf_iterator<char>());

	auto socket = new Connector<UDP>(INADDR_LOOPBACK, 8010);
	if (!socket->Bind(false)) return;

	std::size_t size = file.length();
	std::size_t answer = 0;

	socket->Send(&size, sizeof(size));
	socket->Receive(&answer, sizeof(answer));
	if (answer == size)
		socket->Send(&file, size);

	std::clog << "Server finished work" << std::endl;
}

int main() {
	system("rm -f libsock_connect.so.received");

	auto socket = new Connector<UDP>(INADDR_LOOPBACK, 8010);
	std::ofstream ofstream("libsock_connect.so.received", std::ios::binary);
	std::string file;

	std::thread t(server);
	t.detach();

	std::size_t size = 0;
	socket->Receive(&size, sizeof(size));
	socket->Send(&size, sizeof(size));
	socket->Receive(&file, size);

	ofstream << file;
	socket->Shutdown();

	std::clog << "Client finished work" << std::endl;
	system("md5sum libsock_connect.so libsock_connect.so.received");
	return 0;
}