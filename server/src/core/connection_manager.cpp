#include <server/core/connection_manager.hpp>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
int ConnectionManager::acceptConnection()
{
    struct sockaddr_in clientAddr;
    socklen_t addrLen = sizeof(clientAddr);
    int client_fd = accept(server_fd, reinterpret_cast<struct sockaddr *>(&clientAddr), &addrLen);
    if (client_fd < 0)
    {
        return -1;
    }
    std::cout << "Accepted new connection: " << client_fd << std::endl;
    connectionMap[client_fd] = "";
    return client_fd;
}

bool ConnectionManager::registerConnection(int client_fd, const std::string &userName)
{
    connectionMap[client_fd] = userName;
    return true;
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
    std::cout << "Closed connection: " << client_fd << std::endl;
    return true;
}

bool ConnectionManager::sendMessage(int fd, const json &message)
{
    std::string data_str = message.dump();
    uint32_t len = static_cast<uint32_t>(data_str.size());
    uint32_t len_network = htonl(len);

    if (send(fd, &len_network, sizeof(len_network), 0) != sizeof(len_network))
    {
        return false;
    }
    std::cout << "Sending message to fd=" << fd << ": " << message << std::endl;
    ssize_t sent = send(fd, data_str.c_str(), data_str.size(), 0);
    return sent == static_cast<ssize_t>(data_str.size());
}

bool ConnectionManager::sendErrorMessage(int fd, const DomainResult &result)
{
    if (result.isSuccess())
    {
        return false;
    }

    json message = {
        {"type", "error"},
        {"status", "error"},
        {"errorType", result.getResponseType()},
        {"payload", {{"reason", result.getMessage()}}}};
    return sendMessage(fd, message);
}

bool ConnectionManager::sendSuccessMessage(int fd, std::string_view type, const DomainResult &result, const json &payload)
{
    if (!result.isSuccess())
    {
        return false;
    }

    json message = {
        {"type", type},
        {"status", "ok"},
        {"message", result.getMessage()},
        {"payload", payload}};
    return sendMessage(fd, message);
}
json ConnectionManager::receiveMessage(int client_fd)
{
    while (true)
    {
        uint32_t len_network;
        ssize_t received = recv(client_fd, &len_network, sizeof(len_network), 0);
        if (received <= 0)
        {
            return json(); // Return empty JSON on error or disconnect
        }
        uint32_t len = ntohl(len_network);
        std::string buffer(len, '\0');
        received = recv(client_fd, buffer.data(), len, 0);
        if (received <= 0)
        {
            return json(); // Return empty JSON on error or disconnect
        }
        std::cout << "Received message from fd=" << client_fd << ": " << buffer << std::endl;
        try
        {
            return json::parse(buffer);
        }
        catch (const json::parse_error &e)
        {
            std::cerr << "JSON parse error: " << e.what() << std::endl;
            return json(); // Return empty JSON on parse error
        }
    }
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
const std::string ConnectionManager::getUsernameFromFd(int fd)
{
    auto item = connectionMap.find(fd);
    return (item != connectionMap.end()) ? item->second : "";
}
