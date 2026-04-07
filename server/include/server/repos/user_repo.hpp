#pragma once
#include <string>
#include <common/models/user.hpp>
#include <unordered_map>
#include <shared_mutex>
#include <mutex>
#include <vector>

/**
 * @brief Repozytorium użytkowników serwera.
 *
 * Przechowuje wszystkich zarejestrowanych użytkowników w pamięci operacyjnej.
 * Dostęp jest synchronizowany współdzieloną blokadą (shared_mutex).
 */
class UserRepository
{
public:
    UserRepository() = default;

    /**
     * @brief Dodaje użytkownika do repozytorium.
     * @param user Użytkownik do dodania.
     * @return @c true jeśli dodanie się powiódło.
     */
    bool addUser(const User &user);

    /**
     * @brief Usuwa użytkownika z repozytorium.
     * @param username Nazwa użytkownika do usunięcia.
     * @return @c true jeśli usunięcie się powiódło.
     */
    bool removeUser(const std::string &username);

    /**
     * @brief Zwraca użytkownika o podanej nazwie.
     * @param username Nazwa użytkownika.
     * @return Obiekt użytkownika.
     */
    User getUser(const std::string &username);

    /**
     * @brief Sprawdza, czy użytkownik istnieje w repozytorium.
     * @param username Nazwa użytkownika.
     * @return @c true jeśli użytkownik istnieje.
     */
    bool userExists(const std::string &username) const;

    /**
     * @brief Zwraca listę wszystkich użytkowników.
     * @return Wektor wszystkich użytkowników.
     */
    std::vector<User> getAllUsers() const;

private:
    mutable std::shared_mutex mutex;             ///< Blokada synchronizująca dostęp wielowątkowy.
    std::unordered_map<std::string, User> users; ///< Mapa: nazwa użytkownika → obiekt użytkownika.
};