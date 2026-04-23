#pragma once
#include <server/repos/session_repo.hpp>
#include <unordered_map>
#include <server/core/connection_manager.hpp>
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
    SessionManager(SessionRepository &sessionRepository, ConnectionManager &connectionManager, std::chrono::milliseconds sessionTimeout, std::unordered_map<std::string, int> userSessions, int nextSessionId = 1)
        : sessionRepository(sessionRepository), connectionManager(connectionManager), sessionTimeout(sessionTimeout), userSessions(userSessions), nextSessionId(nextSessionId) {}
    /// @brief Usuwa wszystkie nieaktywne sesje. Zwraca @c true jeśli coś usunięto.
    bool cleanInactiveSessions();

    /**
     * @brief Sprawdza, czy użytkownik ma aktywną sesję (jest zalogowany).
     * @param userName Nazwa użytkownika.
     * @return @c true jeśli sesja istnieje.
     */
    bool hasSession(int fd);

    /**
     * @brief Tworzy nową sesję dla użytkownika.
     * @param userId Identyfikator użytkownika.
     * @return @c true jeśli tworzenie się powiódło.
     */
    bool createSession(int fd);

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
    bool updateUserSession(int fd);

    /**
     * @brief Sprawdza, czy sesja jest aktywna (nie wygasła).
     * @param sessionId Identyfikator sesji.
     * @return @c true jeśli sesja jest ważna.
     */
    bool isSessionValid(const int &sessionId);

private:
    SessionRepository &sessionRepository; ///< Repozytorium sesji.
    ConnectionManager &connectionManager;
    std::chrono::milliseconds sessionTimeout;          ///< Timeout sesji.
    std::unordered_map<std::string, int> userSessions; ///< Mapa: userName → sessionId.
    int nextSessionId;                                 ///< Licznik identyfikatorów sesji.
};