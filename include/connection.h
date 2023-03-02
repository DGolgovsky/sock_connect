/**
 * @file connection.h
 * @author Dmitry Golgovsky
 * @date 2018
 * @brief Socket connection Class
 *
 * Class realize socket connection
 */

#ifndef SOCK_CONNECT_CONNECTION_H
#define SOCK_CONNECT_CONNECTION_H

#include <vector>
#include <string>
#include <thread>

#ifndef NDEBUG
#include "include/debug_output.h"
#endif
#include "include/socket_id.h"

class connection
{
    /**
     * @brief Inner storage for each connection state
     */
    using storage_t = std::vector<std::pair<std::thread::id, int>>;

public:
    connection(conn_type cp, uint32_t address, uint16_t port);
    connection(conn_type cp, char const *address, uint16_t port);
    connection(conn_type cp, std::string const &address, uint16_t port);
    connection(conn_type cp, char const &&address, uint16_t port);
    connection(conn_type cp, std::string const &sun_path);
    connection(conn_type cp, char const *path);
    virtual ~connection();

    void bind(bool listen) const;
    void listen() const;
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
    connection(connection const &) = delete;
    connection &operator=(connection &) = delete;
    connection &operator=(connection const &) = delete;
    connection &operator=(connection &&) = delete;
protected:
    void conn_memset();
    int get_descriptor();

protected:
    socket_id id_;

    uint32_t ip_address_{};
    uint16_t ip_port_{};
    std::string sun_path_{};

    sockaddr_in self_socket_{};
    sockaddr_in client_socket_{};
    sockaddr_un unix_socket_{};
    sockaddr *addr_ptr_{};
    socklen_t addr_size_{};

    storage_t *client_list_{};
    bool state_{false};
};

#endif // SOCK_CONNECT_CONNECTION_H
