#pragma once
#include <common/models/connection.hpp>
#include <common/repos/message_repo.hpp>
#include <common/repos/user_repo.hpp>
#include <common/repos/session_repo.hpp>

#include <unordered_map>
class ConnectionManager
{
    ConnectionManager(int server_fd, std::unordered_map<int, Connection> map) : server_fd(server_fd), connectionMap(map) {}

public:
    bool acceptConnection();
    bool sendMessage(std::string &message, std::string &username);
    bool receiveMessage(std::string &username);
    const int getServerFD() const
    {
        return server_fd;
    }
    const std::unordered_map<int, Connection> getConnectionMap() const
    {
        return connectionMap;
    }

private:
    int server_fd;
    std::unordered_map<int, Connection> connectionMap; // map clietnt fd to connection
};