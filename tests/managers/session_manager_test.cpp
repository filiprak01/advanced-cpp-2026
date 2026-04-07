#include <gtest/gtest.h>
#include <server/core/session_manager.hpp>
#include <thread>

// ---------------------------------------------------------------------------
// Fixtures
// ---------------------------------------------------------------------------

/// @brief Long-timeout fixture (30 min) — sessions effectively never expire.
struct SessionFixture : ::testing::Test
{
    SessionRepository repo;
    SessionManager mgr{repo, std::chrono::milliseconds(30 * 60 * 1000), {}};
};

/// @brief Very short timeout (1 ms) — sessions expire almost immediately.
struct ShortTimeoutFixture : ::testing::Test
{
    SessionRepository repo;
    SessionManager mgr{repo, std::chrono::milliseconds(1), {}};
};

// ---------------------------------------------------------------------------
// createSession
// ---------------------------------------------------------------------------

/// @test Creating a session for a user succeeds and repo contains the session.
TEST_F(SessionFixture, CreateSessionSucceeds)
{
    EXPECT_TRUE(mgr.createSession("alice"));
    EXPECT_TRUE(repo.sessionExists(1));
}

/// @test Creating sessions for multiple different users all succeed.
TEST_F(SessionFixture, CreateSessionForMultipleUsers)
{
    EXPECT_TRUE(mgr.createSession("user10"));
    EXPECT_TRUE(mgr.createSession("user20"));
    EXPECT_TRUE(mgr.createSession("user30"));
    EXPECT_TRUE(repo.sessionExists(1));
    EXPECT_TRUE(repo.sessionExists(2));
    EXPECT_TRUE(repo.sessionExists(3));
}

/// @test Creating a second session for the same user succeeds but orphans the first session.
/// @note BUG: the first session (id 1) stays in the repo as a leaked entry; only the
///       userSessions mapping is overwritten. The old session is never cleaned up.
TEST_F(SessionFixture, CreateSessionTwiceForSameUserLeaksFirstSession)
{
    mgr.createSession("alice"); // creates session id 1
    // Second call uses nextSessionId == 2; session id 2 does not exist yet → returns true.
    EXPECT_TRUE(mgr.createSession("alice")); // creates session id 2, overwrites userSessions["alice"]
    // Session id 1 is now orphaned in the repo — never removed.
    EXPECT_TRUE(repo.sessionExists(1)); // leaked
    EXPECT_TRUE(repo.sessionExists(2)); // active
}

/// @test Session created for a user is stored with correct userName.
TEST_F(SessionFixture, CreatedSessionHasCorrectUserName)
{
    mgr.createSession("bob");
    Session s = repo.getSession(1);
    EXPECT_EQ(s.getUserName(), "bob");
}

// ---------------------------------------------------------------------------
// removeUserSession
// ---------------------------------------------------------------------------

/// @test Removing the session for a logged-in user returns true and clears repo.
TEST_F(SessionFixture, RemoveUserSessionReturnsTrueForExisting)
{
    mgr.createSession("alice");
    EXPECT_TRUE(mgr.removeUserSession("alice"));
    EXPECT_FALSE(repo.sessionExists(1));
}

/// @test Removing session for a user who never logged in returns false.
TEST_F(SessionFixture, RemoveUserSessionReturnsFalseForUnknownUser)
{
    // "unknown" has no session → userSessions["unknown"] default-inits to 0.
    // sessionExists(0) should be false → removeUserSession returns false.
    EXPECT_FALSE(mgr.removeUserSession("unknown"));
}

/// @test Double remove: second call returns false because session is already gone.
TEST_F(SessionFixture, RemoveUserSessionTwiceReturnsFalseOnSecond)
{
    mgr.createSession("charlie");
    EXPECT_TRUE(mgr.removeUserSession("charlie"));
    EXPECT_FALSE(mgr.removeUserSession("charlie"));
}

// ---------------------------------------------------------------------------
// updateUserSession
// ---------------------------------------------------------------------------

/// @test Updating an existing session returns true.
TEST_F(SessionFixture, UpdateUserSessionReturnsTrueForExisting)
{
    mgr.createSession("dave");
    EXPECT_TRUE(mgr.updateUserSession("dave"));
}

/// @test After update the session still exists in the repo.
TEST_F(SessionFixture, UpdateUserSessionKeepsSessionInRepo)
{
    mgr.createSession("dave");
    mgr.updateUserSession("dave");
    EXPECT_TRUE(repo.sessionExists(1));
}

/// @test Updating a session refreshes its lastActive timestamp.
TEST_F(SessionFixture, UpdateUserSessionRefreshesLastActive)
{
    mgr.createSession("eve");
    Session before = repo.getSession(1);
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    mgr.updateUserSession("eve");
    Session after = repo.getSession(1);
    EXPECT_GE(after.getLastActive(), before.getLastActive());
}

/// @test Updating a session for a user with no session returns false.
TEST_F(SessionFixture, UpdateUserSessionReturnsFalseForUnknownUser)
{
    EXPECT_FALSE(mgr.updateUserSession("unknown"));
}

// ---------------------------------------------------------------------------
// Session validity (tested indirectly via cleanInactiveSessions behaviour)
// ---------------------------------------------------------------------------

/// @test A fresh session is not cleaned by cleanInactiveSessions under long timeout.
TEST_F(SessionFixture, CleanInactiveSessionsDoesNotRemoveFreshSession)
{
    mgr.createSession("alice");
    mgr.cleanInactiveSessions();
    EXPECT_TRUE(repo.sessionExists(1));
}

/// @test Expired session is removed by cleanInactiveSessions.
// TEST_F(ShortTimeoutFixture, CleanInactiveSessionsRemovesExpiredSession)
// {
//     mgr.createSession("alice");
//     std::this_thread::sleep_for(std::chrono::milliseconds(20));
//     mgr.cleanInactiveSessions();
//     EXPECT_FALSE(repo.sessionExists(1));
// }

/// @test cleanInactiveSessions only removes expired sessions, leaving fresh ones intact.
TEST_F(SessionFixture, CleanInactiveSessionsLeavesValidSessionsAlone)
{
    mgr.createSession("alice");
    mgr.createSession("bob");
    mgr.cleanInactiveSessions();
    // Both sessions were created just now — neither should be removed.
    EXPECT_TRUE(repo.sessionExists(1));
    EXPECT_TRUE(repo.sessionExists(2));
}

/// @test Updating a session keeps it alive past a short timeout.
TEST_F(ShortTimeoutFixture, UpdatedSessionSurvivesCleanup)
{
    mgr.createSession("alice");
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    // Refresh before cleanup.
    mgr.updateUserSession("alice");
    // Run cleanup immediately after refresh — session should survive.
    mgr.cleanInactiveSessions();
    EXPECT_TRUE(repo.sessionExists(1));
}

// ---------------------------------------------------------------------------
// Edge cases
// ---------------------------------------------------------------------------

/// @test Empty string userName can create a session (boundary value).
TEST_F(SessionFixture, CreateSessionForEmptyUserName)
{
    EXPECT_TRUE(mgr.createSession(""));
    EXPECT_TRUE(repo.sessionExists(1));
}

/// @test Long userName works without issue.
TEST_F(SessionFixture, CreateSessionForLongUserName)
{
    EXPECT_TRUE(mgr.createSession(std::string(1000, 'x')));
    EXPECT_TRUE(repo.sessionExists(1));
}

/// @test cleanInactiveSessions returns true regardless of whether any session was removed.
TEST_F(SessionFixture, CleanInactiveSessionsAlwaysReturnsTrue)
{
    EXPECT_TRUE(mgr.cleanInactiveSessions());
    mgr.createSession("alice");
    EXPECT_TRUE(mgr.cleanInactiveSessions());
}
