#pragma once
#include <string>

class ServerConfig
{
public:
    ServerConfig() = default;
    ServerConfig(const std::string &host, int port, int maxConnections,
                 int sessionTimeout, int pingInterval, int maxInactiveTime,
                 int minUsernameLength, int maxUsernameLength,
                 int minPasswordLength, int maxPasswordLength,
                 int maxChannels, int maxUsersPerChannel,
                 int maxMessageLength, int maxMessagesPerSecond,
                 int maxEventQueueSize, int eventProcessingThreadPoolSize,
                 int logLevel, bool enableConsoleLogging)
        : host(host), port(port), maxConnections(maxConnections),
          sessionTimeout(sessionTimeout), pingInterval(pingInterval), maxInactiveTime(maxInactiveTime),
          minUsernameLength(minUsernameLength), maxUsernameLength(maxUsernameLength),
          minPasswordLength(minPasswordLength), maxPasswordLength(maxPasswordLength),
          maxChannels(maxChannels), maxUsersPerChannel(maxUsersPerChannel),
          maxMessageLength(maxMessageLength), maxMessagesPerSecond(maxMessagesPerSecond),
          maxEventQueueSize(maxEventQueueSize), eventProcessingThreadPoolSize(eventProcessingThreadPoolSize),
          logLevel(logLevel), enableConsoleLogging(enableConsoleLogging) {}
    ServerConfig readFromFile(const std::string &filename);
    bool exportToFile(const std::string &filename); // Placeholder for reading config from a file
    std::string getHost() const
    {
        return host;
    }
    int getPort() const { return port; }
    int getMaxConnections() const { return maxConnections; }
    int getSessionTimeout() const { return sessionTimeout; }
    int getPingInterval() const { return pingInterval; }
    int getMaxInactiveTime() const { return maxInactiveTime; }
    int getMinUsernameLength() const { return minUsernameLength; }
    int getMaxUsernameLength() const { return maxUsernameLength; }
    int getMinPasswordLength() const { return minPasswordLength; }
    int getMaxPasswordLength() const { return maxPasswordLength; }
    int getMaxChannels() const { return maxChannels; }
    int getMaxUsersPerChannel() const { return maxUsersPerChannel; }
    int getMaxMessageLength() const { return maxMessageLength; }
    int getMaxMessagesPerSecond() const { return maxMessagesPerSecond; }
    int getMaxEventQueueSize() const { return maxEventQueueSize; }
    int getEventProcessingThreadPoolSize() const { return eventProcessingThreadPoolSize; }
    int getLogLevel() const { return logLevel; }
    bool isConsoleLoggingEnabled() const { return enableConsoleLogging; }

private:
    // readonly server configuration parameters
    std::string host;
    int port;
    int maxConnections;

    int sessionTimeout;  // in seconds
    int pingInterval;    // in seconds
    int maxInactiveTime; // in seconds

    int minUsernameLength;
    int maxUsernameLength;
    int minPasswordLength;
    int maxPasswordLength;

    int maxChannels;
    int maxUsersPerChannel;

    int maxMessageLength;
    int maxMessagesPerSecond;

    int maxEventQueueSize;
    int eventProcessingThreadPoolSize;

    int logLevel;              // 0 = ERROR, 1 = WARN, 2 = INFO, 3 = DEBUG
    bool enableConsoleLogging; // 0 = false, 1 = true
};

class ClientConfig
{
public:
    ClientConfig() = default;
    ClientConfig(int serverIp, int serverPort, bool autoReconnect,
                 int reconnectIntervalMs, int maxMessageLength,
                 bool sendOnEnter, int messageCacheSize, bool showConnectionStatus)
        : serverIp(serverIp), serverPort(serverPort), autoReconnect(autoReconnect),
          reconnectIntervalMs(reconnectIntervalMs), maxMessageLength(maxMessageLength),
          sendOnEnter(sendOnEnter), messageCacheSize(messageCacheSize),
          showConnectionStatus(showConnectionStatus) {}
    ClientConfig readFromFile(const std::string &filename);
    bool exportToFile(const std::string &filename); // Placeholder for reading config from a file
    int getServerIp() const { return serverIp; }
    int getServerPort() const { return serverPort; }
    bool isAutoReconnectEnabled() const { return autoReconnect; }
    int getReconnectIntervalMs() const { return reconnectIntervalMs; }
    int getMaxMessageLength() const { return maxMessageLength; }
    bool isSendOnEnterEnabled() const { return sendOnEnter; }
    int getMessageCacheSize() const { return messageCacheSize; }
    bool isShowConnectionStatusEnabled() const { return showConnectionStatus; }

private:
    // readonly client configuration parameters
    int serverIp;
    int serverPort;
    bool autoReconnect;      // 0 = false, 1 = true
    int reconnectIntervalMs; // in milliseconds
    int maxMessageLength;
    bool sendOnEnter;          // 0 = false, 1 = true
    int messageCacheSize;      // Number of messages to cache locally
    bool showConnectionStatus; // 0 = false, 1 = true
};