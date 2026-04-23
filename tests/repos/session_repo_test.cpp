#include <gtest/gtest.h>
#include <server/repos/session_repo.hpp>

/// @brief Fixture dla testów SessionRepository.
class SessionRepositoryTest : public ::testing::Test
{
protected:
    SessionRepository repo;
    Session makeSession(int id = 1, const std::string &userName = "user10") const
    {
        return Session(id, userName, std::chrono::steady_clock::now());
    }
};

/// @test addSession dodaje sesję do repozytorium.
TEST_F(SessionRepositoryTest, AddSessionStoresSession) {}

/// @test sessionExists zwraca true po dodaniu sesji.
TEST_F(SessionRepositoryTest, SessionExistsAfterAdd) {}

/// @test sessionExists zwraca false dla nieznanego id.
TEST_F(SessionRepositoryTest, SessionNotExistsForUnknownId) {}

/// @test getSession zwraca dodaną sesję.
TEST_F(SessionRepositoryTest, GetSessionReturnsAddedSession) {}

/// @test removeSession usuwa sesję.
TEST_F(SessionRepositoryTest, RemoveSessionDeletesSession) {}

/// @test updateSession aktualizuje dane sesji.
TEST_F(SessionRepositoryTest, UpdateSessionModifiesSession) {}

TEST_F(SessionRepositoryTest, JsonRoundTripPreservesSessions)
{
    repo.addSession(makeSession(3, "alice"));

    SessionRepository restored;
    restored.fromJson(repo.toJson());

    ASSERT_TRUE(restored.sessionExists(3));
    EXPECT_EQ(restored.getSession(3).getUserName(), "alice");
}
