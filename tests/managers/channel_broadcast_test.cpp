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
#include <server/core/events/add_user_to_channel_event.hpp>
#include <server/core/events/create_channel_event.hpp>
#include <server/core/events/edit_channel_event.hpp>
#include <server/core/events/remove_channel_event.hpp>
#include <server/core/manager_context.hpp>
#include <server/core/message_manager.hpp>
#include <server/core/session_manager.hpp>
#include <server/repos/session_repo.hpp>

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

class ChannelBroadcastFixture : public ::testing::Test
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
    SocketClient oscar;
    int existingChannelId = 10;

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
        oscar = makeClientSocket();

        registerClient(alice, "alice");
        registerClient(bob, "bob");
        registerClient(mallory, "mallory");
        registerClient(oscar, "oscar");

        ASSERT_TRUE(userRepo.addUser(User{"alice", "hash", "salt"}));
        ASSERT_TRUE(userRepo.addUser(User{"bob", "hash", "salt"}));
        ASSERT_TRUE(userRepo.addUser(User{"mallory", "hash", "salt"}));
        ASSERT_TRUE(userRepo.addUser(User{"oscar", "hash", "salt"}));

        ASSERT_TRUE(channelRepo.addChannel(Channel{existingChannelId, "general", {}, {"alice", "bob"}, false}));
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
        for (int serverFd : {alice.serverFd, bob.serverFd, mallory.serverFd, oscar.serverFd})
        {
            if (serverFd >= 0)
            {
                connectionManager.closeConnection(serverFd);
            }
        }
        for (int peerFd : {alice.peerFd, bob.peerFd, mallory.peerFd, oscar.peerFd})
        {
            if (peerFd >= 0)
            {
                close(peerFd);
            }
        }
    }
};

TEST_F(ChannelBroadcastFixture, CreateChannelBroadcastsOnlyToCreatedMembersWithFullPayload)
{
    CreateChannelEvent event("team", {"bob", "mallory"}, false);

    event.perform(*context, alice.serverFd);

    auto senderMessage = readFramedJson(alice.peerFd, 200);
    auto bobMessage = readFramedJson(bob.peerFd, 200);
    auto malloryMessage = readFramedJson(mallory.peerFd, 200);
    auto outsiderMessage = readFramedJson(oscar.peerFd, 50);
    auto duplicateSenderMessage = readFramedJson(alice.peerFd, 50);

    ASSERT_TRUE(senderMessage.has_value());
    ASSERT_TRUE(bobMessage.has_value());
    ASSERT_TRUE(malloryMessage.has_value());
    EXPECT_FALSE(outsiderMessage.has_value());
    EXPECT_FALSE(duplicateSenderMessage.has_value());
    EXPECT_EQ(senderMessage.value(), bobMessage.value());
    EXPECT_EQ(senderMessage.value(), malloryMessage.value());

    const json &payload = senderMessage->at("payload");
    EXPECT_EQ(senderMessage->at("type"), "channel_created");
    EXPECT_EQ(payload.at("name"), "team");
    EXPECT_EQ(payload.at("isPrivate"), false);
    EXPECT_EQ(payload.at("isActive"), true);
    ASSERT_TRUE(payload.contains("channelId"));
    ASSERT_TRUE(payload.contains("userIds"));
    ASSERT_TRUE(payload.contains("messageIds"));
}

TEST_F(ChannelBroadcastFixture, EditChannelBroadcastsOnlyToCurrentMembers)
{
    EditChannelEvent event(existingChannelId, "renamed");

    event.perform(*context, alice.serverFd);

    auto senderMessage = readFramedJson(alice.peerFd, 200);
    auto memberMessage = readFramedJson(bob.peerFd, 200);
    auto outsiderOne = readFramedJson(mallory.peerFd, 50);
    auto outsiderTwo = readFramedJson(oscar.peerFd, 50);
    auto duplicateSenderMessage = readFramedJson(alice.peerFd, 50);

    ASSERT_TRUE(senderMessage.has_value());
    ASSERT_TRUE(memberMessage.has_value());
    EXPECT_FALSE(outsiderOne.has_value());
    EXPECT_FALSE(outsiderTwo.has_value());
    EXPECT_FALSE(duplicateSenderMessage.has_value());
    EXPECT_EQ(senderMessage.value(), memberMessage.value());
    EXPECT_EQ(senderMessage->at("type"), "channel_edited");
    EXPECT_EQ(senderMessage->at("payload").at("channelId"), existingChannelId);
    EXPECT_EQ(senderMessage->at("payload").at("newName"), "renamed");
}

TEST_F(ChannelBroadcastFixture, RemoveChannelBroadcastsOnlyToFormerMembers)
{
    RemoveChannelEvent event(existingChannelId);

    event.perform(*context, alice.serverFd);

    auto senderMessage = readFramedJson(alice.peerFd, 200);
    auto memberMessage = readFramedJson(bob.peerFd, 200);
    auto outsiderOne = readFramedJson(mallory.peerFd, 50);
    auto outsiderTwo = readFramedJson(oscar.peerFd, 50);
    auto duplicateSenderMessage = readFramedJson(alice.peerFd, 50);

    ASSERT_TRUE(senderMessage.has_value());
    ASSERT_TRUE(memberMessage.has_value());
    EXPECT_FALSE(outsiderOne.has_value());
    EXPECT_FALSE(outsiderTwo.has_value());
    EXPECT_FALSE(duplicateSenderMessage.has_value());
    EXPECT_EQ(senderMessage.value(), memberMessage.value());
    EXPECT_EQ(senderMessage->at("type"), "channel_removed");
    EXPECT_EQ(senderMessage->at("payload").at("channelId"), existingChannelId);
    EXPECT_FALSE(channelRepo.channelExists(existingChannelId));
}

TEST_F(ChannelBroadcastFixture, AddUserToChannelBroadcastsUpdatedChannelToAllMembers)
{
    AddUserToChannelEvent event(existingChannelId, "mallory");

    event.perform(*context, alice.serverFd);

    auto senderMessage = readFramedJson(alice.peerFd, 200);
    auto bobMessage = readFramedJson(bob.peerFd, 200);
    auto malloryMessage = readFramedJson(mallory.peerFd, 200);
    auto outsiderMessage = readFramedJson(oscar.peerFd, 50);
    auto duplicateSenderMessage = readFramedJson(alice.peerFd, 50);

    ASSERT_TRUE(senderMessage.has_value());
    ASSERT_TRUE(bobMessage.has_value());
    ASSERT_TRUE(malloryMessage.has_value());
    EXPECT_FALSE(outsiderMessage.has_value());
    EXPECT_FALSE(duplicateSenderMessage.has_value());
    EXPECT_EQ(senderMessage.value(), bobMessage.value());
    EXPECT_EQ(senderMessage.value(), malloryMessage.value());
    EXPECT_EQ(senderMessage->at("type"), "user_joined_channel");
    EXPECT_EQ(senderMessage->at("payload").at("channelId"), existingChannelId);
    EXPECT_EQ(senderMessage->at("payload").at("userName"), "mallory");
    ASSERT_TRUE(senderMessage->at("payload").contains("userIds"));
}
