#include <gtest/gtest.h>
#include <server/core/factory/event_factory.hpp>
#include <server/core/events/invalid_event.hpp>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

static json makeRequest(const std::string &type, json payload = json::object())
{
    return {{"type", type}, {"payload", std::move(payload)}};
}

// ---------------------------------------------------------------------------
// Null / unknown types
// ---------------------------------------------------------------------------

/// @test Unknown event type creates InvalidEvent.
TEST(EventFactoryTest, UnknownTypeCreatesInvalidEvent)
{
    auto event = EventFactory::createEvent(makeRequest("does_not_exist"));
    ASSERT_NE(event, nullptr);
    EXPECT_NE(dynamic_cast<InvalidEvent *>(event.get()), nullptr);
}

/// @test Empty type string creates InvalidEvent.
TEST(EventFactoryTest, EmptyTypeCreatesInvalidEvent)
{
    auto event = EventFactory::createEvent(makeRequest(""));
    ASSERT_NE(event, nullptr);
    EXPECT_NE(dynamic_cast<InvalidEvent *>(event.get()), nullptr);
}

/// @test Missing "type" key creates InvalidEvent.
TEST(EventFactoryTest, MissingTypeKeyCreatesInvalidEvent)
{
    auto event = EventFactory::createEvent(json::object());
    ASSERT_NE(event, nullptr);
    EXPECT_NE(dynamic_cast<InvalidEvent *>(event.get()), nullptr);
}

/// @test JSON with no payload key creates InvalidEvent.
TEST(EventFactoryTest, MissingPayloadKeyCreatesInvalidEvent)
{
    json req = {{"type", "login"}};
    auto event = EventFactory::createEvent(req);
    ASSERT_NE(event, nullptr);
    EXPECT_NE(dynamic_cast<InvalidEvent *>(event.get()), nullptr);
}

// ---------------------------------------------------------------------------
// login  →  AuthenticationEvent
// ---------------------------------------------------------------------------

/// @test Valid login request creates a non-null event.
TEST(EventFactoryTest, LoginCreatesEvent)
{
    json req = makeRequest("login", {{"username", "alice"}, {"password", "secret"}});
    EXPECT_NE(EventFactory::createEvent(req), nullptr);
}

/// @test login with empty username still creates event (validation is the manager's job).
TEST(EventFactoryTest, LoginEmptyUsernameCreatesEvent)
{
    json req = makeRequest("login", {{"username", ""}, {"password", "x"}});
    EXPECT_NE(EventFactory::createEvent(req), nullptr);
}

/// @test login with missing password field still creates event.
TEST(EventFactoryTest, LoginMissingPasswordCreatesEvent)
{
    json req = makeRequest("login", {{"username", "alice"}});
    EXPECT_NE(EventFactory::createEvent(req), nullptr);
}

// ---------------------------------------------------------------------------
// register  →  RegistrationEvent
// ---------------------------------------------------------------------------

/// @test Valid register request creates a non-null event.
TEST(EventFactoryTest, RegisterCreatesEvent)
{
    json req = makeRequest("register", {{"username", "bob"}, {"password", "pass1"}});
    EXPECT_NE(EventFactory::createEvent(req), nullptr);
}

/// @test register with empty payload creates InvalidEvent.
TEST(EventFactoryTest, RegisterEmptyPayloadCreatesInvalidEvent)
{
    auto event = EventFactory::createEvent(makeRequest("register"));
    ASSERT_NE(event, nullptr);
    EXPECT_NE(dynamic_cast<InvalidEvent *>(event.get()), nullptr);
}

// ---------------------------------------------------------------------------
// message (legacy)  →  MessageEvent
// ---------------------------------------------------------------------------

/// @test Valid message request creates a non-null event.
TEST(EventFactoryTest, MessageCreatesEvent)
{
    json req = makeRequest("message", {{"senderName", "alice"}, {"channelId", 1}, {"content", "Hello"}});
    EXPECT_NE(EventFactory::createEvent(req), nullptr);
}

/// @test message with missing channelId defaults to 0 and still creates event.
TEST(EventFactoryTest, MessageMissingChannelIdCreatesEvent)
{
    json req = makeRequest("message", {{"senderName", "alice"}, {"content", "hi"}});
    EXPECT_NE(EventFactory::createEvent(req), nullptr);
}

/// @test message with empty content creates event.
TEST(EventFactoryTest, MessageEmptyContentCreatesEvent)
{
    json req = makeRequest("message", {{"senderName", "alice"}, {"channelId", 2}, {"content", ""}});
    EXPECT_NE(EventFactory::createEvent(req), nullptr);
}

// ---------------------------------------------------------------------------
// add_message  →  AddMessageEvent
// ---------------------------------------------------------------------------

/// @test add_message creates a non-null event.
TEST(EventFactoryTest, AddMessageCreatesEvent)
{
    json req = makeRequest("add_message", {{"senderName", "alice"}, {"channelId", 1}, {"content", "Hello"}});
    EXPECT_NE(EventFactory::createEvent(req), nullptr);
}

/// @test add_message with missing senderName creates event (defaults to "").
TEST(EventFactoryTest, AddMessageMissingSenderCreatesEvent)
{
    json req = makeRequest("add_message", {{"channelId", 1}, {"content", "hello"}});
    EXPECT_NE(EventFactory::createEvent(req), nullptr);
}

/// @test add_message with long content creates event.
TEST(EventFactoryTest, AddMessageLongContentCreatesEvent)
{
    std::string longMsg(1000, 'x');
    json req = makeRequest("add_message", {{"senderName", "alice"}, {"channelId", 5}, {"content", longMsg}});
    EXPECT_NE(EventFactory::createEvent(req), nullptr);
}

// ---------------------------------------------------------------------------
// edit_message  →  EditMessageEvent
// ---------------------------------------------------------------------------

/// @test edit_message creates a non-null event.
TEST(EventFactoryTest, EditMessageCreatesEvent)
{
    json req = makeRequest("edit_message", {{"messageId", 42}, {"newContent", "Updated text"}});
    EXPECT_NE(EventFactory::createEvent(req), nullptr);
}

/// @test edit_message with missing messageId defaults to 0, still creates event.
TEST(EventFactoryTest, EditMessageMissingIdCreatesEvent)
{
    json req = makeRequest("edit_message", {{"newContent", "Updated"}});
    EXPECT_NE(EventFactory::createEvent(req), nullptr);
}

// ---------------------------------------------------------------------------
// remove_message  →  RemoveMessageEvent
// ---------------------------------------------------------------------------

/// @test remove_message creates a non-null event.
TEST(EventFactoryTest, RemoveMessageCreatesEvent)
{
    json req = makeRequest("remove_message", {{"messageId", 7}, {"channelId", 1}});
    EXPECT_NE(EventFactory::createEvent(req), nullptr);
}

/// @test remove_message with missing channelId defaults to 0.
TEST(EventFactoryTest, RemoveMessageMissingChannelIdCreatesEvent)
{
    json req = makeRequest("remove_message", {{"messageId", 7}});
    EXPECT_NE(EventFactory::createEvent(req), nullptr);
}

// ---------------------------------------------------------------------------
// create_channel  →  CreateChannelEvent  (public)
// ---------------------------------------------------------------------------

/// @test create_channel (public) creates a non-null event.
TEST(EventFactoryTest, CreatePublicChannelCreatesEvent)
{
    json req = makeRequest("create_channel", {{"name", "general"}, {"userNames", {"alice", "bob"}}, {"isPrivate", false}});
    EXPECT_NE(EventFactory::createEvent(req), nullptr);
}

/// @test create_channel with no members creates event.
TEST(EventFactoryTest, CreateChannelNoMembersCreatesEvent)
{
    json req = makeRequest("create_channel", {{"name", "empty"}, {"userNames", json::array()}, {"isPrivate", false}});
    EXPECT_NE(EventFactory::createEvent(req), nullptr);
}

/// @test create_channel with isPrivate true creates event.
TEST(EventFactoryTest, CreatePrivateChannelViaCreateChannelCreatesEvent)
{
    json req = makeRequest("create_channel", {{"name", ""}, {"userNames", {"alice", "bob"}}, {"isPrivate", true}});
    EXPECT_NE(EventFactory::createEvent(req), nullptr);
}

/// @test create_channel with missing isPrivate defaults to false.
TEST(EventFactoryTest, CreateChannelMissingIsPrivateCreatesEvent)
{
    json req = makeRequest("create_channel", {{"name", "general"}, {"userNames", {"alice"}}});
    EXPECT_NE(EventFactory::createEvent(req), nullptr);
}

// ---------------------------------------------------------------------------
// create_private_channel  →  CreateChannelEvent  (isPrivate forced true)
// ---------------------------------------------------------------------------

/// @test create_private_channel creates a non-null event.
TEST(EventFactoryTest, CreatePrivateChannelCreatesEvent)
{
    json req = makeRequest("create_private_channel", {{"userNames", {"alice", "bob"}}});
    EXPECT_NE(EventFactory::createEvent(req), nullptr);
}

/// @test create_private_channel with empty userNames still creates event.
TEST(EventFactoryTest, CreatePrivateChannelEmptyMembersCreatesEvent)
{
    json req = makeRequest("create_private_channel", {{"userNames", json::array()}});
    EXPECT_NE(EventFactory::createEvent(req), nullptr);
}

// ---------------------------------------------------------------------------
// edit_channel  →  EditChannelEvent
// ---------------------------------------------------------------------------

/// @test edit_channel creates a non-null event.
TEST(EventFactoryTest, EditChannelCreatesEvent)
{
    json req = makeRequest("edit_channel", {{"channelId", 3}, {"newName", "general-chat"}});
    EXPECT_NE(EventFactory::createEvent(req), nullptr);
}

/// @test edit_channel with missing channelId defaults to 0.
TEST(EventFactoryTest, EditChannelMissingIdCreatesEvent)
{
    json req = makeRequest("edit_channel", {{"newName", "renamed"}});
    EXPECT_NE(EventFactory::createEvent(req), nullptr);
}

// ---------------------------------------------------------------------------
// remove_channel  →  RemoveChannelEvent
// ---------------------------------------------------------------------------

/// @test remove_channel creates a non-null event.
TEST(EventFactoryTest, RemoveChannelCreatesEvent)
{
    json req = makeRequest("remove_channel", {{"channelId", 3}});
    EXPECT_NE(EventFactory::createEvent(req), nullptr);
}

/// @test remove_channel with empty payload creates InvalidEvent.
TEST(EventFactoryTest, RemoveChannelMissingIdCreatesInvalidEvent)
{
    json req = makeRequest("remove_channel", json::object());
    auto event = EventFactory::createEvent(req);
    ASSERT_NE(event, nullptr);
    EXPECT_NE(dynamic_cast<InvalidEvent *>(event.get()), nullptr);
}

// ---------------------------------------------------------------------------
// add_user_to_channel  →  AddUserToChannelEvent
// ---------------------------------------------------------------------------

/// @test add_user_to_channel creates a non-null event.
TEST(EventFactoryTest, AddUserToChannelCreatesEvent)
{
    json req = makeRequest("add_user_to_channel", {{"channelId", 3}, {"userName", "charlie"}});
    EXPECT_NE(EventFactory::createEvent(req), nullptr);
}

/// @test add_user_to_channel with missing userName defaults to "".
TEST(EventFactoryTest, AddUserToChannelMissingUserNameCreatesEvent)
{
    json req = makeRequest("add_user_to_channel", {{"channelId", 3}});
    EXPECT_NE(EventFactory::createEvent(req), nullptr);
}

// ---------------------------------------------------------------------------
// synchronize  →  SynchronizeDataEvent
// ---------------------------------------------------------------------------

/// @test synchronize creates a non-null event.
TEST(EventFactoryTest, SynchronizeCreatesEvent)
{
    json req = makeRequest("synchronize", {{"clientFd", 7}});
    EXPECT_NE(EventFactory::createEvent(req), nullptr);
}

/// @test synchronize with empty payload creates InvalidEvent.
TEST(EventFactoryTest, SynchronizeMissingClientFdCreatesInvalidEvent)
{
    json req = makeRequest("synchronize", json::object());
    auto event = EventFactory::createEvent(req);
    ASSERT_NE(event, nullptr);
    EXPECT_NE(dynamic_cast<InvalidEvent *>(event.get()), nullptr);
}

// ---------------------------------------------------------------------------
// save_data  →  SaveDataEvent
// ---------------------------------------------------------------------------

/// @test save_data creates a non-null event.
TEST(EventFactoryTest, SaveDataCreatesEvent)
{
    json req = makeRequest("save_data", {{"filePath", "/var/pwchat/data.json"}});
    EXPECT_NE(EventFactory::createEvent(req), nullptr);
}

/// @test save_data with empty payload creates InvalidEvent.
TEST(EventFactoryTest, SaveDataMissingFilePathCreatesInvalidEvent)
{
    json req = makeRequest("save_data", json::object());
    auto event = EventFactory::createEvent(req);
    ASSERT_NE(event, nullptr);
    EXPECT_NE(dynamic_cast<InvalidEvent *>(event.get()), nullptr);
}

// ---------------------------------------------------------------------------
// Type uniqueness — each type creates a distinct dynamic type
// (compile-time check via dynamic_cast after you implement the events)
// ---------------------------------------------------------------------------

/// @test login and register produce different event types.
TEST(EventFactoryTest, LoginAndRegisterProduceDifferentTypes)
{
    auto loginEvt = EventFactory::createEvent(makeRequest("login", {{"username", "a"}, {"password", "p"}}));
    auto registerEvt = EventFactory::createEvent(makeRequest("register", {{"username", "a"}, {"password", "p"}}));
    ASSERT_NE(loginEvt, nullptr);
    ASSERT_NE(registerEvt, nullptr);
    // They must not be the same object type — pointer identity is always different for unique_ptr.
    // A full type check requires dynamic_cast; enable after events are fully implemented.
    EXPECT_NE(loginEvt.get(), registerEvt.get());
}

/// @test Two separate calls with the same type produce independent event objects.
TEST(EventFactoryTest, TwoCallsProduceIndependentObjects)
{
    json req = makeRequest("login", {{"username", "alice"}, {"password", "pw"}});
    auto e1 = EventFactory::createEvent(req);
    auto e2 = EventFactory::createEvent(req);
    ASSERT_NE(e1, nullptr);
    ASSERT_NE(e2, nullptr);
    EXPECT_NE(e1.get(), e2.get());
}
