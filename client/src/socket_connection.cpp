#include <client/socket_connection.hpp>

#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>

#include <array>
#include <cerrno>
#include <cstring>
#include <stdexcept>

// ---- helpers -----------------------------------------------------------

static void writeAll(int fd, const char *buf, std::size_t len)
{
    while (len > 0)
    {
        ssize_t n = ::send(fd, buf, len, MSG_NOSIGNAL);
        if (n < 0 && errno == EINTR)
            continue;
        if (n <= 0)
            throw std::runtime_error("SocketConnection: write failed");
        buf += n;
        len -= static_cast<std::size_t>(n);
    }
}

static bool readAll(int fd, char *buf, std::size_t len)
{
    while (len > 0)
    {
        ssize_t n = ::read(fd, buf, len);
        if (n < 0 && errno == EINTR)
            continue;
        if (n == 0)
            return false;
        if (n < 0)
            throw std::runtime_error("SocketConnection: read failed");
        buf += n;
        len -= static_cast<std::size_t>(n);
    }
    return true;
}

// ---- SocketConnection --------------------------------------------------

SocketConnection::~SocketConnection()
{
    disconnect();
}

SocketConnection::SocketConnection(SocketConnection &&other) noexcept
    : fd(other.fd)
{
    other.fd = -1;
}

SocketConnection &SocketConnection::operator=(SocketConnection &&other) noexcept
{
    if (this != &other)
    {
        disconnect();
        fd = other.fd;
        other.fd = -1;
    }
    return *this;
}

void SocketConnection::connect(const std::string &host, int port)
{
    if (fd >= 0)
        disconnect();

    addrinfo hints{};
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    addrinfo *res = nullptr;
    if (getaddrinfo(host.c_str(), std::to_string(port).c_str(), &hints, &res) != 0 || res == nullptr)
        throw std::runtime_error("SocketConnection: cannot resolve host '" + host + "'");

    fd = ::socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (fd < 0)
    {
        freeaddrinfo(res);
        throw std::runtime_error("SocketConnection: socket() failed");
    }

    if (::connect(fd, res->ai_addr, res->ai_addrlen) != 0)
    {
        freeaddrinfo(res);
        ::close(fd);
        fd = -1;
        throw std::runtime_error("SocketConnection: connect() failed");
    }

    freeaddrinfo(res);
}

void SocketConnection::send(const std::string &data)
{
    if (fd < 0)
        throw std::runtime_error("SocketConnection: not connected");

    try
    {
        uint32_t len = htonl(static_cast<uint32_t>(data.size()));
        writeAll(fd, reinterpret_cast<const char *>(&len), sizeof(len));
        writeAll(fd, data.data(), data.size());
    }
    catch (...)
    {
        disconnect();
        throw;
    }
}

bool SocketConnection::recv(std::string &out)
{
    if (fd < 0)
        throw std::runtime_error("SocketConnection: not connected");

    try
    {
        uint32_t netLen = 0;
        if (!readAll(fd, reinterpret_cast<char *>(&netLen), sizeof(netLen)))
        {
            disconnect();
            return false;
        }

        uint32_t len = ntohl(netLen);
        out.resize(len);
        if (!readAll(fd, out.data(), len))
        {
            disconnect();
            return false;
        }

        return true;
    }
    catch (...)
    {
        disconnect();
        throw;
    }
}

void SocketConnection::disconnect()
{
    if (fd >= 0)
    {
        ::shutdown(fd, SHUT_RDWR);
        ::close(fd);
        fd = -1;
    }
}
