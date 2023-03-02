#include <fstream>
#include <random>
#include <chrono>
#include <memory>
#include <iostream>
#include <thread>

#include "interface/sock_connect.h"

static void server()
{
    size_t const array_size = 8;
    unsigned seed = static_cast<unsigned int>
    (std::chrono::system_clock::now().time_since_epoch().count());
    std::default_random_engine generator(seed);
    std::uniform_int_distribution<int> distribution(0, 64);

    uint8_t ui_8 = 8;
    uint16_t ui_16 = 16;
    uint32_t ui_32 = 32;
    uint64_t ui_64 = 64;

    uint8_t ui_8_a[array_size];
    uint16_t ui_16_a[array_size];
    uint32_t ui_32_a[array_size];
    uint64_t ui_64_a[array_size];

    std::fill(ui_8_a, ui_8_a + array_size, distribution(generator));
    std::fill(ui_16_a, ui_16_a + array_size, distribution(generator));
    std::fill(ui_32_a, ui_32_a + array_size, distribution(generator));
    std::fill(ui_64_a, ui_64_a + array_size, distribution(generator));

    std::ifstream fstream("sun_socket_test", std::ios::binary);
    std::string file;
    fstream.seekg(0, std::ios::end);
    file.reserve(static_cast<unsigned long>(fstream.tellg()));
    fstream.seekg(0, std::ios::beg);
    file.assign((std::istreambuf_iterator<char>(fstream)),
                std::istreambuf_iterator<char>());

    std::string sun_path = "/tmp/sun_path";
    auto socket = std::make_shared<connector<sun>>(sun_path);

    socket->bind();
    if (socket->accept())
    {
        size_t size = file.length();
        socket->send(&ui_8, sizeof(ui_8));
        socket->send(&ui_16, sizeof(ui_16));
        socket->send(&ui_32, sizeof(ui_32));
        socket->send(&ui_64, sizeof(ui_64));

        socket->send(ui_8_a, sizeof(ui_8) * array_size);
        socket->send(ui_16_a, sizeof(ui_16) * array_size);
        socket->send(ui_32_a, sizeof(ui_32) * array_size);
        socket->send(ui_64_a, sizeof(ui_64) * array_size);

        //std::clog << "[TEST] SENT: ui_8 = " << (int)ui_8 << "; ui_16 = " << ui_16 << "; ui_32 = " << ui_32 << std::endl;
        socket->send(&size, sizeof(size));
        socket->send(&file, size);
    }
    std::clog << "[TEST] Server finished work" << std::endl;
}

int main()
{
    system("rm -f sun_socket_test.received /tmp/sun_path");
    std::thread t(server);
    t.detach();

    auto socket = std::make_shared<connector<sun>>("/tmp/sun_path");

    std::ofstream ofstream("sun_socket_test.received", std::ios::binary);
    std::string file;
    size_t const array_size = 8;
    uint8_t ui_8 = 0;
    uint16_t ui_16 = 0;
    uint32_t ui_32 = 0;
    uint64_t ui_64 = 0;

    uint8_t ui_8_a[array_size];
    uint16_t ui_16_a[array_size];
    uint32_t ui_32_a[array_size];
    uint64_t ui_64_a[array_size];

    while (socket->status())
    {
        if (socket->connect())
        {
            socket->receive(&ui_8, sizeof(ui_8));
            socket->receive(&ui_16, sizeof(ui_16));
            socket->receive(&ui_32, sizeof(ui_32));
            socket->receive(&ui_64, sizeof(ui_64));

            socket->receive(ui_8_a, sizeof(ui_8) * array_size);
            socket->receive(ui_16_a, sizeof(ui_16) * array_size);
            socket->receive(ui_32_a, sizeof(ui_32) * array_size);
            socket->receive(ui_64_a, sizeof(ui_64) * array_size);

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
    system("md5sum sun_socket_test sun_socket_test.received");

    return 0;
}
