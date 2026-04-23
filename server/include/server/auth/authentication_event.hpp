#pragma once
#include <server/core/event.hpp>
#include <server/core/manager_context.hpp>

/**
 * @brief Zdarzenie uwierzytelniania użytkownika.
 *
 * Wywoływane gdy klient próbuje się zalogować. Deleguje
 * weryfikację danych logowania do AuthManager.
 */
class AuthenticationEvent : public Event
{
public:
    /**
     * @brief Tworzy zdarzenie uwierzytelniania.
     * @param username Nazwa użytkownika.
     * @param password Hasło w postaci jawnej.
     */
    AuthenticationEvent(const std::string &username, const std::string &password) : username(username), password(password) {}

    /**
     * @brief Wykonuje uwierzytelnianie w podanym kontekście.
     * @param context Kontekst menedżerów serwera.
     * @param clientFd Deskryptor klienta wysyłającego zadanie.
     */
    void perform(ManagerContext &context, int clientFd) override;

private:
    std::string username; ///< Nazwa użytkownika.
    std::string password; ///< Hasło w postaci jawnej.
};
