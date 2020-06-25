#include "sock_connect.h"
#include <fstream>
#include <random>

void server() {
	std::size_t const array_size = 8;
	unsigned seed = static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count());
	std::default_random_engine generator(seed);
	std::uniform_int_distribution<int> distribution(0, 64);

	uint8_t ui8 = 8;
	uint16_t ui16 = 16;
	uint32_t ui32 = 32;
	uint64_t ui64 = 64;

	uint8_t ui8a [array_size];
	uint16_t ui16a [array_size];
	uint32_t ui32a [array_size];
	uint64_t ui64a [array_size];

	std::fill(ui8a, ui8a + array_size, distribution(generator));
	std::fill(ui16a, ui16a + array_size, distribution(generator));
	std::fill(ui32a, ui32a + array_size, distribution(generator));
	std::fill(ui64a, ui64a + array_size, distribution(generator));

	std::ifstream fstream("libsock_connect.so", std::ios::binary);
	std::string file;
	fstream.seekg(0, std::ios::end);
	file.reserve(static_cast<unsigned long>(fstream.tellg()));
	fstream.seekg(0, std::ios::beg);
	file.assign((std::istreambuf_iterator<char>(fstream)),
				std::istreambuf_iterator<char>());

	auto socket = new Connector<TCP>(INADDR_LOOPBACK, 8010);
	auto bind = socket->Bind(true);
	if (!bind)
		return;
	if (socket->Accept()) {
		std::size_t size = file.length();
		socket->Send(&ui8, sizeof(ui8));
		socket->Send(&ui16, sizeof(ui16));
		socket->Send(&ui32, sizeof(ui32));
		socket->Send(&ui64, sizeof(ui64));

		socket->Send(ui8a, sizeof(ui8) * array_size);
		socket->Send(ui16a, sizeof(ui16) * array_size);
		socket->Send(ui32a, sizeof(ui32) * array_size);
		socket->Send(ui64a, sizeof(ui64) * array_size);

		//std::clog << "SENT: ui8 = " << (int)ui8 << "; ui16 = " << ui16 << "; ui32 = " << ui32 << std::endl;
		socket->Send(&size, sizeof(size));
		socket->Send(&file, size);
	}
	std::clog << "Server finished work" << std::endl;
}

int main() {
	system ("rm -f libsock_connect.so.received");
	std::thread t(server);
	t.detach();
	auto socket = new Connector<TCP>(INADDR_LOOPBACK, 8010);
	std::ofstream ofstream("libsock_connect.so.received", std::ios::binary);
	std::string file;
	int i = 1;
	std::size_t const array_size = 8;
	uint8_t ui8 = 0;
	uint16_t ui16 = 0;
	uint32_t ui32 = 0;
	uint64_t ui64 = 0;

	uint8_t ui8a [array_size];
	uint16_t ui16a [array_size];
	uint32_t ui32a [array_size];
	uint64_t ui64a [array_size];

	while (i) {
		if (socket->Connect()) {
			socket->Receive(&ui8, sizeof(ui8));
			socket->Receive(&ui16, sizeof(ui16));
			socket->Receive(&ui32, sizeof(ui32));
			socket->Receive(&ui64, sizeof(ui64));

			socket->Receive(ui8a, sizeof(ui8) * array_size);
			socket->Receive(ui16a, sizeof(ui16) * array_size);
			socket->Receive(ui32a, sizeof(ui32) * array_size);
			socket->Receive(ui64a, sizeof(ui64) * array_size);

			std::size_t size = 0;
			socket->Receive(&size, sizeof(size));
			socket->Receive(&file, size);

			if (!file.empty())
				ofstream << file;
			socket->Shutdown();
			i = 0;
			std::clog << "Client finished work" << std::endl;
		}
	}
	system("md5sum libsock_connect.so libsock_connect.so.received");

	return 0;
}