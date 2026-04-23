#pragma once
#include <string>

/**
 * @brief Wraps a POSIX TCP socket for the PWChat client.
 *
 * Provides a simple blocking interface: connect, send framed JSON,
 * receive framed JSON, and disconnect.  Each message on the wire is
 * a 4-byte big-endian length prefix followed by the UTF-8 JSON body.
 */
class SocketConnection
{
public:
    SocketConnection() = default;
    ~SocketConnection();

    // Non-copyable, movable
    SocketConnection(const SocketConnection &) = delete;
    SocketConnection &operator=(const SocketConnection &) = delete;
    SocketConnection(SocketConnection &&other) noexcept;
    SocketConnection &operator=(SocketConnection &&other) noexcept;

    /**
     * @brief Opens a TCP connection to host:port.
     * @throws std::runtime_error on failure.
     */
    void connect(const std::string &host, int port);

    /**
     * @brief Sends a JSON string (length-prefixed).
     * @throws std::runtime_error if the socket is not connected or on write error.
     */
    void send(const std::string &data);

    /**
     * @brief Receives one complete JSON string (length-prefixed).
     * @param out  Destination string; populated on success.
     * @return     @c true on success, @c false when the peer closed the connection.
     * @throws std::runtime_error on read error.
     */
    bool recv(std::string &out);

    /// @brief Closes the socket gracefully.
    void disconnect();

    /// @brief Returns @c true if the socket is open.
    bool isConnected() const { return fd >= 0; }

private:
    int fd{-1}; ///< Underlying file descriptor; -1 means disconnected.
};
