#include <fstream>
#include <iostream>
#include <memory>
#include <thread>

#include "tests/helpers.h"
#include "interface/sock_connect.h"

static void server()
{
    v.randomize_fill();

    std::ifstream fstream("udp_socket_test", std::ios::binary);
    std::string file_content;
    fstream.seekg(0, std::ios::end);
    file_content.reserve(static_cast<unsigned long>(fstream.tellg()));
    fstream.seekg(0, std::ios::beg);
    file_content.assign(std::istreambuf_iterator<char>(fstream),
                        std::istreambuf_iterator<char>());

    auto socket = std::make_shared<connector<udp>>(ip_addr, ip_port);

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
    auto msg_sz = socket->send(&file_content, file_size);

    if (msg_sz < file_size)
    {
        std::cout << "[TEST] File doesn't sent: msg_size = " << msg_sz << '\n';
    }
    std::cout << "[TEST] Sender finished work" << std::endl;
}

int main(int, char **)
{
    auto socket = std::make_shared<connector<udp>>(ip_addr, ip_port);
    socket->bind();

    system("rm -f udp_socket_test.received");
    std::ofstream ofstream("udp_socket_test.received", std::ios::binary);
    value_storage recv_v{};

    std::thread t(server);
    t.detach();

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

    std::cout << "[TEST] Receiver finished work" << std::endl;

    if (!file_content.empty())
    {
        ofstream << file_content;
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

    system("md5sum udp_socket_test udp_socket_test.received");
    system("rm -f udp_socket_test.received");

    return 0;
}
