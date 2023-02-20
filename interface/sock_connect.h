/**
 * @mainpage
 * Class realize socket connection_ as client
 * socket binding, listening, accepting as server
 *
 * Realize sending, receiving data [int, string]
 *
 * Functional for prototyping the following:
 * - binding address
 * - listening address
 * - accepting new connection_
 * - connecting to something via TCP/UDP/UNIX/USB
 * - sendind data
 * - receiving data
 *
 * The main emphasis is on the controller template and polymorphism.
 */

#ifndef SOCK_CONNECT_SOCK_CONNECT_H
#define SOCK_CONNECT_SOCK_CONNECT_H

#include <string>

#include "tcp.h"
#include "udp.h"
#include "sun.h"
#include "usb.h"

template<class Type>
class connector final
{
public:
    /**
     * @brief Creates new UNIX-Socket
     * @param sun_path Filename of unix socket
     */
    template<typename T>
    explicit connector(T sun_path)
        : connection_(sun_path)
    {}

    /**
     * @brief Creates new Stream-Socket with IP:PORT
     * If constructor creates set status true
     * @param address IP Address
     * @param port Port
     */
    template<typename T, typename U>
    connector(T address, U port)
        : connection_(address, static_cast<uint16_t>(port))
    {}

    ~connector() = default;

    /**
     * @brief Accepting new connection and returns client id
     * @param[out] client_address Write IP address of connected client to it
     * @return client_id
     */
    int accept(std::string *client_address = nullptr)
    { return connection_.accept(client_address); }

    /**
     * @brief Listening address
     * @return Status of execution
     */
    bool listen() const
    { return connection_.listen(); }

    /**
     * @brief Binding address
     * @param listen True - listen after bind, False - bind only
     * @return Status of execution
     */
    bool bind(bool listen = true) const
    { return connection_.bind(listen); }

    /**
     * @brief Connecting to server
     * If connection lost or failed set state to false
     * If connection success set state to true
     * @return Status of execution
     */
    bool connect()
    { return connection_.connect(); }

    /**
     * @brief Shutdown socket descriptor
     * @param id Identifier of the descriptor to disable
     * @return void
     */
    void shutdown(int id = 0)
    { connection_.shutdown(id); }

    /**
     * Receive template function
     * @tparam T type of value
     * @param value pointer to value
     * @param size size of bytes
     * @return Count of received bytes
     */
    template<typename T>
    size_t receive(T *value, size_t size)
    { return connection_.receive(value, size); }

    /**
     * @brief Send template function
     * @tparam T Value type
     * @param value Pointer to value
     * @param size Size of bytes
     * @return Count of sent bytes
     */
    template<typename T>
    size_t send(const T *value, size_t size)
    { return connection_.send(value, size); }

    /**
     * @brief Return socket id
     * @return socket.id
     */
    int id()
    { return connection_.id(); }

    /**
     * Socket or connection status
     */
    bool status() const
    { return connection_.status(); }

    /**
     * Used with multithreading sockets interactions
     * @param id Client ID that has been assigned as descriptor
     */
    void assign_thread(int id)
    { connection_.assign_thread(id); }

private:
    Type connection_;
};

#endif // SOCK_CONNECT_SOCK_CONNECT_H
