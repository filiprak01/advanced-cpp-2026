#pragma once
#include <server/auth/password_hasher.hpp>
#include <server/core/domain_result.hpp>
#include <server/repos/user_repo.hpp>

/**
 * @brief Menedżer uwierzytelniania użytkowników.
 *
 * Weryfikuje poprawnść danych logowania (nazwa użytkownika + hasło)
 * za pomocą skrótów hasła przechowywanych w repozytorium.
 */
class AuthManager
{
public:
    /**
     * @brief Tworzy menedżer uwierzytelniania.
     * @param userRepository  Repozytorium użytkowników.
     * @param passwordHasher  Narzędzie do hashowania i weryfikacji haseł.
     */
    AuthManager(UserRepository &userRepository, PasswordHasher &passwordHasher)
        : userRepository(userRepository), passwordHasher(passwordHasher) {}

    /**
     * @brief Uwierzytelnia użytkownika.
     * @param username Nazwa użytkownika.
     * @param password Hasło w postaci jawnej.
     * @return @c true jeśli dane logowania są poprawne.
     */
    DomainResult authenticate(const std::string &username, const std::string &password);

private:
    UserRepository &userRepository; ///< Repozytorium użytkowników.
    PasswordHasher &passwordHasher; ///< Narzędzie do hashowania haseł.

    /**
     * @brief Weryfikuje hasło względem zapisanego skrótu.
     * @param password  Hasło w postaci jawnej.
     * @param user      Użytkownik z zapisanym skrótem i solą.
     * @return @c true jeśli hasło jest poprawne.
     */
    bool verifyPassword(const std::string password, const User &user);
};
