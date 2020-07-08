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

	std::ifstream fstream("unix_socket_test", std::ios::binary);
	std::string file;
	fstream.seekg(0, std::ios::end);
	file.reserve(static_cast<unsigned long>(fstream.tellg()));
	fstream.seekg(0, std::ios::beg);
	file.assign((std::istreambuf_iterator<char>(fstream)),
				std::istreambuf_iterator<char>());

	std::string sun_path = "/tmp/sun_path";
	auto socket = new Connector<UNIX>(sun_path);
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
	system ("rm -f unix_socket_test.received");
	std::thread t(server);
	t.detach();
	//std::string sun_path = "/tmp/sun_path";
	auto socket = new Connector<UNIX>("/tmp/sun_path");
	std::ofstream ofstream("unix_socket_test.received", std::ios::binary);
	std::string file;
	std::size_t const array_size = 8;
	uint8_t ui8 = 0;
	uint16_t ui16 = 0;
	uint32_t ui32 = 0;
	uint64_t ui64 = 0;

	uint8_t ui8a [array_size];
	uint16_t ui16a [array_size];
	uint32_t ui32a [array_size];
	uint64_t ui64a [array_size];

	while (socket->Status()) {
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
			std::clog << "Client finished work" << std::endl;
		}
	}
	system("md5sum unix_socket_test unix_socket_test.received");

	return 0;
}