#include <gtest/gtest.h>
#include <server/core/session_manager.hpp>
#include <thread>

namespace
{
constexpr int kAliceFd = 101;
constexpr int kBobFd = 102;
constexpr int kCharlieFd = 103;
constexpr int kDaveFd = 104;
constexpr int kEveFd = 105;
constexpr int kUnknownFd = 999;
} // namespace

struct SessionFixture : ::testing::Test
{
    SessionRepository repo;
    ConnectionManager connectionManager{-1};
    SessionManager mgr{repo, connectionManager, std::chrono::milliseconds(30 * 60 * 1000), {}};

    void bindUser(int fd, const std::string &userName)
    {
        ASSERT_TRUE(connectionManager.registerConnection(fd, userName));
    }
};

struct ShortTimeoutFixture : ::testing::Test
{
    SessionRepository repo;
    ConnectionManager connectionManager{-1};
    SessionManager mgr{repo, connectionManager, std::chrono::milliseconds(20), {}};

    void bindUser(int fd, const std::string &userName)
    {
        ASSERT_TRUE(connectionManager.registerConnection(fd, userName));
    }
};

TEST_F(SessionFixture, CreateSessionSucceeds)
{
    bindUser(kAliceFd, "alice");
    EXPECT_TRUE(mgr.createSession(kAliceFd));
    EXPECT_TRUE(repo.sessionExists(1));
    EXPECT_TRUE(mgr.hasSession(kAliceFd));
}

TEST_F(SessionFixture, CreateSessionForMultipleUsers)
{
    bindUser(kAliceFd, "user10");
    bindUser(kBobFd, "user20");
    bindUser(kCharlieFd, "user30");

    EXPECT_TRUE(mgr.createSession(kAliceFd));
    EXPECT_TRUE(mgr.createSession(kBobFd));
    EXPECT_TRUE(mgr.createSession(kCharlieFd));
    EXPECT_TRUE(repo.sessionExists(1));
    EXPECT_TRUE(repo.sessionExists(2));
    EXPECT_TRUE(repo.sessionExists(3));
}

TEST_F(SessionFixture, CreateSessionReturnsFalseForUnknownFd)
{
    EXPECT_FALSE(mgr.createSession(kUnknownFd));
    EXPECT_FALSE(repo.sessionExists(1));
}

TEST_F(SessionFixture, CreateSessionTwiceForSameUserReplacesOldRepositoryEntry)
{
    bindUser(kAliceFd, "alice");

    EXPECT_TRUE(mgr.createSession(kAliceFd));
    EXPECT_TRUE(mgr.createSession(kAliceFd));
    EXPECT_FALSE(repo.sessionExists(1));
    EXPECT_TRUE(repo.sessionExists(2));
    EXPECT_TRUE(mgr.hasSession(kAliceFd));
}

TEST_F(SessionFixture, CreatedSessionHasCorrectUserName)
{
    bindUser(kBobFd, "bob");

    ASSERT_TRUE(mgr.createSession(kBobFd));
    Session session = repo.getSession(1);
    EXPECT_EQ(session.getUserName(), "bob");
}

TEST_F(SessionFixture, RemoveUserSessionReturnsTrueForExisting)
{
    bindUser(kAliceFd, "alice");

    ASSERT_TRUE(mgr.createSession(kAliceFd));
    EXPECT_TRUE(mgr.removeUserSession("alice"));
    EXPECT_FALSE(repo.sessionExists(1));
}

TEST_F(SessionFixture, RemoveUserSessionReturnsTrueForUnknownUser)
{
    EXPECT_TRUE(mgr.removeUserSession("unknown"));
}

TEST_F(SessionFixture, RemoveUserSessionTwiceReturnsTrueOnSecondCall)
{
    bindUser(kCharlieFd, "charlie");

    ASSERT_TRUE(mgr.createSession(kCharlieFd));
    EXPECT_TRUE(mgr.removeUserSession("charlie"));
    EXPECT_TRUE(mgr.removeUserSession("charlie"));
}

TEST_F(SessionFixture, UpdateUserSessionReturnsTrueForExisting)
{
    bindUser(kDaveFd, "dave");

    ASSERT_TRUE(mgr.createSession(kDaveFd));
    EXPECT_TRUE(mgr.updateUserSession(kDaveFd));
}

TEST_F(SessionFixture, UpdateUserSessionKeepsSessionInRepo)
{
    bindUser(kDaveFd, "dave");

    ASSERT_TRUE(mgr.createSession(kDaveFd));
    ASSERT_TRUE(mgr.updateUserSession(kDaveFd));
    EXPECT_TRUE(repo.sessionExists(1));
}

TEST_F(SessionFixture, UpdateUserSessionRefreshesLastActive)
{
    bindUser(kEveFd, "eve");

    ASSERT_TRUE(mgr.createSession(kEveFd));
    Session before = repo.getSession(1);
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    ASSERT_TRUE(mgr.updateUserSession(kEveFd));
    Session after = repo.getSession(1);
    EXPECT_GE(after.getLastActive(), before.getLastActive());
}

TEST_F(SessionFixture, UpdateUserSessionReturnsFalseForUnknownFd)
{
    EXPECT_FALSE(mgr.updateUserSession(kUnknownFd));
}

TEST_F(SessionFixture, CleanInactiveSessionsDoesNotRemoveFreshSession)
{
    bindUser(kAliceFd, "alice");

    ASSERT_TRUE(mgr.createSession(kAliceFd));
    EXPECT_TRUE(mgr.cleanInactiveSessions());
    EXPECT_TRUE(repo.sessionExists(1));
}

TEST_F(ShortTimeoutFixture, CleanInactiveSessionsRemovesExpiredSession)
{
    bindUser(kAliceFd, "alice");

    ASSERT_TRUE(mgr.createSession(kAliceFd));
    std::this_thread::sleep_for(std::chrono::milliseconds(40));
    EXPECT_TRUE(mgr.cleanInactiveSessions());
    EXPECT_FALSE(repo.sessionExists(1));
    EXPECT_FALSE(mgr.hasSession(kAliceFd));
}

TEST_F(SessionFixture, CleanInactiveSessionsLeavesValidSessionsAlone)
{
    bindUser(kAliceFd, "alice");
    bindUser(kBobFd, "bob");

    ASSERT_TRUE(mgr.createSession(kAliceFd));
    ASSERT_TRUE(mgr.createSession(kBobFd));
    EXPECT_TRUE(mgr.cleanInactiveSessions());
    EXPECT_TRUE(repo.sessionExists(1));
    EXPECT_TRUE(repo.sessionExists(2));
}

TEST_F(ShortTimeoutFixture, UpdatedSessionSurvivesCleanup)
{
    bindUser(kAliceFd, "alice");

    ASSERT_TRUE(mgr.createSession(kAliceFd));
    std::this_thread::sleep_for(std::chrono::milliseconds(40));
    ASSERT_TRUE(mgr.updateUserSession(kAliceFd));
    EXPECT_TRUE(mgr.cleanInactiveSessions());
    EXPECT_TRUE(repo.sessionExists(1));
}

TEST_F(SessionFixture, CreateSessionForEmptyUserNameReturnsFalse)
{
    bindUser(kAliceFd, "");

    EXPECT_FALSE(mgr.createSession(kAliceFd));
    EXPECT_FALSE(repo.sessionExists(1));
}

TEST_F(SessionFixture, CreateSessionForLongUserName)
{
    bindUser(kAliceFd, std::string(1000, 'x'));

    EXPECT_TRUE(mgr.createSession(kAliceFd));
    EXPECT_TRUE(repo.sessionExists(1));
}

TEST_F(SessionFixture, CleanInactiveSessionsAlwaysReturnsTrue)
{
    EXPECT_TRUE(mgr.cleanInactiveSessions());

    bindUser(kAliceFd, "alice");
    ASSERT_TRUE(mgr.createSession(kAliceFd));
    EXPECT_TRUE(mgr.cleanInactiveSessions());
}
