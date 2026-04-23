#pragma once
#include <common/utilities/config.hpp>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

/**
 * @brief Narzedzie do serializacji i deserializacji konfiguracji.
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
        j["userRepoFile"] = cfg.getUserRepoFile();
        j["sessionRepoFile"] = cfg.getSessionRepoFile();
        j["channelRepoFile"] = cfg.getChannelRepoFile();
        j["messageRepoFile"] = cfg.getMessageRepoFile();
        return j;
    }

    /// @brief Deserializuje JSON do ServerConfig.
    ServerConfig deserializeServerConfig(const json &j) const
    {
        ServerConfig config;
        config.fromJson(j);
        return config;
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
        ClientConfig config;
        config.fromJson(j);
        return config;
    }
};
