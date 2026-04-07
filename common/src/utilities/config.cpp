#include <common/utilities/config.hpp>
#include <common/utilities/serializer.hpp>
#include <fstream>
#include <sstream>
ServerConfig ServerConfig::readFromFile(const std::string &filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        return ServerConfig(); // Return default config if file cannot be opened
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return Serializer().deserializeServerConfig(json::parse(buffer.str()));
}
bool ServerConfig::exportToFile(const std::string &filename)
{
    std::ofstream file(filename);
    if (!file.is_open())
    {
        return false; // Failed to open file for writing
    }
    json j = Serializer().serialize(*this);
    file << j.dump(4); // Write pretty-printed JSON to file
    return true;
}
ClientConfig ClientConfig::readFromFile(const std::string &filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        return ClientConfig(); // Return default config if file cannot be opened
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return Serializer().deserializeClientConfig(json::parse(buffer.str()));
}
bool ClientConfig::exportToFile(const std::string &filename)
{
    std::ofstream file(filename);
    if (!file.is_open())
    {
        return false; // Failed to open file for writing
    }
    json j = Serializer().serialize(*this);
    file << j.dump(4); // Write pretty-printed JSON to file
    return true;
}   