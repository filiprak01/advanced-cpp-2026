#include <gtest/gtest.h>
#include <server/repos/user_repo.hpp>

/// @brief Fixture dla testów UserRepository.
class UserRepositoryTest : public ::testing::Test
{
protected:
    UserRepository repo;
    User makeUser(const std::string &name = "alice") const
    {
        return User(name, "hash", "salt");
    }
};

/// @test addUser dodaje użytkownika do repozytorium.
TEST_F(UserRepositoryTest, AddUserStoresUser) {}

/// @test userExists zwraca true po dodaniu użytkownika.
TEST_F(UserRepositoryTest, UserExistsAfterAdd) {}

/// @test userExists zwraca false dla nieznanej nazwy.
TEST_F(UserRepositoryTest, UserNotExistsForUnknownName) {}

/// @test getUser zwraca dodanego użytkownika.
TEST_F(UserRepositoryTest, GetUserReturnsAddedUser) {}

/// @test removeUser usuwa użytkownika.
TEST_F(UserRepositoryTest, RemoveUserDeletesUser) {}

// / @test getAllUsers zwraca wszystkich użytkowników.
TEST_F(UserRepositoryTest, GetAllUsersReturnsAll) {}

TEST_F(UserRepositoryTest, JsonRoundTripPreservesUsers)
{
    repo.addUser(User("alice", "hash", "salt"));

    UserRepository restored;
    restored.fromJson(repo.toJson());

    ASSERT_TRUE(restored.userExists("alice"));
    EXPECT_EQ(restored.getUser("alice").getPasswordHash(), "hash");
    EXPECT_EQ(restored.getUser("alice").getBase64salt(), "salt");
}
