/**
 * @file Connection.h
 * @author DGolgovsky
 * @date 2018-2019
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

#ifndef CONNECTION_H
#define CONNECTION_H

#include "SocketIp.h"
#include <fcntl.h>

class Connection {
protected:
    SocketIp socket_;

    uint32_t address_{};
    uint16_t port_{};
    std::string m_path{};

    sockaddr_in socket_addr{};
    sockaddr_in client_addr{};
    sockaddr_un unix_addr;
    sockaddr *ptr_addr;

    int transmission{};
    bool state{false};

public:
    /**
     * @brief Constructor of two arguments
     * Explicit ctor with two arguments:
     * @param address
     * @param port
     * Create new socket
     */
    explicit Connection(conn_type cp, uint32_t address, uint16_t port);

    explicit Connection(conn_type cp, std::string path);

    /**
     * @brief Deleted constructors and operator=
     */
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
     * @return void
     */
    bool Bind(bool listen) const;

    /**
     * @brief Listening address
     * Listen functions
     * @return void
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
    void Shutdown(int id) const;

    /**
     * @brief Return socket id
     * @return socket.id
     */
    int id() const noexcept;

    /**
     * Connection status
     */
    bool status() const;
};

#endif // CONNECTION_H