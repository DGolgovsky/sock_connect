/**
 * @file socket_id.h
 * @author Dmitry Golgowski
 * @date 2018
 * @brief Socket connection support class
 */

#ifndef SOCK_CONNECT_SOCKET_ID_H
#define SOCK_CONNECT_SOCKET_ID_H

#include <arpa/inet.h>
#include <netinet/in.h>

#include <sys/socket.h>
#include <sys/un.h>

#include <unistd.h>
#include <stdexcept>

enum conn_type : char
{
    TCP,
    UDP,
    SUN,
    USB
};

class socket_id final
{
public:
    explicit socket_id(conn_type ct)
        : id_(-1)
        , type_()
    {
        switch (ct)
        {
            case TCP:
                id_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
                type_ = "tcp";
                break;
            case UDP:
                id_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
                type_ = "udp";
                break;
            case SUN:
                id_ = socket(AF_UNIX, SOCK_STREAM, 0);
                type_ = "sun";
                break;
            case USB:
                id_ = 0;
                type_ = "usb";
                break;
        }

        if (id_ < 0)
        {
            throw std::runtime_error("Socket was not created, error number: " +
                                     std::to_string(errno));
        }
    }

    explicit socket_id(int id)
        : id_(id)
        , type_("cli")
    {}

    socket_id(socket_id &&other) noexcept
        : id_(other.id_)
        , type_(std::move(other.type_))
    {
        other.id_ = -1;
        other.type_ = "";
    }

    socket_id(socket_id const &) = delete;
    socket_id &operator=(socket_id const &) = delete;
    socket_id &operator=(socket_id &&) = delete;

    ~socket_id()
    {
        if (id_ >= 0)
        {
            shutdown(id_, SHUT_RDWR);
        }
    }

    /**
     * @brief Get socket identification
     * @return Socket ID
     */
    int id() const noexcept
    { return id_; }

    void set_id(int id)
    { id_ = id; }

    /**
     * @brief Get Connection text type
     * @return Type string
     */
    std::string connection_type() const noexcept
    { return type_; }

    operator int() const noexcept
    { return id_; }
    bool operator==(int compare) const noexcept
    { return id_ == compare; }
    bool operator!=(int compare) const noexcept
    { return id_ != compare; }
    bool operator>=(int compare) const noexcept
    { return id_ >= compare; }

private:
    int id_;
    std::string type_;
};

#endif // SOCK_CONNECT_SOCKET_ID_H
