#pragma once
#include <common/models/connection.hpp>
#include <server/repos/message_repo.hpp>
#include <server/repos/user_repo.hpp>
#include <server/repos/session_repo.hpp>

#include <unordered_map>
#include <string>
#include <nlohmann/json.hpp>
using json = nlohmann::json;
/**
 * @brief Menedżer połączeń sieciowych serwera.
 *
 * Obsługuje akceptowanie nowych połączeń TCP, wysyłanie i odbieranie
 * wiadomości oraz mapowanie deskryptorów gniazd na nazwy użytkowników.
 */
class ConnectionManager
{
public:
    /**
     * @brief Tworzy menedżer połączeń.
     * @param server_fd Deskryptor nasłuchującego gniazda serwera.
     */
    ConnectionManager(int server_fd) : server_fd(server_fd) {}

    /**
     * @brief Akceptuje nowe połączenie TCP.
     * @return Deskryptor gniazda klienta, -1 w przypadku błędu.
     */
    int acceptConnection();

    /**
     * @brief Zamyka połączenie po deskryptorze gniazda.
     * @param client_fd Deskryptor gniazda klienta.
     * @return @c true jeśli zamknięcie się powiódło.
     */
    bool closeConnection(int client_fd);

    /**
     * @brief Zamyka połączenie po nazwie użytkownika.
     * @param userName Nazwa użytkownika.
     * @return @c true jeśli zamknięcie się powiódło.
     */
    bool closeConnection(const std::string &userName);

    /**
     * @brief Wysyła wiadomość do klienta.
     * @param client_fd Deskryptor gniazda klienta.
     * @param message   Treść wiadomości.
     * @return @c true jeśli wysyłanie się powiódło.
     */
    bool sendMessage(int client_fd, const std::string &message);

    /**
     * @brief Odbiera wiadomość od klienta.
     * @param client_fd Deskryptor gniazda klienta.
     * @return Odebrana wiadomość jako string.
     */
    json receiveMessage(int client_fd);

    /**
     * @brief Aktualizuje mapowanie: deskryptor gniazda → nazwa użytkownika.
     * @param client_fd Deskryptor gniazda klienta.
     * @param userName  Nazwa użytkownika.
     * @return @c true jeśli aktualizacja się powiódła.
     */
    bool updateConnectionMapping(int client_fd, const std::string &userName);

    /// @brief Zwraca deskryptor gniazda serwera.
    const int getServerFD() const
    {
        return server_fd;
    }

    /// @brief Zwraca mapę deskryptorów klientów na nazwy użytkowników.
    const std::unordered_map<int, std::string> &getConnectionMap() const
    {
        return connectionMap;
    }

private:
    int server_fd;                                      ///< Deskryptor gniazda serwera.
    std::unordered_map<int, std::string> connectionMap; ///< Mapa: fd klienta → nazwa użytkownika.
};