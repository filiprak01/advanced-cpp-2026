#pragma once
#include <server/repos/session_repo.hpp>
#include <unordered_map>

/**
 * @brief Menedżer sesji użytkowników serwera.
 *
 * Odpowiada za tworzenie, usuwanie i odświeżanie sesji
 * oraz automatyczne czyszczenie nieaktywnych sesji.
 */
class SessionManager
{
public:
    /**
     * @brief Tworzy menedżer sesji.
     * @param sessionRepository Repozytorium sesji.
     * @param sessionTimeout    Czas wygasania sesji.
     * @param userSessions      Początkowa mapa: userId → sessionId.
     * @param nextSessionId     Początkowa wartość licznika sesji.
     */
    SessionManager(SessionRepository &sessionRepository, std::chrono::milliseconds sessionTimeout, std::unordered_map<std::string, int> userSessions, int nextSessionId = 1)
        : sessionRepository(sessionRepository), sessionTimeout(sessionTimeout), userSessions(userSessions), nextSessionId(nextSessionId) {}

    /// @brief Usuwa wszystkie nieaktywne sesje. Zwraca @c true jeśli coś usunięto.
    bool cleanInactiveSessions();

    /**
     * @brief Tworzy nową sesję dla użytkownika.
     * @param userId Identyfikator użytkownika.
     * @return @c true jeśli tworzenie się powiódło.
     */
    bool createSession(const std::string &userName);

    /**
     * @brief Usuwa sesję użytkownika.
     * @param userId Identyfikator użytkownika.
     * @return @c true jeśli usunięcie się powiódło.
     */
    bool removeUserSession(const std::string &userName);

    /**
     * @brief Aktualizuje czas ostatniej aktywności sesji użytkownika.
     * @param userId Identyfikator użytkownika.
     * @return @c true jeśli aktualizacja się powiódła.
     */
    bool updateUserSession(const std::string &userName);

private:
    SessionRepository &sessionRepository;              ///< Repozytorium sesji.
    std::chrono::milliseconds sessionTimeout;          ///< Timeout sesji.
    std::unordered_map<std::string, int> userSessions; ///< Mapa: userName → sessionId.
    int nextSessionId;                                 ///< Licznik identyfikatorów sesji.

    /**
     * @brief Sprawdza, czy sesja jest aktywna (nie wygasła).
     * @param sessionId Identyfikator sesji.
     * @return @c true jeśli sesja jest ważna.
     */
    bool isSessionValid(const int &sessionId);
};