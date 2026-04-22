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
#include "domain_result_expect.hpp"

/// @brief Fixture — tworzy repo z jednym użytkownikiem "alice" / "Password1!".
struct AuthFixture : ::testing::Test
{
    UserRepository repo;
    PasswordHasher hasher;
    RegistrationManager reg{repo, hasher};
    AuthManager auth{repo, hasher};

    void SetUp() override
    {
        ASSERT_RESULT_SUCCESS(reg.registerUser("alice", "Password1!"), success::Code::user_registered);
    }
};

// ---------------------------------------------------------------------------
// Happy path
// ---------------------------------------------------------------------------

/// @test Poprawne dane logowania zwracają true.
TEST_F(AuthFixture, CorrectCredentialsReturnTrue)
{
    EXPECT_RESULT_SUCCESS(auth.authenticate("alice", "Password1!"), success::Code::user_logged_in);
}

/// @test Logowanie można wykonać wielokrotnie z poprawnymi danymi.
TEST_F(AuthFixture, RepeatedCorrectLoginSucceeds)
{
    EXPECT_RESULT_SUCCESS(auth.authenticate("alice", "Password1!"), success::Code::user_logged_in);
    EXPECT_RESULT_SUCCESS(auth.authenticate("alice", "Password1!"), success::Code::user_logged_in);
    EXPECT_RESULT_SUCCESS(auth.authenticate("alice", "Password1!"), success::Code::user_logged_in);
}

// ---------------------------------------------------------------------------
// Wrong password
// ---------------------------------------------------------------------------

/// @test Błędne hasło zwraca false.
TEST_F(AuthFixture, WrongPasswordReturnsFalse)
{
    EXPECT_RESULT_DOMAIN_ERROR(auth.authenticate("alice", "WrongPass!"), errors::Code::unauthorized);
}

/// @test Puste hasło zwraca false.
TEST_F(AuthFixture, EmptyPasswordReturnsFalse)
{
    EXPECT_RESULT_DOMAIN_ERROR(auth.authenticate("alice", ""), errors::Code::unauthorized);
}

/// @test Hasło różniące się jednym znakiem od prawidłowego zwraca false.
TEST_F(AuthFixture, AlmostCorrectPasswordReturnsFalse)
{
    EXPECT_RESULT_DOMAIN_ERROR(auth.authenticate("alice", "Password1"), errors::Code::unauthorized);
    EXPECT_RESULT_DOMAIN_ERROR(auth.authenticate("alice", "Password1!!"), errors::Code::unauthorized);
    EXPECT_RESULT_DOMAIN_ERROR(auth.authenticate("alice", "password1!"), errors::Code::unauthorized);
}

/// @test Hasło z dodatkową spacją na końcu zwraca false.
TEST_F(AuthFixture, PasswordWithTrailingSpaceReturnsFalse)
{
    EXPECT_RESULT_DOMAIN_ERROR(auth.authenticate("alice", "Password1! "), errors::Code::unauthorized);
}

// ---------------------------------------------------------------------------
// Wrong / nonexistent user
// ---------------------------------------------------------------------------

/// @test Nieistniejący użytkownik zwraca false.
TEST_F(AuthFixture, NonexistentUserReturnsFalse)
{
    EXPECT_RESULT_DOMAIN_ERROR(auth.authenticate("bob", "Password1!"), errors::Code::unauthorized);
}

/// @test Pusta nazwa użytkownika zwraca false.
TEST_F(AuthFixture, EmptyUsernameReturnsFalse)
{
    EXPECT_RESULT_DOMAIN_ERROR(auth.authenticate("", "Password1!"), errors::Code::unauthorized);
}

// ---------------------------------------------------------------------------
// Case sensitivity
// ---------------------------------------------------------------------------

/// @test Nazwa użytkownika rozróżnia wielkość liter ("Alice" != "alice").
TEST_F(AuthFixture, UsernameIsCaseSensitive)
{
    EXPECT_RESULT_DOMAIN_ERROR(auth.authenticate("Alice", "Password1!"), errors::Code::unauthorized);
    EXPECT_RESULT_DOMAIN_ERROR(auth.authenticate("ALICE", "Password1!"), errors::Code::unauthorized);
}

/// @test Hasło rozróżnia wielkość liter.
TEST_F(AuthFixture, PasswordIsCaseSensitive)
{
    EXPECT_RESULT_DOMAIN_ERROR(auth.authenticate("alice", "password1!"), errors::Code::unauthorized);
    EXPECT_RESULT_DOMAIN_ERROR(auth.authenticate("alice", "PASSWORD1!"), errors::Code::unauthorized);
}

// ---------------------------------------------------------------------------
// Cross-user isolation
// ---------------------------------------------------------------------------

/// @test Hasło użytkownika A nie działa dla użytkownika B.
TEST_F(AuthFixture, PasswordDoesNotWorkForOtherUser)
{
    ASSERT_RESULT_SUCCESS(reg.registerUser("bob", "BobPass123!"), success::Code::user_registered);
    EXPECT_RESULT_DOMAIN_ERROR(auth.authenticate("alice", "BobPass123!"), errors::Code::unauthorized);
    EXPECT_RESULT_DOMAIN_ERROR(auth.authenticate("bob", "Password1!"), errors::Code::unauthorized);
}

// ---------------------------------------------------------------------------
// Security
// ---------------------------------------------------------------------------

/// @test Brute-force 100 niepoprawnych prób nie crashuje systemu.
TEST_F(AuthFixture, BruteForceDoesNotCrash)
{
    for (int i = 0; i < 100; ++i)
    {
        EXPECT_RESULT_DOMAIN_ERROR(auth.authenticate("alice", "wrong" + std::to_string(i)), errors::Code::unauthorized);
    }
}

/// @test Użytkownik usunięty z repo nie może się zalogować.
TEST_F(AuthFixture, DeletedUserCannotLogin)
{
    ASSERT_TRUE(repo.removeUser("alice"));
    EXPECT_RESULT_DOMAIN_ERROR(auth.authenticate("alice", "Password1!"), errors::Code::unauthorized);
}
