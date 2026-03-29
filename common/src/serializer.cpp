#include <common/utilities/serializer.hpp>
#include <common/messages/message.hpp>

json Serializer::serialize(const Message &message)
{
    json j;
    j["content"] = message.getContent();
    j["sender"] = message.getSender();
    j["receiver"] = message.getReceiver();
    j["timestamp"] = message.getTimestamp();
    j["channel_id"] = message.getChannelId();
    return j;
}
json Serializer::serialize(const ServerConfig &config)
{
    json j;
    j["host"] = config.getHost();
    j["port"] = config.getPort();
    j["maxConnections"] = config.getMaxConnections();
    j["sessionTimeout"] = config.getSessionTimeout();
    j["pingInterval"] = config.getPingInterval();
    j["maxInactiveTime"] = config.getMaxInactiveTime();
    j["minUsernameLength"] = config.getMinUsernameLength();
    j["maxUsernameLength"] = config.getMaxUsernameLength();
    j["minPasswordLength"] = config.getMinPasswordLength();
    j["maxPasswordLength"] = config.getMaxPasswordLength();
    j["maxChannels"] = config.getMaxChannels();
    j["maxUsersPerChannel"] = config.getMaxUsersPerChannel();
    j["maxMessageLength"] = config.getMaxMessageLength();
    j["maxMessagesPerSecond"] = config.getMaxMessagesPerSecond();
    j["maxEventQueueSize"] = config.getMaxEventQueueSize();
    j["eventProcessingThreadPoolSize"] = config.getEventProcessingThreadPoolSize();
    j["logLevel"] = config.getLogLevel();
    j["enableConsoleLogging"] = config.isConsoleLoggingEnabled();
    return j;
}
json Serializer::serialize(const ClientConfig &config)
{
    json j;
    j["serverIp"] = config.getServerIp();
    j["serverPort"] = config.getServerPort();
    j["autoReconnect"] = config.isAutoReconnectEnabled();
    j["reconnectIntervalMs"] = config.getReconnectIntervalMs();
    j["maxMessageLength"] = config.getMaxMessageLength();
    j["sendOnEnter"] = config.isSendOnEnterEnabled();
    j["messageCacheSize"] = config.getMessageCacheSize();
    j["showConnectionStatus"] = config.isShowConnectionStatusEnabled();
    return j;
}

Message Serializer::deserializeMsg(const json &j)
{
    std::string content = j.at("content").get<std::string>();
    std::string sender = j.at("sender").get<std::string>();
    std::string receiver = j.at("receiver").get<std::string>();
    std::string timestamp = j.at("timestamp").get<std::string>();
    std::string channel_id = j.at("channel_id").get<std::string>();
    return Message(content, sender, receiver, timestamp, channel_id);
}
ServerConfig Serializer::deserializeServerConfig(const json &j)
{
    std::string host = j.value("host", "localhost");
    int port = j.value("port", 8080);
    int maxConnections = j.value("maxConnections", 100);
    int sessionTimeout = j.value("sessionTimeout", 3600);
    int pingInterval = j.value("pingInterval", 60);
    int maxInactiveTime = j.value("maxInactiveTime", 300);
    int minUsernameLength = j.value("minUsernameLength", 3);
    int maxUsernameLength = j.value("maxUsernameLength", 20);
    int minPasswordLength = j.value("minPasswordLength", 6);
    int maxPasswordLength = j.value("maxPasswordLength", 64);
    int maxChannels = j.value("maxChannels", 50);
    int maxUsersPerChannel = j.value("maxUsersPerChannel", 100);
    int maxMessageLength = j.value("maxMessageLength", 1000);
    int maxMessagesPerSecond = j.value("maxMessagesPerSecond", 5);
    int maxEventQueueSize = j.value("maxEventQueueSize", 1000);
    int eventProcessingThreadPoolSize = j.value("eventProcessingThreadPoolSize", 4);
    int logLevel = j.value("logLevel", 1);
    bool enableConsoleLogging = j.value("enableConsoleLogging", true);
    return ServerConfig(host, port, maxConnections, sessionTimeout, pingInterval, maxInactiveTime,
                        minUsernameLength, maxUsernameLength, minPasswordLength, maxPasswordLength,
                        maxChannels, maxUsersPerChannel, maxMessageLength, maxMessagesPerSecond,
                        maxEventQueueSize, eventProcessingThreadPoolSize, logLevel, enableConsoleLogging);
}

ClientConfig Serializer::deserializeClientConfig(const json &j)
{
    int serverIp = j.value("serverIp", 0);
    int serverPort = j.value("serverPort", 8080);
    bool autoReconnect = j.value("autoReconnect", true);
    int reconnectIntervalMs = j.value("reconnectIntervalMs", 5000);
    int maxMessageLength = j.value("maxMessageLength", 1000);
    bool sendOnEnter = j.value("sendOnEnter", true);
    int messageCacheSize = j.value("messageCacheSize", 100);
    bool showConnectionStatus = j.value("showConnectionStatus", true);
    return ClientConfig(serverIp, serverPort, autoReconnect, reconnectIntervalMs,
                        maxMessageLength, sendOnEnter, messageCacheSize, showConnectionStatus);
}