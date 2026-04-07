#pragma once
#include <server/auth/password_hasher.hpp>
#include <server/repos/user_repo.hpp>

/**
 * @brief Menedżer rejestracji nowych użytkowników.
 *
 * Waliduje nazwę użytkownika i hasło według skonfigurowanych reguł,
 * a następnie tworzy konto użytkownika z zahaszowanym hasłem.
 */
class RegistrationManager
{
public:
    /**
     * @brief Tworzy menedżer rejestracji.
     * @param userRepo          Repozytorium użytkowników.
     * @param passwordHasher    Narzędzie do hashowania haseł.
     * @param passwordMinLength Minimalna długość hasła (domyślnie 8).
     * @param passwordMaxLength Maksymalna długość hasła (domyślnie 64).
     * @param usernameMinLength Minimalna długość nazwy użytkownika (domyślnie 3).
     * @param usernameMaxLength Maksymalna długość nazwy użytkownika (domyślnie 20).
     */
    RegistrationManager(UserRepository &userRepo, PasswordHasher &passwordHasher,
                        int passwordMinLength = 8, int passwordMaxLength = 64,
                        int usernameMinLength = 3, int usernameMaxLength = 20);

    /**
     * @brief Rejestruje nowego użytkownika.
     * @param username Nazwa użytkownika.
     * @param password Hasło w postaci jawnej.
     * @return @c true jeśli rejestracja się powiódła.
     */
    bool registerUser(const std::string &username, const std::string &password);

private:
    int passwordMinLength;          ///< Minimalna długość hasła.
    int passwordMaxLength;          ///< Maksymalna długość hasła.
    int usernameMinLength;          ///< Minimalna długość nazwy użytkownika.
    int usernameMaxLength;          ///< Maksymalna długość nazwy użytkownika.
    UserRepository &userRepo;       ///< Repozytorium użytkowników.
    PasswordHasher &passwordHasher; ///< Narzędzie do hashowania haseł.

    /**
     * @brief Waliduje hasło według reguł długości.
     * @param password Hasło do walidacji.
     * @return @c true jeśli hasło spełnia wymagania.
     */
    bool validatePassword(const std::string &password) const;

    /**
     * @brief Waliduje nazwę użytkownika według reguł długości.
     * @param username Nazwa użytkownika do walidacji.
     * @return @c true jeśli nazwa spełnia wymagania.
     */
    bool validateUsername(const std::string &username) const;
};