#include <fstream>
#include <random>
#include <chrono>
#include <memory>
#include <netinet/in.h>
#include <iostream>
#include <thread>

#include "interface/sock_connect.h"

void server()
{
    size_t const array_size = 8;
    unsigned seed = static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count());
    std::default_random_engine generator(seed);
    std::uniform_int_distribution<int> distribution(0, 64);

    uint8_t ui8 = 8;
    uint16_t ui16 = 16;
    uint32_t ui32 = 32;
    uint64_t ui64 = 64;

    uint8_t ui8a[array_size];
    uint16_t ui16a[array_size];
    uint32_t ui32a[array_size];
    uint64_t ui64a[array_size];

    std::fill(ui8a, ui8a + array_size, distribution(generator));
    std::fill(ui16a, ui16a + array_size, distribution(generator));
    std::fill(ui32a, ui32a + array_size, distribution(generator));
    std::fill(ui64a, ui64a + array_size, distribution(generator));

    std::ifstream fstream("test_tcp_socket", std::ios::binary);
    std::string file;
    fstream.seekg(0, std::ios::end);
    file.reserve(static_cast<unsigned long>(fstream.tellg()));
    fstream.seekg(0, std::ios::beg);
    file.assign((std::istreambuf_iterator<char>(fstream)),
                std::istreambuf_iterator<char>());

    auto socket = std::make_shared<connector<tcp>>(INADDR_LOOPBACK, 8010);
    if (!socket->bind())
    {
        return;
    }
    if (socket->accept())
    {
        size_t size = file.length();
        socket->send(&ui8, sizeof(ui8));
        socket->send(&ui16, sizeof(ui16));
        socket->send(&ui32, sizeof(ui32));
        socket->send(&ui64, sizeof(ui64));

        socket->send(ui8a, sizeof(ui8) * array_size);
        socket->send(ui16a, sizeof(ui16) * array_size);
        socket->send(ui32a, sizeof(ui32) * array_size);
        socket->send(ui64a, sizeof(ui64) * array_size);

        socket->send(&size, sizeof(size));
        socket->send(&file, size);
    }
    std::clog << "[TEST] Server finished work" << std::endl;
}

int main()
{
    system("rm -f test_tcp_socket.received");
    std::thread t(server);
    t.detach();

    auto socket = std::make_shared<connector<tcp>>(INADDR_LOOPBACK, 8010);

    std::ofstream ofstream("test_tcp_socket.received", std::ios::binary);
    std::string file;
    size_t const array_size = 8;
    uint8_t ui8 = 0;
    uint16_t ui16 = 0;
    uint32_t ui32 = 0;
    uint64_t ui64 = 0;

    uint8_t ui8a[array_size];
    uint16_t ui16a[array_size];
    uint32_t ui32a[array_size];
    uint64_t ui64a[array_size];

    while (socket->status())
    {
        if (socket->connect())
        {
            socket->receive(&ui8, sizeof(ui8));
            socket->receive(&ui16, sizeof(ui16));
            socket->receive(&ui32, sizeof(ui32));
            socket->receive(&ui64, sizeof(ui64));

            socket->receive(ui8a, sizeof(ui8) * array_size);
            socket->receive(ui16a, sizeof(ui16) * array_size);
            socket->receive(ui32a, sizeof(ui32) * array_size);
            socket->receive(ui64a, sizeof(ui64) * array_size);

            size_t size = 0;
            socket->receive(&size, sizeof(size));
            socket->receive(&file, size);

            if (!file.empty())
            {
                ofstream << file;
            }
            socket->shutdown();
            std::clog << "[TEST] Client finished work" << std::endl;
        }
    }
    system("md5sum test_tcp_socket test_tcp_socket.received");

    return 0;
}