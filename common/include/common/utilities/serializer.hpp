#pragma once
#include <common/utilities/config.hpp>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

/**
 * @brief Narz\u0119dzie do serializacji i deserializacji konfiguracji.
 */
class Serializer
{
public:
    /// @brief Serializuje ServerConfig do JSON.
    json serialize(const ServerConfig &cfg) const
    {
        json j;
        j["host"] = cfg.getHost();
        j["port"] = cfg.getPort();
        j["maxConnections"] = cfg.getMaxConnections();
        j["sessionTimeout"] = cfg.getSessionTimeout();
        j["pingInterval"] = cfg.getPingInterval();
        j["maxInactiveTime"] = cfg.getMaxInactiveTime();
        j["minUsernameLength"] = cfg.getMinUsernameLength();
        j["maxUsernameLength"] = cfg.getMaxUsernameLength();
        j["minPasswordLength"] = cfg.getMinPasswordLength();
        j["maxPasswordLength"] = cfg.getMaxPasswordLength();
        j["maxChannels"] = cfg.getMaxChannels();
        j["maxUsersPerChannel"] = cfg.getMaxUsersPerChannel();
        j["maxMessageLength"] = cfg.getMaxMessageLength();
        j["maxMessagesPerSecond"] = cfg.getMaxMessagesPerSecond();
        j["maxEventQueueSize"] = cfg.getMaxEventQueueSize();
        j["eventProcessingThreadPoolSize"] = cfg.getEventProcessingThreadPoolSize();
        j["logLevel"] = cfg.getLogLevel();
        j["enableConsoleLogging"] = cfg.isConsoleLoggingEnabled();
        return j;
    }

    /// @brief Deserializuje JSON do ServerConfig.
    ServerConfig deserializeServerConfig(const json &j) const
    {
        return ServerConfig(
            j.value("host", "0.0.0.0"),
            j.value("port", 8090),
            j.value("maxConnections", 100),
            j.value("sessionTimeout", 1800),
            j.value("pingInterval", 60),
            j.value("maxInactiveTime", 300),
            j.value("minUsernameLength", 3),
            j.value("maxUsernameLength", 20),
            j.value("minPasswordLength", 6),
            j.value("maxPasswordLength", 64),
            j.value("maxChannels", 50),
            j.value("maxUsersPerChannel", 32),
            j.value("maxMessageLength", 1000),
            j.value("maxMessagesPerSecond", 5),
            j.value("maxEventQueueSize", 1000),
            j.value("eventProcessingThreadPoolSize", 4),
            j.value("logLevel", 1),
            j.value("enableConsoleLogging", true));
    }

    /// @brief Serializuje ClientConfig do JSON.
    json serialize(const ClientConfig &cfg) const
    {
        json j;
        j["serverIp"] = cfg.getServerIp();
        j["serverPort"] = cfg.getServerPort();
        j["autoReconnect"] = cfg.isAutoReconnectEnabled();
        j["reconnectIntervalMs"] = cfg.getReconnectIntervalMs();
        j["maxMessageLength"] = cfg.getMaxMessageLength();
        j["sendOnEnter"] = cfg.isSendOnEnterEnabled();
        j["messageCacheSize"] = cfg.getMessageCacheSize();
        j["showConnectionStatus"] = cfg.isShowConnectionStatusEnabled();
        return j;
    }

    /// @brief Deserializuje JSON do ClientConfig.
    ClientConfig deserializeClientConfig(const json &j) const
    {
        return ClientConfig(
            j.value("serverIp", 0),
            j.value("serverPort", 8090),
            j.value("autoReconnect", true),
            j.value("reconnectIntervalMs", 5000),
            j.value("maxMessageLength", 1000),
            j.value("sendOnEnter", true),
            j.value("messageCacheSize", 100),
            j.value("showConnectionStatus", true));
    }
};
