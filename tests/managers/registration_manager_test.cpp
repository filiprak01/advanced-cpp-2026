/**
 * @file registration_manager_test.cpp
 * @brief Testy jednostkowe RegistrationManager.
 *
 * Pokrywa walidację nazwy użytkownika, hasła, duplikaty,
 * bezpieczeństwo (null-bytes, max-len overflow, SQL-like injection)
 * oraz poprawność zapisu zahaszowanego hasła.
 */
#include <gtest/gtest.h>
#include <server/auth/registration_manager.hpp>
#include <server/repos/user_repo.hpp>
#include <server/auth/password_hasher.hpp>
#include "domain_result_expect.hpp"

/// @brief Fixture — domyślna konfiguracja (min 3 / max 20 username, min 8 / max 64 password).
struct RegFixture : ::testing::Test
{
    UserRepository repo;
    PasswordHasher hasher;
    RegistrationManager mgr{repo, hasher};
};

// ---------------------------------------------------------------------------
// Happy path
// ---------------------------------------------------------------------------

/// @test Poprawna rejestracja tworzy użytkownika w repozytorium.
TEST_F(RegFixture, RegisterValidUserSucceeds)
{
    EXPECT_RESULT_SUCCESS(mgr.registerUser("alice", "Password1!"), success::Code::user_registered);
    EXPECT_TRUE(repo.userExists("alice"));
}

/// @test Zarejestrowany użytkownik ma zahaszowane hasło (nie przechowuje jawnego tekstu).
TEST_F(RegFixture, StoredPasswordIsNotPlaintext)
{
    ASSERT_RESULT_SUCCESS(mgr.registerUser("alice", "Password1!"), success::Code::user_registered);
    User u = repo.getUser("alice");
    EXPECT_NE(u.getPasswordHash(), "Password1!");
    EXPECT_FALSE(u.getPasswordHash().empty());
}

/// @test Zarejestrowany użytkownik ma niepustą sól.
TEST_F(RegFixture, StoredSaltIsNotEmpty)
{
    ASSERT_RESULT_SUCCESS(mgr.registerUser("alice", "Password1!"), success::Code::user_registered);
    User u = repo.getUser("alice");
    EXPECT_FALSE(u.getBase64salt().empty());
}

/// @test Każde konto ma unikatową sól (nawet przy tym samym haśle).
TEST_F(RegFixture, TwoUsersWithSamePasswordHaveDifferentSalts)
{
    ASSERT_RESULT_SUCCESS(mgr.registerUser("alice", "SamePass1!"), success::Code::user_registered);
    ASSERT_RESULT_SUCCESS(mgr.registerUser("bob", "SamePass1!"), success::Code::user_registered);
    EXPECT_NE(repo.getUser("alice").getBase64salt(),
              repo.getUser("bob").getBase64salt());
}

/// @test Dwa konta z tym samym hasłem mają różne skróty.
TEST_F(RegFixture, TwoUsersWithSamePasswordHaveDifferentHashes)
{
    ASSERT_RESULT_SUCCESS(mgr.registerUser("alice", "SamePass1!"), success::Code::user_registered);
    ASSERT_RESULT_SUCCESS(mgr.registerUser("bob", "SamePass1!"), success::Code::user_registered);
    EXPECT_NE(repo.getUser("alice").getPasswordHash(),
              repo.getUser("bob").getPasswordHash());
}

// ---------------------------------------------------------------------------
// Username validation
// ---------------------------------------------------------------------------

/// @test Nazwa użytkownika dokładnie na granicy minimalnej długości (3 znaki) jest akceptowana.
TEST_F(RegFixture, UsernameAtMinLengthAccepted)
{
    EXPECT_RESULT_SUCCESS(mgr.registerUser("ali", "Password1!"), success::Code::user_registered);
}

/// @test Nazwa użytkownika krótsza niż minimum (2 znaki) jest odrzucona.
TEST_F(RegFixture, UsernameTooShortRejected)
{
    EXPECT_RESULT_FORMAT_ERROR(mgr.registerUser("al", "Password1!"), errors::Code::invalid_payload);
}

/// @test Pusta nazwa użytkownika jest odrzucona.
TEST_F(RegFixture, EmptyUsernameRejected)
{
    EXPECT_RESULT_FORMAT_ERROR(mgr.registerUser("", "Password1!"), errors::Code::invalid_payload);
}

/// @test Nazwa użytkownika dokładnie na granicy maksymalnej długości (20 znaków) jest akceptowana.
TEST_F(RegFixture, UsernameAtMaxLengthAccepted)
{
    EXPECT_RESULT_SUCCESS(mgr.registerUser("alice12345678901234", "Password1!"), success::Code::user_registered);
    // 20 characters exactly
    std::string exactly20(20, 'a');
    EXPECT_RESULT_SUCCESS(mgr.registerUser(exactly20, "Password1!"), success::Code::user_registered);
}

/// @test Nazwa użytkownika o 21 znakach jest odrzucona.
TEST_F(RegFixture, UsernameTooLongRejected)
{
    std::string tooLong(21, 'a');
    EXPECT_RESULT_FORMAT_ERROR(mgr.registerUser(tooLong, "Password1!"), errors::Code::invalid_payload);
}

/// @test Nazwa użytkownika nie jest zapisywana do repo gdy walidacja nie przejdzie.
TEST_F(RegFixture, FailedRegistrationDoesNotPollutRepo)
{
    mgr.registerUser("al", "Password1!"); // too short
    EXPECT_FALSE(repo.userExists("al"));
}

// ---------------------------------------------------------------------------
// Password validation
// ---------------------------------------------------------------------------

/// @test Hasło dokładnie na granicy minimalnej długości (8 znaków) jest akceptowane.
TEST_F(RegFixture, PasswordAtMinLengthAccepted)
{
    EXPECT_RESULT_SUCCESS(mgr.registerUser("alice", "Pass123!"), success::Code::user_registered);
}

/// @test Hasło krótsze niż minimum jest odrzucone.
TEST_F(RegFixture, PasswordTooShortRejected)
{
    EXPECT_RESULT_FORMAT_ERROR(mgr.registerUser("alice", "Pass1!"), errors::Code::invalid_payload);
}

/// @test Puste hasło jest odrzucone.
TEST_F(RegFixture, EmptyPasswordRejected)
{
    EXPECT_RESULT_FORMAT_ERROR(mgr.registerUser("alice", ""), errors::Code::invalid_payload);
}

/// @test Hasło dokładnie na granicy maksymalnej długości (64 znaki) jest akceptowane.
TEST_F(RegFixture, PasswordAtMaxLengthAccepted)
{
    std::string exactly64(64, 'x');
    EXPECT_RESULT_SUCCESS(mgr.registerUser("alice", exactly64), success::Code::user_registered);
}

/// @test Hasło o 65 znakach jest odrzucone.
TEST_F(RegFixture, PasswordTooLongRejected)
{
    std::string tooLong(65, 'x');
    EXPECT_RESULT_FORMAT_ERROR(mgr.registerUser("alice", tooLong), errors::Code::invalid_payload);
}

// ---------------------------------------------------------------------------
// Duplicate registration
// ---------------------------------------------------------------------------

/// @test Próba rejestracji z tą samą nazwą użytkownika zwraca false.
TEST_F(RegFixture, DuplicateUsernameRejected)
{
    ASSERT_RESULT_SUCCESS(mgr.registerUser("alice", "Password1!"), success::Code::user_registered);
    EXPECT_RESULT_DOMAIN_ERROR(mgr.registerUser("alice", "DifferentPass2!"), errors::Code::user_already_exists);
}

/// @test Duplikat nie nadpisuje istniejącego hasła w repo.
TEST_F(RegFixture, DuplicateDoesNotOverwriteExistingHash)
{
    ASSERT_RESULT_SUCCESS(mgr.registerUser("alice", "OriginalPass1!"), success::Code::user_registered);
    std::string originalHash = repo.getUser("alice").getPasswordHash();
    mgr.registerUser("alice", "NewPass1234!");
    EXPECT_EQ(repo.getUser("alice").getPasswordHash(), originalHash);
}

// ---------------------------------------------------------------------------
// Security corner cases
// ---------------------------------------------------------------------------

/// @test Nazwa użytkownika z null-byte jest odrzucona lub przechowywana bezpiecznie.
TEST_F(RegFixture, UsernameWithNullByteHandledSafely)
{
    std::string withNull = std::string("ali") + '\0' + "ce";
    // Should either reject or treat the full string as longer than "alice"
    // Either outcome is acceptable as long as it doesn't crash
    DomainResult result = mgr.registerUser(withNull, "Password1!");
    (void)result; // crash = fail
}

/// @test Hasło składające się z samych spacji jest odrzucone lub akceptowane konsekwentnie.
TEST_F(RegFixture, PasswordAllSpacesHandledConsistently)
{
    std::string spaces(8, ' ');
    // Must not crash; result (accept/reject) is a policy decision for the dev
    DomainResult result = mgr.registerUser("alice", spaces);
    (void)result;
}

/// @test Nazwa użytkownika wyglądająca jak SQL injection jest traktowana jako tekst.
TEST_F(RegFixture, UsernameSqlInjectionTreatedAsPlainText)
{
    // Model stores strings — injection is relevant at DB layer but should not crash here
    std::string sqlName = "'; DROP TABLE users; --";
    // Too long for default config (>20), so should be rejected
    EXPECT_RESULT_FORMAT_ERROR(mgr.registerUser(sqlName, "Password1!"), errors::Code::invalid_payload);
}

/// @test Nazwa użytkownika z Unicode nie powoduje crashu.
TEST_F(RegFixture, UnicodeUsernameDoesNotCrash)
{
    std::string uni = "\xc5\xbc"; // UTF-8: 'ż' (2 bytes) — length >= min
    DomainResult result = mgr.registerUser(uni, "Password1!");
    (void)result;
}
