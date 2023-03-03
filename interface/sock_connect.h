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
 * - accepting new connections
 * - connecting to something via TCP/UDP/UNIX/USB
 * - sending data
 * - receiving data
 *
 * The main emphasis is on the controller template and polymorphism
 */

#ifndef SOCK_CONNECT_SOCK_CONNECT_H
#define SOCK_CONNECT_SOCK_CONNECT_H

#include <string>

#include "tcp.h"
#include "udp.h"
#include "sun.h"
#include "usb.h"

template <class Type>
class connector final
{
public:
    /**
     * @brief Create new UNIX-Socket
     * @param sun_path Filename of unix sun socket
     */
    template <typename T>
    explicit connector(T sun_path)
        : connection_(sun_path)
    {}

    /**
     * @brief Create new Stream-Socket with IP:PORT
     * @param address IP Address
     * @param port IP Port
     */
    template <typename T, typename U>
    connector(T address, U port)
        : connection_(address, static_cast<uint16_t>(port))
    {}

    /**
     * @brief Accept new connection and returns client id
     * @param[out] client_address Write IP address of connected client
     * @return Client id descriptor
     */
    int accept(std::string *client_address = nullptr)
    { return connection_.accept(client_address); }

    /**
     * @brief Listen TCP address
     */
    void listen() const
    { connection_.listen(); }

    /**
     * @brief Bind IP address
     * @param listen True - listen after bind, False - bind only
     * @throw std::runtime_error Can't bind IP address
     */
    void bind(bool listen = true) const
    { connection_.bind(listen); }

    /**
     * @brief Connect to server
     * If connection lost or failed set state to false
     * If connection success set state to true
     * @return Status of execution
     */
    bool connect()
    { return connection_.connect(); }

    /**
     * @brief Shutdown socket descriptor
     * @param id Identifier of the descriptor to disable
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
    template <typename T>
    size_t receive(T *value, size_t size)
    { return connection_.receive(value, size); }

    /**
     * @brief Send template function
     * @tparam T Value type
     * @param value Pointer to value
     * @param size Size of bytes
     * @return Count of sent bytes
     */
    template <typename T>
    size_t send(const T *value, size_t size)
    { return connection_.send(value, size); }

    /**
     * @brief Get socket id
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

    ~connector() = default;

private:
    Type connection_;
};

#endif // SOCK_CONNECT_SOCK_CONNECT_H
