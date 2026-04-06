#pragma once
#include <common/models/connection.hpp>
#include <server/repos/message_repo.hpp>
#include <server/repos/user_repo.hpp>
#include <server/repos/session_repo.hpp>

#include <unordered_map>
#include <string>

class ConnectionManager
{
public:
    ConnectionManager(int server_fd) : server_fd(server_fd) {}

    int acceptConnection();
    bool closeConnection(int client_fd);
    bool closeConnection(const std::string &userName);
    bool sendMessage(int client_fd, const std::string &message);
    std::string receiveMessage(int client_fd);
    bool updateConnectionMapping(int client_fd, const std::string &userName);
    const int getServerFD() const
    {
        return server_fd;
    }
    const std::unordered_map<int, std::string> &getConnectionMap() const
    {
        return connectionMap;
    }

private:
    int server_fd;
    std::unordered_map<int, std::string> connectionMap;
};