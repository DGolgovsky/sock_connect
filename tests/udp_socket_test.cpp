#include <fstream>
#include <memory>
#include <netinet/in.h>
#include <iostream>
#include <thread>

#include "interface/sock_connect.h"

void server()
{
    std::ifstream fstream("test_udp_socket", std::ios::binary);
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
        std::clog << "[TEST] File doesn't sent: msg_size = " << msg_sz << std::endl;
    }
    std::clog << "[TEST] Server finished work" << std::endl;
}

int main()
{
    auto socket = std::make_shared<connector<udp>>(INADDR_LOOPBACK, 8010);
    if (!socket->bind(false))
    {
        return 0;
    }

    system("rm -f test_udp_socket.received");
    std::ofstream ofstream("test_udp_socket.received", std::ios::binary);
    std::string file;

    std::thread t(server);
    t.detach();

    auto size = file.length();
    socket->receive(&size, sizeof(size));
    socket->receive(&file, size);
    //socket->shutdown(); //TODO Fix exception

    if (!file.empty())
    {
        ofstream << file;
    }

    system("md5sum test_udp_socket test_udp_socket.received");

    return 0;
}
