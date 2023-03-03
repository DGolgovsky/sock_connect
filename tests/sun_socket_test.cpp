#include <fstream>
#include <iostream>
#include <memory>
#include <thread>

#include "tests/helpers.h"
#include "interface/sock_connect.h"

static void server()
{
    v.randomize_fill();

    std::ifstream fstream("sun_socket_test", std::ios::binary);
    std::string file_content;
    fstream.seekg(0, std::ios::end);
    file_content.reserve(static_cast<unsigned long>(fstream.tellg()));
    fstream.seekg(0, std::ios::beg);
    file_content.assign(std::istreambuf_iterator<char>(fstream),
                        std::istreambuf_iterator<char>());

    auto socket = std::make_shared<connector<sun>>(sun_path);
    socket->bind();
    if (socket->accept())
    {
        socket->send(&v.ui_8, sizeof(v.ui_8));
        socket->send(&v.ui_16, sizeof(v.ui_16));
        socket->send(&v.ui_32, sizeof(v.ui_32));
        socket->send(&v.ui_64, sizeof(v.ui_64));

        socket->send(v.ui_8_a, sizeof(v.ui_8) * array_size);
        socket->send(v.ui_16_a, sizeof(v.ui_16) * array_size);
        socket->send(v.ui_32_a, sizeof(v.ui_32) * array_size);
        socket->send(v.ui_64_a, sizeof(v.ui_64) * array_size);

        size_t const file_size = file_content.length();
        socket->send(&file_size, sizeof(file_size));
        socket->send(&file_content, file_size);
    }
    std::cout << "[TEST] Sender (server) finished work" << std::endl;
}

int main(int, char **)
{
    system("rm -f sun_socket_test.received /tmp/sun_path");
    std::thread t(server);
    t.detach();

    std::ofstream ofstream("sun_socket_test.received", std::ios::binary);
    value_storage recv_v{};

    auto socket = std::make_shared<connector<sun>>(sun_path);
    while (socket->status())
    {
        if (socket->connect())
        {
            socket->receive(&recv_v.ui_8, sizeof(recv_v.ui_8));
            socket->receive(&recv_v.ui_16, sizeof(recv_v.ui_16));
            socket->receive(&recv_v.ui_32, sizeof(recv_v.ui_32));
            socket->receive(&recv_v.ui_64, sizeof(recv_v.ui_64));

            socket->receive(recv_v.ui_8_a, sizeof(recv_v.ui_8) * array_size);
            socket->receive(recv_v.ui_16_a, sizeof(recv_v.ui_16) * array_size);
            socket->receive(recv_v.ui_32_a, sizeof(recv_v.ui_32) * array_size);
            socket->receive(recv_v.ui_64_a, sizeof(recv_v.ui_64) * array_size);

            std::string file_content;
            size_t file_size = 0;
            socket->receive(&file_size, sizeof(file_size));
            socket->receive(&file_content, file_size);

            socket->shutdown();
            std::cout << "[TEST] Receiver (client) finished work" << std::endl;

            if (!file_content.empty())
            {
                ofstream << file_content;
            }
        }
    }
    std::cout << "[TEST] Transferred int values ";
    if (recv_v == v)
    {
        std::cout << "is the same\n";
    }
    else
    {
        std::cout << "isn't the same\n";
    }

    system("md5sum sun_socket_test sun_socket_test.received");
    system("rm -f sun_socket_test.received /tmp/sun_path");

    return 0;
}
