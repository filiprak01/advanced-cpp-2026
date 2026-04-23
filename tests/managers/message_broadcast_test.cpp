#include <gtest/gtest.h>

#include <arpa/inet.h>
#include <poll.h>
#include <sys/socket.h>
#include <unistd.h>

#include <common/models/channel.hpp>
#include <common/models/user.hpp>
#include <server/auth/auth_manager.hpp>
#include <server/auth/password_hasher.hpp>
#include <server/auth/registration_manager.hpp>
#include <server/core/channel_manager.hpp>
#include <server/core/connection_manager.hpp>
#include <server/core/events/add_message_event.hpp>
#include <server/core/events/edit_message_event.hpp>
#include <server/core/events/remove_message_event.hpp>
#include <server/core/manager_context.hpp>
#include <server/core/message_manager.hpp>
#include <server/core/session_manager.hpp>
#include <server/repos/session_repo.hpp>

#include "domain_result_expect.hpp"

namespace
{
std::optional<json> readFramedJson(int fd, int timeoutMs)
{
    pollfd descriptor{fd, POLLIN, 0};
    int pollResult = poll(&descriptor, 1, timeoutMs);
    if (pollResult <= 0)
    {
        return std::nullopt;
    }

    uint32_t lengthNetwork = 0;
    ssize_t headerBytes = recv(fd, &lengthNetwork, sizeof(lengthNetwork), MSG_WAITALL);
    if (headerBytes != static_cast<ssize_t>(sizeof(lengthNetwork)))
    {
        return std::nullopt;
    }

    uint32_t length = ntohl(lengthNetwork);
    std::string payload(length, '\0');
    ssize_t payloadBytes = recv(fd, payload.data(), length, MSG_WAITALL);
    if (payloadBytes != static_cast<ssize_t>(length))
    {
        return std::nullopt;
    }

    return json::parse(payload);
}

struct SocketClient
{
    int serverFd = -1;
    int peerFd = -1;
};
} // namespace

class MessageBroadcastFixture : public ::testing::Test
{
protected:
    UserRepository userRepo;
    ChannelRepository channelRepo;
    MessageRepository messageRepo;
    SessionRepository sessionRepo;
    PasswordHasher passwordHasher;
    ConnectionManager connectionManager{-1};
    AuthManager authManager{userRepo, passwordHasher};
    RegistrationManager registrationManager{userRepo, passwordHasher};
    SessionManager sessionManager{sessionRepo, connectionManager, std::chrono::minutes(5), {}};
    ChannelManager channelManager{channelRepo, userRepo};
    MessageManager messageManager{connectionManager, messageRepo, userRepo, channelRepo, 1, {}};
    std::unique_ptr<ManagerContext> context;

    SocketClient alice;
    SocketClient bob;
    SocketClient mallory;
    int channelId = 1;

    static SocketClient makeClientSocket()
    {
        int sockets[2] = {-1, -1};
        if (socketpair(AF_UNIX, SOCK_STREAM, 0, sockets) != 0)
        {
            return {};
        }
        return {sockets[0], sockets[1]};
    }

    void registerClient(const SocketClient &client, const std::string &userName)
    {
        ASSERT_GE(client.serverFd, 0);
        ASSERT_GE(client.peerFd, 0);
        ASSERT_TRUE(connectionManager.registerConnection(client.serverFd, userName));
    }

    void SetUp() override
    {
        alice = makeClientSocket();
        bob = makeClientSocket();
        mallory = makeClientSocket();

        ASSERT_GE(alice.serverFd, 0);
        ASSERT_GE(bob.serverFd, 0);
        ASSERT_GE(mallory.serverFd, 0);

        registerClient(alice, "alice");
        registerClient(bob, "bob");
        registerClient(mallory, "mallory");

        ASSERT_TRUE(userRepo.addUser(User{"alice", "hash", "salt"}));
        ASSERT_TRUE(userRepo.addUser(User{"bob", "hash", "salt"}));
        ASSERT_TRUE(userRepo.addUser(User{"mallory", "hash", "salt"}));

        ASSERT_TRUE(channelRepo.addChannel(Channel{channelId, "general", {}, {"alice", "bob"}, false}));
        ASSERT_TRUE(sessionManager.createSession(alice.serverFd));

        context = std::make_unique<ManagerContext>(
            authManager,
            registrationManager,
            connectionManager,
            sessionManager,
            channelManager,
            messageManager);
    }

    void TearDown() override
    {
        if (alice.serverFd >= 0)
        {
            connectionManager.closeConnection(alice.serverFd);
        }
        if (bob.serverFd >= 0)
        {
            connectionManager.closeConnection(bob.serverFd);
        }
        if (mallory.serverFd >= 0)
        {
            connectionManager.closeConnection(mallory.serverFd);
        }
        if (alice.peerFd >= 0)
        {
            close(alice.peerFd);
        }
        if (bob.peerFd >= 0)
        {
            close(bob.peerFd);
        }
        if (mallory.peerFd >= 0)
        {
            close(mallory.peerFd);
        }
    }
};

TEST_F(MessageBroadcastFixture, AddMessageBroadcastsOnlyToChannelMembersWithClientPayload)
{
    AddMessageEvent event("ignored", channelId, "hello broadcast", std::chrono::steady_clock::now());

    event.perform(*context, alice.serverFd);

    auto senderMessage = readFramedJson(alice.peerFd, 200);
    auto memberMessage = readFramedJson(bob.peerFd, 200);
    auto outsiderMessage = readFramedJson(mallory.peerFd, 50);
    auto duplicateSenderMessage = readFramedJson(alice.peerFd, 50);

    ASSERT_TRUE(senderMessage.has_value());
    ASSERT_TRUE(memberMessage.has_value());
    EXPECT_FALSE(outsiderMessage.has_value());
    EXPECT_FALSE(duplicateSenderMessage.has_value());
    EXPECT_EQ(senderMessage.value(), memberMessage.value());

    const json &payload = senderMessage->at("payload");
    EXPECT_EQ(senderMessage->at("type"), "new_message");
    EXPECT_EQ(senderMessage->at("status"), "ok");
    ASSERT_TRUE(payload.contains("messageId"));
    ASSERT_TRUE(payload.contains("id"));
    ASSERT_TRUE(payload.contains("timestampMs"));
    EXPECT_EQ(payload.at("messageId"), payload.at("id"));
    EXPECT_EQ(payload.at("channelId"), channelId);
    EXPECT_EQ(payload.at("content"), "hello broadcast");
    EXPECT_EQ(payload.at("senderName"), "alice");
}

TEST_F(MessageBroadcastFixture, EditMessageBroadcastsOnlyToChannelMembers)
{
    Message createdMessage;
    ASSERT_RESULT_SUCCESS(
        messageManager.sendMessage("before edit", alice.serverFd, channelId, std::chrono::steady_clock::now(), &createdMessage),
        success::Code::message_added);

    EditMessageEvent event(createdMessage.getId(), "after edit");
    event.perform(*context, alice.serverFd);

    auto senderMessage = readFramedJson(alice.peerFd, 200);
    auto memberMessage = readFramedJson(bob.peerFd, 200);
    auto outsiderMessage = readFramedJson(mallory.peerFd, 50);
    auto duplicateSenderMessage = readFramedJson(alice.peerFd, 50);

    ASSERT_TRUE(senderMessage.has_value());
    ASSERT_TRUE(memberMessage.has_value());
    EXPECT_FALSE(outsiderMessage.has_value());
    EXPECT_FALSE(duplicateSenderMessage.has_value());
    EXPECT_EQ(senderMessage.value(), memberMessage.value());
    EXPECT_EQ(senderMessage->at("type"), "message_edited");
    EXPECT_EQ(senderMessage->at("payload").at("messageId"), createdMessage.getId());
    EXPECT_EQ(senderMessage->at("payload").at("newContent"), "after edit");
}

TEST_F(MessageBroadcastFixture, RemoveMessageBroadcastsOnlyToChannelMembers)
{
    Message createdMessage;
    ASSERT_RESULT_SUCCESS(
        messageManager.sendMessage("to remove", alice.serverFd, channelId, std::chrono::steady_clock::now(), &createdMessage),
        success::Code::message_added);

    RemoveMessageEvent event(createdMessage.getId(), channelId);
    event.perform(*context, alice.serverFd);

    auto senderMessage = readFramedJson(alice.peerFd, 200);
    auto memberMessage = readFramedJson(bob.peerFd, 200);
    auto outsiderMessage = readFramedJson(mallory.peerFd, 50);
    auto duplicateSenderMessage = readFramedJson(alice.peerFd, 50);

    ASSERT_TRUE(senderMessage.has_value());
    ASSERT_TRUE(memberMessage.has_value());
    EXPECT_FALSE(outsiderMessage.has_value());
    EXPECT_FALSE(duplicateSenderMessage.has_value());
    EXPECT_EQ(senderMessage.value(), memberMessage.value());
    EXPECT_EQ(senderMessage->at("type"), "message_removed");
    EXPECT_EQ(senderMessage->at("payload").at("messageId"), createdMessage.getId());
    EXPECT_FALSE(messageRepo.messageExists(createdMessage.getId()));
}
