#include <server/core/connection_manager.hpp>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>

int ConnectionManager::acceptConnection()
{
    struct sockaddr_in clientAddr;
    socklen_t addrLen = sizeof(clientAddr);
    int client_fd = accept(server_fd, reinterpret_cast<struct sockaddr *>(&clientAddr), &addrLen);
    if (client_fd < 0)
    {
        return -1;
    }
    connectionMap[client_fd] = "";
    return client_fd;
}

bool ConnectionManager::closeConnection(int client_fd)
{
    auto it = connectionMap.find(client_fd);
    if (it == connectionMap.end())
    {
        return false;
    }
    close(client_fd);
    connectionMap.erase(it);
    return true;
}

bool ConnectionManager::sendMessage(int client_fd, const std::string &message)
{
    if (connectionMap.find(client_fd) == connectionMap.end())
    {
        return false;
    }
    uint32_t len = static_cast<uint32_t>(message.size());
    if (send(client_fd, &len, sizeof(len), 0) != sizeof(len))
    {
        return false;
    }
    ssize_t sent = send(client_fd, message.c_str(), message.size(), 0);
    return sent == static_cast<ssize_t>(message.size());
}

std::string ConnectionManager::receiveMessage(int client_fd)
{
    uint32_t len = 0;
    ssize_t received = recv(client_fd, &len, sizeof(len), MSG_WAITALL);
    if (received != sizeof(len) || len == 0 || len > 65536)
    {
        return "";
    }
    std::string buffer(len, '\0');
    received = recv(client_fd, &buffer[0], len, MSG_WAITALL);
    if (received != static_cast<ssize_t>(len))
    {
        return "";
    }
    return buffer;
}
bool ConnectionManager::updateConnectionMapping(int client_fd, const std::string &userName)
{
    auto it = connectionMap.find(client_fd);
    if (it == connectionMap.end())
    {
        return false;
    }
    it->second = userName;
    return true;
}
bool ConnectionManager::closeConnection(const std::string &userName)
{
    for (auto it = connectionMap.begin(); it != connectionMap.end(); ++it)
    {
        if (it->second == userName)
        {
            close(it->first);
            connectionMap.erase(it);
            return true;
        }
    }
    return false;
}