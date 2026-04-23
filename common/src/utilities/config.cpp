#include <common/utilities/config.hpp>
#include <fstream>
#include <sstream>

json ServerConfig::toJson() const
{
    json j;
    j["host"] = host;
    j["port"] = port;
    j["maxConnections"] = maxConnections;
    j["sessionTimeout"] = sessionTimeout;
    j["pingInterval"] = pingInterval;
    j["maxInactiveTime"] = maxInactiveTime;
    j["minUsernameLength"] = minUsernameLength;
    j["maxUsernameLength"] = maxUsernameLength;
    j["minPasswordLength"] = minPasswordLength;
    j["maxPasswordLength"] = maxPasswordLength;
    j["maxChannels"] = maxChannels;
    j["maxUsersPerChannel"] = maxUsersPerChannel;
    j["maxMessageLength"] = maxMessageLength;
    j["maxMessagesPerSecond"] = maxMessagesPerSecond;
    j["maxEventQueueSize"] = maxEventQueueSize;
    j["eventProcessingThreadPoolSize"] = eventProcessingThreadPoolSize;
    j["logLevel"] = logLevel;
    j["enableConsoleLogging"] = enableConsoleLogging;
    return j;
}

void ServerConfig::fromJson(const json &j)
{
    host = j.value("host", "0.0.0.0");
    port = j.value("port", 8090);
    maxConnections = j.value("maxConnections", 100);
    sessionTimeout = j.value("sessionTimeout", 1800);
    pingInterval = j.value("pingInterval", 60);
    maxInactiveTime = j.value("maxInactiveTime", 300);
    minUsernameLength = j.value("minUsernameLength", 3);
    maxUsernameLength = j.value("maxUsernameLength", 20);
    minPasswordLength = j.value("minPasswordLength", 6);
    maxPasswordLength = j.value("maxPasswordLength", 64);
    maxChannels = j.value("maxChannels", 50);
    maxUsersPerChannel = j.value("maxUsersPerChannel", 32);
    maxMessageLength = j.value("maxMessageLength", 1000);
    maxMessagesPerSecond = j.value("maxMessagesPerSecond", 5);
    maxEventQueueSize = j.value("maxEventQueueSize", 1000);
    eventProcessingThreadPoolSize = j.value("eventProcessingThreadPoolSize", 4);
    logLevel = j.value("logLevel", 1);
    enableConsoleLogging = j.value("enableConsoleLogging", true);
}

json ClientConfig::toJson() const
{
    json j;
    j["serverIp"] = serverIp;
    j["serverPort"] = serverPort;
    j["autoReconnect"] = autoReconnect;
    j["reconnectIntervalMs"] = reconnectIntervalMs;
    j["maxMessageLength"] = maxMessageLength;
    j["sendOnEnter"] = sendOnEnter;
    j["messageCacheSize"] = messageCacheSize;
    j["showConnectionStatus"] = showConnectionStatus;
    return j;
}

void ClientConfig::fromJson(const json &j)
{
    serverIp = j.value("serverIp", 0);
    serverPort = j.value("serverPort", 8090);
    autoReconnect = j.value("autoReconnect", true);
    reconnectIntervalMs = j.value("reconnectIntervalMs", 5000);
    maxMessageLength = j.value("maxMessageLength", 1000);
    sendOnEnter = j.value("sendOnEnter", true);
    messageCacheSize = j.value("messageCacheSize", 100);
    showConnectionStatus = j.value("showConnectionStatus", true);
}
ServerConfig ServerConfig::readFromFile(const std::string &filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        return ServerConfig(); // Return default config if file cannot be opened
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    ServerConfig config;
    config.fromJson(json::parse(buffer.str()));
    return config;
}
bool ServerConfig::exportToFile(const std::string &filename)
{
    std::ofstream file(filename);
    if (!file.is_open())
    {
        return false; // Failed to open file for writing
    }
    json j = toJson();
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
    ClientConfig config;
    config.fromJson(json::parse(buffer.str()));
    return config;
}
bool ClientConfig::exportToFile(const std::string &filename)
{
    std::ofstream file(filename);
    if (!file.is_open())
    {
        return false; // Failed to open file for writing
    }
    json j = toJson();
    file << j.dump(4); // Write pretty-printed JSON to file
    return true;
}