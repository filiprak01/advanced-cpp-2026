#pragma once
#include <string>
#include <common/models/session.hpp>

class Connection
{
    Connection(std::string &socket, Session &session) : socket(socket), session(session) {}

public:
    const std::string &getSocket() const
    {
        return socket;
    }
    const Session &getSession() const
    {
        return session;
    }
    bool operator==(const Connection &other) const;

private:
    std::string socket;
    Session session;
};
namespace std
{
    template <>
    struct hash<Connection>
    {
        std::size_t operator()(const Connection &connection) const
        {
            return std::hash<std::string>{}(connection.getSocket());
        }
    };
}
