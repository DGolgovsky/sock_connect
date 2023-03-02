#include <fstream>
#include <random>
#include <chrono>
#include <memory>
#include <netinet/in.h>
#include <iostream>
#include <thread>

#include "interface/sock_connect.h"

static void server()
{
    std::ifstream fstream("udp_socket_test", std::ios::binary);
    std::string file;
    fstream.seekg(0, std::ios::end);
    file.reserve(static_cast<unsigned long>(fstream.tellg()));
    fstream.seekg(0, std::ios::beg);
    file.assign((std::istreambuf_iterator<char>(fstream)),
                std::istreambuf_iterator<char>());

    auto socket = std::make_shared<connector<udp>>(INADDR_LOOPBACK, 8010);

    unsigned long sz = file.length();
    socket->send(&sz, sizeof(sz));
    auto msg_sz = socket->send(&file, sz);
    if (msg_sz < sz)
    {
        std::clog << "[TEST] File doesn't sent: msg_size = " << msg_sz
                  << std::endl;
    }
    std::clog << "[TEST] Server finished work" << std::endl;
}

int main()
{
    auto socket = std::make_shared<connector<udp>>(INADDR_LOOPBACK, 8010);
    socket->bind();

    system("rm -f udp_socket_test.received");
    std::ofstream ofstream("udp_socket_test.received", std::ios::binary);
    std::string file;

    std::thread t(server);
    t.detach();

    auto size = file.length();
    socket->receive(&size, sizeof(size));
    socket->receive(&file, size);

    if (!file.empty())
    {
        ofstream << file;
    }

    system("md5sum udp_socket_test udp_socket_test.received");

    return 0;
}
