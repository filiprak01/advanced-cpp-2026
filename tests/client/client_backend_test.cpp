/**
 * @file client_backend_test.cpp
 * @brief Placeholder for ClientBackend unit tests.
 *
 * Add tests for connect/disconnect, request building, queue draining,
 * response dispatch handling, and error conditions.
 */

#include <chrono>
#include <gtest/gtest.h>
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
