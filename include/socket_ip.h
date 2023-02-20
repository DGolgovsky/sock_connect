/**
 * @file socket_ip.h
 * @author DGolgovsky
 * @date 2018
 * @brief Socket connection_ support Class
 *
 * Class realize socket connection
 */

#ifndef SOCK_CONNECT_SOCKET_IP_H
#define SOCK_CONNECT_SOCKET_IP_H

#include <arpa/inet.h>
#include <netinet/in.h>

#include <sys/socket.h>
#include <sys/un.h>

#include <unistd.h>
#include <stdexcept>

enum conn_type: char
{
    _TCP, _UDP, _UNIX
};

class socket_ip
{
public:
    explicit socket_ip(conn_type ct)
    {
        switch (ct)
        {
            case _TCP  :
                m_id_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
                conn_text_ = "TCP";
                break;
            case _UDP  :
                m_id_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
                conn_text_ = "UDP";
                break;
            case _UNIX :
                m_id_ = socket(AF_UNIX, SOCK_STREAM, 0);
                conn_text_ = "sun";
                break;
        }

        if (m_id_ < 0)
        {
            throw std::runtime_error("Socket was not created, error number: " + std::to_string(errno));
        }
    }

    explicit socket_ip(int socket_id)
        : m_id_(socket_id)
    {}

    socket_ip(socket_ip &&other) noexcept
        : m_id_(other.m_id_)
    {
        other.m_id_ = -1;
    }

    socket_ip(const socket_ip &) = delete;
    socket_ip &operator=(const socket_ip &) = delete;
    socket_ip &operator=(socket_ip &&) = delete;

    ~socket_ip()
    {
        if (m_id_ >= 0)
        {
            shutdown(m_id_, SHUT_RDWR);
        }
    }

    int id() const noexcept
    {
        return m_id_;
    }

    std::string c_type() const noexcept
    {
        return conn_text_;
    }

private:
    int m_id_{};
    std::string conn_text_{};
};

#endif // SOCK_CONNECT_SOCKET_IP_H
