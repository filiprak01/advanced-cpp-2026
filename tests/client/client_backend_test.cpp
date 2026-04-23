/**
 * @file client_backend_test.cpp
 * @brief Placeholder for ClientBackend unit tests.
 *
 * Add tests for connect/disconnect, request building, queue draining,
 * response dispatch handling, and error conditions.
 */

#include <chrono>
#include <algorithm>
#include <thread>
#include <stdexcept>

#include <arpa/inet.h>
#include <gtest/gtest.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <client/client_backend.hpp>
#include <common/utilities/config.hpp>

static ClientConfig makeTestConfig()
{
    return ClientConfig(
        0x7f000001, // 127.0.0.1
        19001,
        false,
        5000,
        1000,
        true,
        100,
        true);
}

namespace
{
class ClosingServer
{
public:
    explicit ClosingServer(std::chrono::milliseconds closeDelay = std::chrono::milliseconds(50))
        : closeDelay(closeDelay)
    {
        listenFd = ::socket(AF_INET, SOCK_STREAM, 0);
        if (listenFd < 0)
            throw std::runtime_error("ClosingServer: socket() failed");

        int reuse = 1;
        ::setsockopt(listenFd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        addr.sin_port = htons(0);

        if (::bind(listenFd, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) != 0)
            throw std::runtime_error("ClosingServer: bind() failed");

        if (::listen(listenFd, 1) != 0)
            throw std::runtime_error("ClosingServer: listen() failed");

        socklen_t addrLen = sizeof(addr);
        if (::getsockname(listenFd, reinterpret_cast<sockaddr *>(&addr), &addrLen) != 0)
            throw std::runtime_error("ClosingServer: getsockname() failed");

        assignedPort = ntohs(addr.sin_port);

        worker = std::thread([this]()
                             {
            acceptedFd = ::accept(listenFd, nullptr, nullptr);
            if (acceptedFd < 0)
                return;

            std::this_thread::sleep_for(this->closeDelay);
            ::shutdown(acceptedFd, SHUT_RDWR);
            ::close(acceptedFd);
            acceptedFd = -1; });
    }

    ~ClosingServer()
    {
        if (worker.joinable())
            worker.join();

        if (acceptedFd >= 0)
        {
            ::shutdown(acceptedFd, SHUT_RDWR);
            ::close(acceptedFd);
        }

        if (listenFd >= 0)
        {
            ::shutdown(listenFd, SHUT_RDWR);
            ::close(listenFd);
        }
    }

    int port() const { return assignedPort; }

private:
    int listenFd{-1};
    int acceptedFd{-1};
    int assignedPort{0};
    std::chrono::milliseconds closeDelay;
    std::thread worker;
};

template <typename Predicate>
bool waitUntil(Predicate predicate, std::chrono::milliseconds timeout = std::chrono::milliseconds(1500))
{
    const auto deadline = std::chrono::steady_clock::now() + timeout;
    while (std::chrono::steady_clock::now() < deadline)
    {
        if (predicate())
            return true;

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    return predicate();
}
} // namespace

TEST(ClientBackendTest, LoginResponseSetsCurrentUser)
{
    ClientBackend client(makeTestConfig());
    json response = {
        {"type", "login_response"},
        {"status", "ok"},
        {"payload", {{"userName", "alice"}}}};
    client.dispatcher().dispatch(response);
    EXPECT_EQ(client.getCurrentUser(), "alice");
}

TEST(ClientBackendTest, StartsDisconnected)
{
    ClientBackend client(makeTestConfig());
    EXPECT_FALSE(client.isConnected());
    EXPECT_EQ(client.getConnectionState(), connection::ConnectionState::Disconnected);
}

TEST(ClientBackendTest, SendRawThrowsWhenDisconnected)
{
    ClientBackend client(makeTestConfig());
    EXPECT_THROW(client.sendRaw(R"({"type":"ping"})"), std::runtime_error);
    EXPECT_EQ(client.getConnectionState(), connection::ConnectionState::Disconnected);
}

TEST(ClientBackendTest, NewMessageResponseAddsMessage)
{
    ClientBackend client(makeTestConfig());
    json payload = {
        {"messageId", 123},
        {"id", 123},
        {"content", "Hello"},
        {"senderName", "alice"},
        {"timestampMs", 1000}};
    json response = {
        {"type", "new_message"},
        {"status", "ok"},
        {"payload", payload}};
    client.dispatcher().dispatch(response);
    auto &messages = client.getMessages();
    ASSERT_EQ(messages.size(), 1u);
    EXPECT_EQ(messages.at(123).getContent(), "Hello");
    EXPECT_EQ(messages.at(123).getSenderName(), "alice");
}

TEST(ClientBackendTest, MessageEditedResponseUpdatesMessage)
{
    ClientBackend client(makeTestConfig());
    Message original(5, "Old", "alice", std::chrono::steady_clock::now());
    auto &messages = const_cast<std::unordered_map<int, Message> &>(client.getMessages());
    messages[5] = original;
    json response = {
        {"type", "message_edited"},
        {"status", "ok"},
        {"payload", {{"messageId", 5}, {"newContent", "New"}}}};
    client.dispatcher().dispatch(response);
    EXPECT_EQ(client.getMessages().at(5).getContent(), "New");
}

TEST(ClientBackendTest, MessageRemovedResponseDeletesMessage)
{
    ClientBackend client(makeTestConfig());
    Message original(7, "Delete me", "bob", std::chrono::steady_clock::now());
    auto &messages = const_cast<std::unordered_map<int, Message> &>(client.getMessages());
    messages[7] = original;
    json response = {
        {"type", "message_removed"},
        {"status", "ok"},
        {"payload", {{"messageId", 7}}}};
    client.dispatcher().dispatch(response);
    EXPECT_EQ(client.getMessages().count(7), 0u);
}

TEST(ClientBackendTest, ChannelCreatedResponseAddsChannel)
{
    ClientBackend client(makeTestConfig());
    json payload = {
        {"channelId", 2},
        {"name", "general"},
        {"messageIds", json::array()},
        {"userIds", json::array({"alice"})},
        {"isPrivate", false},
        {"isActive", true},
        {"maxUsers", 32}};
    json response = {
        {"type", "channel_created"},
        {"status", "ok"},
        {"payload", payload}};
    client.dispatcher().dispatch(response);
    ASSERT_EQ(client.getChannels().size(), 1u);
    EXPECT_EQ(client.getChannels().at(2).getName(), "general");
}

TEST(ClientBackendTest, ChannelEditedResponseUpdatesChannelName)
{
    ClientBackend client(makeTestConfig());
    Channel original(2, "before", {}, {"alice"}, false);
    auto &channels = const_cast<std::unordered_map<int, Channel> &>(client.getChannels());
    channels[2] = original;
    json response = {
        {"type", "channel_edited"},
        {"status", "ok"},
        {"payload", {{"channelId", 2}, {"newName", "after"}}}};
    client.dispatcher().dispatch(response);
    EXPECT_EQ(client.getChannels().at(2).getName(), "after");
}

TEST(ClientBackendTest, ChannelRemovedResponseDeletesChannel)
{
    ClientBackend client(makeTestConfig());
    Channel original(2, "general", {}, {"alice"}, false);
    auto &channels = const_cast<std::unordered_map<int, Channel> &>(client.getChannels());
    channels[2] = original;
    json response = {
        {"type", "channel_removed"},
        {"status", "ok"},
        {"payload", {{"channelId", 2}}}};
    client.dispatcher().dispatch(response);
    EXPECT_EQ(client.getChannels().count(2), 0u);
}

TEST(ClientBackendTest, UserJoinedChannelResponseReplacesChannelState)
{
    ClientBackend client(makeTestConfig());
    Channel original(2, "general", {}, {"alice"}, false);
    auto &channels = const_cast<std::unordered_map<int, Channel> &>(client.getChannels());
    channels[2] = original;
    json payload = {
        {"channelId", 2},
        {"name", "general"},
        {"messageIds", json::array()},
        {"userIds", json::array({"alice", "bob"})},
        {"isPrivate", false},
        {"isActive", true},
        {"maxUsers", 32},
        {"userName", "bob"}};
    json response = {
        {"type", "user_joined_channel"},
        {"status", "ok"},
        {"payload", payload}};
    client.dispatcher().dispatch(response);
    EXPECT_EQ(client.getChannels().at(2).getUserIds().count("bob"), 1u);
}

TEST(ClientBackendTest, SyncResponseRebuildsState)
{
    ClientBackend client(makeTestConfig());
    json channelJson = {
        {"channelId", 2},
        {"name", "general"},
        {"messageIds", json::array()},
        {"userIds", json::array({"alice"})},
        {"isPrivate", false},
        {"isActive", true}};
    json messageJson = {
        {"id", 10},
        {"content", "sync msg"},
        {"senderName", "alice"},
        {"timestampMs", 1000}};
    json response = {
        {"type", "sync_response"},
        {"status", "ok"},
        {"payload", {{"channels", json::array({channelJson})}, {"messages", json::array({messageJson})}}}};
    client.dispatcher().dispatch(response);
    EXPECT_EQ(client.getChannels().size(), 1u);
    EXPECT_EQ(client.getMessages().size(), 1u);
}

TEST(ClientBackendTest, SyncResponseReplacesExistingStateInsteadOfMerging)
{
    ClientBackend client(makeTestConfig());

    auto &channels = const_cast<std::unordered_map<int, Channel> &>(client.getChannels());
    auto &messages = const_cast<std::unordered_map<int, Message> &>(client.getMessages());

    channels[1] = Channel(1, "stale-channel", {}, {"alice"}, false);
    messages[1] = Message(1, "stale-message", "alice", std::chrono::steady_clock::now());

    json channelJson = {
        {"channelId", 2},
        {"name", "fresh-channel"},
        {"messageIds", json::array()},
        {"userIds", json::array({"bob"})},
        {"isPrivate", false},
        {"isActive", true}};
    json messageJson = {
        {"id", 10},
        {"content", "fresh-message"},
        {"senderName", "bob"},
        {"timestampMs", 1000}};

    json response = {
        {"type", "sync_response"},
        {"status", "ok"},
        {"payload", {{"channels", json::array({channelJson})}, {"messages", json::array({messageJson})}}}};

    client.dispatcher().dispatch(response);

    EXPECT_EQ(client.getChannels().size(), 1u);
    EXPECT_EQ(client.getMessages().size(), 1u);
    EXPECT_EQ(client.getChannels().count(1), 0u);
    EXPECT_EQ(client.getMessages().count(1), 0u);
    EXPECT_EQ(client.getChannels().count(2), 1u);
    EXPECT_EQ(client.getMessages().count(10), 1u);
}

TEST(ClientBackendTest, ErrorResponseQueuesStatus)
{
    ClientBackend client(makeTestConfig());
    json response = {
        {"type", "error"},
        {"status", "error"},
        {"payload", {{"reason", "unauthorized"}}}};
    client.dispatcher().dispatch(response);
    auto status = client.drainStatus();
    ASSERT_EQ(status.size(), 1u);
    EXPECT_EQ(status[0], "unauthorized");
}

TEST(ClientBackendTest, UnexpectedDisconnectMarksConnectionLostAndQueuesStatus)
{
    ClosingServer server;
    ClientConfig cfg(
        0x7f000001,
        server.port(),
        false,
        5000,
        1000,
        true,
        100,
        true);

    ClientBackend client(cfg);
    client.connect();

    ASSERT_TRUE(waitUntil([&client]()
                          { return client.getConnectionState() == connection::ConnectionState::Connected; },
                          std::chrono::milliseconds(250)));

    EXPECT_TRUE(waitUntil([&client]()
                          { return client.getConnectionState() == connection::ConnectionState::ConnectionLost; }));

    const auto statuses = client.drainStatus();
    EXPECT_NE(std::find(statuses.begin(), statuses.end(), "connection_lost"), statuses.end());
}
