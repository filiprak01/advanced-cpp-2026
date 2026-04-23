#pragma once
#include <unordered_map>
#include <common/models/session.hpp>
#include <nlohmann/json.hpp>
#include <shared_mutex>
#include <mutex>
#include <vector>

using json = nlohmann::json;

/**
 * @brief Repozytorium sesji użytkowników serwera.
 *
 * Przechowuje aktywne sesje w pamięci. Dostęp jest synchronizowany
 * współdzieloną blokadą (shared_mutex).
 */
class SessionRepository
{
public:
    SessionRepository() = default;

    /**
     * @brief Dodaje nową sesję do repozytorium.
     * @param session Sesja do dodania.
     * @return @c true jeśli dodanie się powiódło.
     */
    bool addSession(const Session &session);

    /**
     * @brief Usuwa sesję z repozytorium.
     * @param sessionId Identyfikator sesji do usunięcia.
     * @return @c true jeśli usunięcie się powiódło.
     */
    bool removeSession(const int &sessionId);

    /**
     * @brief Zwraca sesję o podanym identyfikatorze.
     * @param sessionId Identyfikator sesji.
     * @return Obiekt sesji.
     */
    Session getSession(const int &sessionId);

    /**
     * @brief Aktualizuje dane sesji.
     * @param newSession Nowe dane sesji.
     * @param sessionId  Identyfikator sesji do zaktualizowania.
     * @return @c true jeśli aktualizacja się powiódła.
     */
    bool updateSession(const Session &newSession, const int &sessionId);

    /**
     * @brief Sprawdza, czy sesja istnieje w repozytorium.
     * @param sessionId Identyfikator sesji.
     * @return @c true jeśli sesja istnieje.
     */
    bool sessionExists(const int &sessionId) const;

    /// @brief Zwraca kopię wszystkich sesji.
    std::vector<Session> getAllSessions() const;

    /// @brief Serializuje repozytorium sesji do JSON.
    json toJson() const;

    /// @brief Odtwarza repozytorium sesji z JSON.
    void fromJson(const json &j);

private:
    mutable std::shared_mutex mutex;           ///< Blokada synchronizująca dostęp wielowątkowy.
    std::unordered_map<int, Session> sessions; ///< Mapa: identyfikator sesji → obiekt sesji.
};
