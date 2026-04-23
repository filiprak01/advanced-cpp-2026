#pragma once
#include <common/const/event_messages.hpp>
#include <common/models/connection.hpp>
#include <server/core/domain_result.hpp>
#include <server/repos/message_repo.hpp>
#include <server/repos/user_repo.hpp>
#include <server/repos/session_repo.hpp>
#include <string_view>
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
     * @brief Rejestruje polaczenie w mapie bez wywolywania acceptConnection().
     * @param client_fd Deskryptor gniazda klienta.
     * @param userName  Nazwa uzytkownika przypisana do polaczenia.
     * @return @c true jezeli wpis zostal zapisany.
     */
    bool registerConnection(int client_fd, const std::string &userName = "");

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
     * @brief Wysyła wiadomość do klienta po deskryptorze gniazda.
     * @param fd      Deskryptor gniazda klienta.
     * @param message Treść wiadomości.
     * @return @c true jeśli wysyłanie się powiodło.
     */
    bool sendMessage(int fd, const json &message);
    bool sendErrorMessage(int fd, const DomainResult &result);
    bool sendSuccessMessage(int fd, std::string_view type, const DomainResult &result, const json &payload = json::object());
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

    const std::string getUsernameFromFd(int fd);

private:
    int server_fd;                                      ///< Deskryptor gniazda serwera.
    std::unordered_map<int, std::string> connectionMap; ///< Mapa: fd klienta → nazwa użytkownika.
};
