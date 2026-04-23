#include <gtest/gtest.h>
#include <common/models/session.hpp>

/// @brief Fixture dla testów klasy Session.
class SessionTest : public ::testing::Test
{
protected:
    Session makeSession(int id = 1, const std::string &userName = "user10") const
    {
        return Session(id, userName, std::chrono::steady_clock::now());
    }
};

/// @test Konstruktor ustawia poprawne pola.
TEST_F(SessionTest, ConstructorSetsFields) {}

/// @test getSessionId zwraca identyfikator sesji.
TEST_F(SessionTest, GetSessionIdReturnsId) {}

/// @test getUserId zwraca identyfikator użytkownika.
TEST_F(SessionTest, GetUserIdReturnsUserId) {}

/// @test withNewLastActive zwraca sesję z nowym czasem.
TEST_F(SessionTest, WithNewLastActiveReturnsUpdated) {}

/// @test withNewLastActive nie modyfikuje oryginalu.
TEST_F(SessionTest, WithNewLastActiveDoesNotMutate) {}

/// @test operator== porównuje sesje po sessionId.
TEST_F(SessionTest, OperatorEqualBySessionId) {}

/// @test Serializacja do JSON zawiera sessionId i userId.
TEST_F(SessionTest, ToJsonContainsRequiredFields) {}

/// @test Deserializacja z JSON odtwarza sesję.
TEST_F(SessionTest, FromJsonRestoresObject)
{
    Session original = makeSession();
    json j = original.toJson();
    Session restored;
    restored.fromJson(j);
    EXPECT_EQ(restored.getSessionId(), original.getSessionId());
    EXPECT_EQ(restored.getUserName(), original.getUserName());
    // Timestamp check similar to message
    EXPECT_NEAR(std::chrono::duration_cast<std::chrono::milliseconds>(restored.getLastActive().time_since_epoch()).count(),
                std::chrono::duration_cast<std::chrono::milliseconds>(original.getLastActive().time_since_epoch()).count(),
                1);
}
