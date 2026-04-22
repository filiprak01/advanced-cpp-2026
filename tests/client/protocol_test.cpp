/**
 * @file protocol_test.cpp
 * @brief Testy jednostkowe funkcji budujących żądania JSON (namespace protocol).
 *
 * Sprawdza, że każda funkcja protocol::build* zwraca obiekt JSON
 * z poprawnym polem "type" i odpowiednimi polami w "payload".
 */
#include <gtest/gtest.h>
#include <client/protocol.hpp>

// ---------------------------------------------------------------------------
// Uwierzytelnianie
// ---------------------------------------------------------------------------

/// @test buildLoginRequest zwraca poprawny typ i pola ładunku.
TEST(ProtocolTest, LoginRequestHasCorrectTypeAndPayload)
{
    auto req = protocol::buildLoginRequest("alice", "secret123");
    EXPECT_EQ(req["type"], "login");
    EXPECT_EQ(req["payload"]["username"], "alice");
    EXPECT_EQ(req["payload"]["password"], "secret123");
}

/// @test buildRegisterRequest zwraca poprawny typ i pola ładunku.
TEST(ProtocolTest, RegisterRequestHasCorrectTypeAndPayload)
{
    auto req = protocol::buildRegisterRequest("bob", "pass!1");
    EXPECT_EQ(req["type"], "register");
    EXPECT_EQ(req["payload"]["username"], "bob");
    EXPECT_EQ(req["payload"]["password"], "pass!1");
}

/// @test buildLoginRequest z pustym hasłem nadal buduje poprawny JSON.
TEST(ProtocolTest, LoginRequestWithEmptyPassword)
{
    auto req = protocol::buildLoginRequest("user", "");
    EXPECT_EQ(req["type"], "login");
    EXPECT_EQ(req["payload"]["password"], "");
}

// ---------------------------------------------------------------------------
// Wiadomości
// ---------------------------------------------------------------------------

/// @test buildAddMessageRequest zawiera nadawcę, kanał i treść.
TEST(ProtocolTest, AddMessageRequestFields)
{
    auto req = protocol::buildAddMessageRequest("alice", 5, "Cześć!");
    EXPECT_EQ(req["type"], "add_message");
    EXPECT_EQ(req["payload"]["senderName"], "alice");
    EXPECT_EQ(req["payload"]["channelId"], 5);
    EXPECT_EQ(req["payload"]["content"], "Cześć!");
}

/// @test buildEditMessageRequest zawiera id wiadomości i nową treść.
TEST(ProtocolTest, EditMessageRequestFields)
{
    auto req = protocol::buildEditMessageRequest(42, "nowa treść");
    EXPECT_EQ(req["type"], "edit_message");
    EXPECT_EQ(req["payload"]["messageId"], 42);
    EXPECT_EQ(req["payload"]["newContent"], "nowa treść");
}

/// @test buildRemoveMessageRequest zawiera id wiadomości i id kanału.
TEST(ProtocolTest, RemoveMessageRequestFields)
{
    auto req = protocol::buildRemoveMessageRequest(7, 3);
    EXPECT_EQ(req["type"], "remove_message");
    EXPECT_EQ(req["payload"]["messageId"], 7);
    EXPECT_EQ(req["payload"]["channelId"], 3);
}

// ---------------------------------------------------------------------------
// Kanały
// ---------------------------------------------------------------------------

/// @test buildCreateChannelRequest zawiera nazwę, listę użytkowników i flagę prywatności.
TEST(ProtocolTest, CreateChannelRequestFields)
{
    auto req = protocol::buildCreateChannelRequest("general", {"alice", "bob"}, false);
    EXPECT_EQ(req["type"], "create_channel");
    EXPECT_EQ(req["payload"]["name"], "general");
    EXPECT_FALSE(req["payload"]["isPrivate"].get<bool>());
    ASSERT_EQ(req["payload"]["userNames"].size(), 2u);
    EXPECT_EQ(req["payload"]["userNames"][0], "alice");
    EXPECT_EQ(req["payload"]["userNames"][1], "bob");
}

/// @test buildCreateChannelRequest z prywatnym kanałem ustawia flagę isPrivate.
TEST(ProtocolTest, CreatePrivateChannelSetsFlag)
{
    auto req = protocol::buildCreateChannelRequest("private-ch", {}, true);
    EXPECT_TRUE(req["payload"]["isPrivate"].get<bool>());
}

/// @test buildEditChannelRequest zawiera id kanału i nową nazwę.
TEST(ProtocolTest, EditChannelRequestFields)
{
    auto req = protocol::buildEditChannelRequest(10, "renamed");
    EXPECT_EQ(req["type"], "edit_channel");
    EXPECT_EQ(req["payload"]["channelId"], 10);
    EXPECT_EQ(req["payload"]["newName"], "renamed");
}

/// @test buildRemoveChannelRequest zawiera id kanału.
TEST(ProtocolTest, RemoveChannelRequestFields)
{
    auto req = protocol::buildRemoveChannelRequest(99);
    EXPECT_EQ(req["type"], "remove_channel");
    EXPECT_EQ(req["payload"]["channelId"], 99);
}

/// @test buildAddUserToChannelRequest zawiera id kanału i nazwę użytkownika.
TEST(ProtocolTest, AddUserToChannelRequestFields)
{
    auto req = protocol::buildAddUserToChannelRequest(2, "charlie");
    EXPECT_EQ(req["type"], "add_user_to_channel");
    EXPECT_EQ(req["payload"]["channelId"], 2);
    EXPECT_EQ(req["payload"]["userName"], "charlie");
}

// ---------------------------------------------------------------------------
// Synchronizacja
// ---------------------------------------------------------------------------

/// @test buildSynchronizeRequest zawiera clientFd i NIE zawiera userName.
TEST(ProtocolTest, SynchronizeRequestContainsClientFdOnly)
{
    auto req = protocol::buildSynchronizeRequest(17);
    EXPECT_EQ(req["type"], "synchronize");
    EXPECT_EQ(req["payload"]["clientFd"], 17);
    EXPECT_FALSE(req["payload"].contains("userName"));
}

/// @test buildSynchronizeRequest nie ma zbędnych pól w ładunku.
TEST(ProtocolTest, SynchronizeRequestPayloadHasExactlyOneField)
{
    auto req = protocol::buildSynchronizeRequest(123);
    EXPECT_EQ(req["payload"].size(), 1u);
}
