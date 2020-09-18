#include "sock_connect.h"
#include <iostream>

// Tested with Arduino Mega as echo read/write
inline bool main_loop(const std::string &dev_path, speed_t speed) {
	std::string msg;
	auto socket = std::make_shared<Connector<USB>>(dev_path, speed);

	std::clog << "[TEST] Connect to: " << dev_path << "\n";
	while (true) { // Infinite loop
		if (socket->Connect()) {
			while (socket->Status()) { // Reading messages queue
				std::clog << "[TEST] >>> Send data: ";
				std::cin >> msg;
				if (!msg.empty()) {
					if (!socket->Send(&msg, msg.length())) {
						std::clog << "[TEST] === Couldn't send data: " << msg << std::endl;
						return false;
					}
					//std::this_thread::sleep_for(std::chrono::seconds(1));
				}
				socket->Receive(&msg, msg.length());
				std::clog << "[TEST] <<< Recv data: " << msg << std::endl;
			}
		}
		std::clog << "[TEST] Socket status returned false. Exit\n";
		return false;
	}
}

int main() {
	std::string dev_path{"/dev/ttyACM0"};
	speed_t speed = 0000015; // B9600
	// speed_t speed = 0010002; // B115200
	//std::clog << "[TEST] Enter device path (default /dev/ttyACM0): ";
	//std::getline (std::cin, dev_path);
	if (dev_path.empty())
		dev_path = "/dev/ttyACM0";

	bool status = true;
	while (status) {
		status = main_loop(dev_path, speed);
	}

	return 0;
}