/**
 * @file connection.h
 * @author DGolgovsky
 * @date 2018
 * @brief Socket connection Class
 *
 * Class realize socket connection
 */

#ifndef SOCK_CONNECT_CONNECTION_H
#define SOCK_CONNECT_CONNECTION_H

#include <vector>
#include <thread>

#include "include/socket_ip.h"
#ifndef NDEBUG
#include "include/helpers.h"
#endif

class connection
{
    using storage_t = std::vector<std::pair<std::thread::id, int>>; // state_ for each connection

public:
    connection(conn_type cp, uint32_t address, uint16_t port);
    connection(conn_type cp, char const *address, uint16_t port);
    connection(conn_type cp, std::string const &address, uint16_t port);
    connection(conn_type cp, char const &&address, uint16_t port);
    connection(conn_type cp, std::string const &path);
    connection(conn_type cp, char const *path);
    virtual ~connection();

    bool bind(bool listen) const;
    bool listen() const;
    int accept(std::string *client_address);
    bool connect();
    void shutdown(int id);

    int id() const noexcept;
    bool status() const noexcept;
    void assign_thread(int id);

    /**
     * @brief Deleted unused constructors and operator=
     */
    connection(connection &) = delete;
    connection(connection &&) = delete;
    connection(const connection &) = delete;
    connection &operator=(connection &) = delete;
    connection &operator=(const connection &) = delete;
    connection &operator=(connection &&) = delete;
protected:
    void conn_memset();
    int get_descriptor();

protected:
    socket_ip socket_;

    uint32_t address_{};
    uint16_t port_{};
    std::string m_path_{};

    sockaddr_in socket_addr_{};
    sockaddr_in client_addr_{};
    sockaddr_un unix_addr_{};
    sockaddr *ptr_addr_;
    socklen_t size_addr_;

    storage_t *client_list_{};
    bool state_{false};
};

#endif // SOCK_CONNECT_CONNECTION_H
