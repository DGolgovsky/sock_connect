/**
 * @file Connection.h
 * @author DGolgovsky
 * @date 2018
 * @brief Socket Connection Class
 *
 * Class realize socket connection
 */

/**
 * @mainpage
 * Class realize socket connection as client
 * socket binding, listening, accepting as server
 *
 * Realize sending, receiving data [int, string]
 *
 * Functional for prototyping the following:
 * - binding address
 * - listening address
 * - accepting new connection
 * - connecting to something via TCP
 * - sendind data
 * - receiving data
 *
 * The main emphasis is on the controller template and polymorphism.
 */

#ifndef _LIB_SOCK_CONNECT_CONNECTION_H
#define _LIB_SOCK_CONNECT_CONNECTION_H

#include <utility>
#include <map>
#include <thread>
#include <algorithm>

#include "SocketIp.h"

class Connection
{
protected:
    SocketIp socket_;

    uint32_t address_{};
    uint16_t port_{};
    std::string m_path{};

    sockaddr_in socket_addr{};
    sockaddr_in client_addr{};
    sockaddr_un unix_addr{};
    sockaddr *ptr_addr;
    socklen_t size_addr;

    std::map<std::thread::id, int> clients;
    bool state{false};

public:
    /**
     * @brief Creates new socket with IP:PORT
     * Explicit ctor:
     * @param cp
     * @param addr
     * @param port
     */
    explicit Connection(conn_type cp, uint32_t addr, uint16_t port);
    explicit Connection(conn_type cp, char const *addr, uint16_t port);

    /**
     * @brief Creates new socket unix_path
     * Explicit ctor:
     * @param cp
     * @param path
     */
    explicit Connection(conn_type cp, std::string path);

    /**
     * @brief Deleted unused constructors and operator=
     */
    Connection(Connection &) = delete;
    Connection(Connection &&) = delete;
    Connection(const Connection &) = delete;
    Connection &operator=(Connection &) = delete;
    Connection &operator=(const Connection &) = delete;
    Connection &operator=(Connection &&) = delete;

    void conn_memset();

    /**
     * @brief Virtual destructor
     * Shutdown opened socket
     */
    virtual ~Connection();

    /**
     * @brief Binding address
     * Bind function
     * @return status of execution
     */
    bool Bind(bool listen) const;

    /**
     * @brief Listening address
     * Listen functions
     * @return status of execution
     */
    bool Listen() const;

    /**
     * @brief Accepting new connection
     * Accept client connection and returns client id
     * @return client_id
     */
    int Accept();

    /**
     * @brief Connecting to server
     * Connect
     * If connection lost or failed set state to false
     * If connection success set state to true
     * @return state
     */
    bool Connect();

    /**
     * @brief Shutdown socket
     * Close opened socket
     * @return void
     */
    void Shutdown(int id);

    /**
     * @brief Return socket id
     * @return socket.id
     */
    int id() noexcept;

    /**
     * Connection status
     */
    bool status() const;
    void assign_client(int id);
};

#endif // _LIB_SOCK_CONNECT_CONNECTION_H