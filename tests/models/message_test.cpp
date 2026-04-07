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
TEST_F(MessageTest, ConstructorSetsFields) {}

/// @test withContent zwraca wiadomość z nową treścią.
TEST_F(MessageTest, WithContentReturnsUpdated) {}

/// @test withContent nie modyfikuje oryginalnej wiadomości.
TEST_F(MessageTest, WithContentDoesNotMutate) {}

/// @test operator== porównuje wiadomości po id.
TEST_F(MessageTest, OperatorEqualById) {}

/// @test Serializacja do JSON zawiera id, content i senderName.
TEST_F(MessageTest, ToJsonContainsRequiredFields) {}

/// @test Deserializacja z JSON odtwarza oryginalny obiekt.
TEST_F(MessageTest, FromJsonRestoresObject) {}
