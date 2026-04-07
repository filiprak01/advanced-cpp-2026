#pragma once
#include <string>
#include <server/core/event.hpp>

/**
 * @brief Zdarzenie rejestracji nowego użytkownika.
 *
 * Wywoływane gdy klient żąda założenia nowego konta.
 * Deleguje walidację i tworzenie konta do RegistrationManager.
 */
class RegistrationEvent : public Event
{
public:
    /**
     * @brief Tworzy zdarzenie rejestracji.
     * @param username Żądana nazwa użytkownika.
     * @param password Hasło w postaci jawnej.
     */
    RegistrationEvent(const std::string &username, const std::string &password) : username(username), password(password) {}

    /**
     * @brief Wykonuje rejestrację w podanym kontekście.
     * @param context Kontekst menedżerów serwera.
     */
    void perform(ManagerContext &context) override;

private:
    std::string username; ///< Żądana nazwa użytkownika.
    std::string password; ///< Hasło w postaci jawnej.
};