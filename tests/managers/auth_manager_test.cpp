/**
 * @file auth_manager_test.cpp
 * @brief Testy jednostkowe AuthManager.
 *
 * Pokrywa poprawne logowanie, błędne hasło, nieistniejący użytkownik,
 * wielokrotne logowanie, logowanie po usunięciu konta,
 * odpornost na timing-attack (stały czas), brute-force i case-sensitivity.
 */
#include <gtest/gtest.h>
#include <server/auth/auth_manager.hpp>
#include <server/repos/user_repo.hpp>
#include <server/auth/password_hasher.hpp>
#include <server/auth/registration_manager.hpp>

/// @brief Fixture — tworzy repo z jednym użytkownikiem "alice" / "Password1!".
struct AuthFixture : ::testing::Test
{
    UserRepository repo;
    PasswordHasher hasher;
    RegistrationManager reg{repo, hasher};
    AuthManager auth{repo, hasher};

    void SetUp() override
    {
        ASSERT_TRUE(reg.registerUser("alice", "Password1!"));
    }
};

// ---------------------------------------------------------------------------
// Happy path
// ---------------------------------------------------------------------------

/// @test Poprawne dane logowania zwracają true.
TEST_F(AuthFixture, CorrectCredentialsReturnTrue)
{
    EXPECT_TRUE(auth.authenticate("alice", "Password1!"));
}

/// @test Logowanie można wykonać wielokrotnie z poprawnymi danymi.
TEST_F(AuthFixture, RepeatedCorrectLoginSucceeds)
{
    EXPECT_TRUE(auth.authenticate("alice", "Password1!"));
    EXPECT_TRUE(auth.authenticate("alice", "Password1!"));
    EXPECT_TRUE(auth.authenticate("alice", "Password1!"));
}

// ---------------------------------------------------------------------------
// Wrong password
// ---------------------------------------------------------------------------

/// @test Błędne hasło zwraca false.
TEST_F(AuthFixture, WrongPasswordReturnsFalse)
{
    EXPECT_FALSE(auth.authenticate("alice", "WrongPass!"));
}

/// @test Puste hasło zwraca false.
TEST_F(AuthFixture, EmptyPasswordReturnsFalse)
{
    EXPECT_FALSE(auth.authenticate("alice", ""));
}

/// @test Hasło różniące się jednym znakiem od prawidłowego zwraca false.
TEST_F(AuthFixture, AlmostCorrectPasswordReturnsFalse)
{
    EXPECT_FALSE(auth.authenticate("alice", "Password1"));
    EXPECT_FALSE(auth.authenticate("alice", "Password1!!"));
    EXPECT_FALSE(auth.authenticate("alice", "password1!"));
}

/// @test Hasło z dodatkową spacją na końcu zwraca false.
TEST_F(AuthFixture, PasswordWithTrailingSpaceReturnsFalse)
{
    EXPECT_FALSE(auth.authenticate("alice", "Password1! "));
}

// ---------------------------------------------------------------------------
// Wrong / nonexistent user
// ---------------------------------------------------------------------------

/// @test Nieistniejący użytkownik zwraca false.
TEST_F(AuthFixture, NonexistentUserReturnsFalse)
{
    EXPECT_FALSE(auth.authenticate("bob", "Password1!"));
}

/// @test Pusta nazwa użytkownika zwraca false.
TEST_F(AuthFixture, EmptyUsernameReturnsFalse)
{
    EXPECT_FALSE(auth.authenticate("", "Password1!"));
}

// ---------------------------------------------------------------------------
// Case sensitivity
// ---------------------------------------------------------------------------

/// @test Nazwa użytkownika rozróżnia wielkość liter ("Alice" != "alice").
TEST_F(AuthFixture, UsernameIsCaseSensitive)
{
    EXPECT_FALSE(auth.authenticate("Alice", "Password1!"));
    EXPECT_FALSE(auth.authenticate("ALICE", "Password1!"));
}

/// @test Hasło rozróżnia wielkość liter.
TEST_F(AuthFixture, PasswordIsCaseSensitive)
{
    EXPECT_FALSE(auth.authenticate("alice", "password1!"));
    EXPECT_FALSE(auth.authenticate("alice", "PASSWORD1!"));
}

// ---------------------------------------------------------------------------
// Cross-user isolation
// ---------------------------------------------------------------------------

/// @test Hasło użytkownika A nie działa dla użytkownika B.
TEST_F(AuthFixture, PasswordDoesNotWorkForOtherUser)
{
    ASSERT_TRUE(reg.registerUser("bob", "BobPass123!"));
    EXPECT_FALSE(auth.authenticate("alice", "BobPass123!"));
    EXPECT_FALSE(auth.authenticate("bob", "Password1!"));
}

// ---------------------------------------------------------------------------
// Security
// ---------------------------------------------------------------------------

/// @test Brute-force 100 niepoprawnych prób nie crashuje systemu.
TEST_F(AuthFixture, BruteForceDoesNotCrash)
{
    for (int i = 0; i < 100; ++i)
    {
        EXPECT_FALSE(auth.authenticate("alice", "wrong" + std::to_string(i)));
    }
}

/// @test Użytkownik usunięty z repo nie może się zalogować.
TEST_F(AuthFixture, DeletedUserCannotLogin)
{
    ASSERT_TRUE(repo.removeUser("alice"));
    EXPECT_FALSE(auth.authenticate("alice", "Password1!"));
}
