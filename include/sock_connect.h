#ifndef SOCK_CONNECT_H
#define SOCK_CONNECT_H

#include "TCP.h"
#include "UDP.h"
#include "UNIX.h"
#include "USB.h"

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
 * - connecting to something via TCP/UDP/UNIX/USB
 * - sendind data
 * - receiving data
 *
 * The main emphasis is on the controller template and polymorphism.
 */
template<class Type>
class Connector final
{
private:
    Type connection;

public:
    /**
     * @brief Creates new UNIX-Socket
     * Explicit ctor:
     * @param sun_path
     */
    template<typename T>
    explicit Connector(T sun_path) : connection(sun_path) {}

    /**
     * @brief Creates new Stream-Socket with IP:PORT
     * If constructor creates set status true
     * @param addr IP Address
     * @param port Port
     */
    template<typename T, typename U>
    Connector(T addr, U port) : connection(addr, static_cast<uint16_t>(port)) {}

    ~Connector() = default;

    /**
     * @brief Accepting new connection
     * Accept client connection and returns client id
     * If parameter passed write ip address of connected client to it
     * @param client_address
     * @return client_id
     */
    int Accept(std::string *client_address = nullptr) { return connection.Accept(client_address); }

    /**
     * @brief Listening address
     * Listen functions
     * @return Status of execution
     */
    bool Listen() const { return connection.Listen(); }

    /**
     * @brief Binding address
     * Bind function
     * @return Status of execution
     */
    bool Bind(bool listen = true) const { return connection.Bind(listen); }

    /**
     * @brief Connecting to server
     * Connect
     * If connection lost or failed set state to false
     * If connection success set state to true
     * @return state
     */
    bool Connect() { return connection.Connect(); }

    /**
     * @brief Shutdown socket
     * Close opened socket
     * @param id Disconnected descriptor id
     * @return void
     */
    void Shutdown(int id = 0) { return connection.Shutdown(id); }

    /**
     * Send template function
     * @tparam T type of value
     * @param value pointer to value
     * @param size size of bytes
     * @return count of sent bytes
     */
    template<typename T>
    std::size_t Send(const T *value, std::size_t size) {
        return connection.Send(value, size);
    }

    /**
     * Receive template function
     * @tparam T type of value
     * @param value pointer to value
     * @param size size of bytes
     * @return count of received bytes
     */
    template<typename T>
	std::size_t Receive(T *value, std::size_t size) {
        return connection.Receive(value, size);
    }

    /**
     * @brief Return socket id
     * @return socket.id
     */
    int id() { return connection.id(); }

    /**
     * Socket or connection status
     */
    bool Status() const { return connection.status(); }

    /**
     * Used with multithreading sockets interactions
     * @param id Client ID that has been assigned as descriptor
     */
    void assign_thread(int id) { return connection.assign_thread(id); }
};

#endif // SOCK_CONNECT_H
