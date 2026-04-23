#include <gtest/gtest.h>
#include <common/models/message.hpp>

/// @brief Fixture dla testów klasy Message.
class MessageTest : public ::testing::Test
{
protected:
    Message makeMessage(int id = 1) const
    {
        return Message(id, "hello", "alice", std::chrono::steady_clock::now());
    }
};

/// @test Konstruktor ustawia poprawne pola.
TEST_F(MessageTest, ConstructorSetsFields)
{
    auto now = std::chrono::steady_clock::now();
    Message msg(42, "test content", "bob", now);
    EXPECT_EQ(msg.getId(), 42);
    EXPECT_EQ(msg.getContent(), "test content");
    EXPECT_EQ(msg.getSenderName(), "bob");
    EXPECT_EQ(msg.getTimestamp(), now);
}

/// @test withContent zwraca wiadomość z nową treścią.
TEST_F(MessageTest, WithContentReturnsUpdated)
{
    Message original = makeMessage();
    Message updated = original.withContent("new content");
    EXPECT_EQ(updated.getContent(), "new content");
    EXPECT_EQ(updated.getId(), original.getId());
    EXPECT_EQ(updated.getSenderName(), original.getSenderName());
}

/// @test withContent nie modyfikuje oryginalnej wiadomości.
TEST_F(MessageTest, WithContentDoesNotMutate)
{
    Message original = makeMessage();
    std::string originalContent = original.getContent();
    Message updated = original.withContent("new content");
    EXPECT_EQ(original.getContent(), originalContent);
}

/// @test operator== porównuje wiadomości po id.
TEST_F(MessageTest, OperatorEqualById)
{
    Message msg1(1, "a", "alice", std::chrono::steady_clock::now());
    Message msg2(1, "b", "bob", std::chrono::steady_clock::now());
    Message msg3(2, "a", "alice", std::chrono::steady_clock::now());
    EXPECT_TRUE(msg1 == msg2);
    EXPECT_FALSE(msg1 == msg3);
}

/// @test Serializacja do JSON zawiera id, content i senderName.
TEST_F(MessageTest, ToJsonContainsRequiredFields)
{
    Message msg = makeMessage();
    json j = msg.toJson();
    EXPECT_TRUE(j.contains("id"));
    EXPECT_TRUE(j.contains("content"));
    EXPECT_TRUE(j.contains("senderName"));
    EXPECT_TRUE(j.contains("timestampMs"));
    EXPECT_EQ(j["id"], 1);
    EXPECT_EQ(j["content"], "hello");
    EXPECT_EQ(j["senderName"], "alice");
}

/// @test Deserializacja z JSON odtwarza oryginalny obiekt.
TEST_F(MessageTest, FromJsonRestoresObject)
{
    Message original = makeMessage();
    json j = original.toJson();
    Message restored;
    restored.fromJson(j);
    EXPECT_EQ(restored.getId(), original.getId());
    EXPECT_EQ(restored.getContent(), original.getContent());
    EXPECT_EQ(restored.getSenderName(), original.getSenderName());
    // Timestamp might not be exact due to serialization, but should be close
    EXPECT_NEAR(std::chrono::duration_cast<std::chrono::milliseconds>(restored.getTimestamp().time_since_epoch()).count(),
                std::chrono::duration_cast<std::chrono::milliseconds>(original.getTimestamp().time_since_epoch()).count(),
                1);
}
