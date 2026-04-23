#pragma once
#include <client/response_dispatcher.hpp>
#include <client/socket_connection.hpp>
#include <common/models/channel.hpp>
#include <common/models/message.hpp>
#include <common/utilities/config.hpp>
#include <client/connection_state.hpp>
#include <atomic>
#include <deque>
#include <functional>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

/**
 * @brief Backend klienta odpowiedzialny za transport, odbior i lokalny cache.
 *
 * Klasa udostepnia metody uzywane przez UI, np. logowanie, tworzenie kanalow
 * i wysylanie wiadomosci. Odpowiedzi serwera trafiaja do kolejki, ktora
 * warstwa UI okresowo oproznia przez drainQueue().
 *
 * Watek odbiorczy zapisuje nowe odpowiedzi pod ochrona queueMutex. Cache
 * kanalow i wiadomosci jest aktualizowany dopiero po odebraniu komunikatu
 * przez czesc kliencka, dzieki czemu logika UI nie musi bezposrednio operowac
 * na gniezdzie sieciowym.
 */
class ClientBackend
{
public:
    explicit ClientBackend(ClientConfig config);
    ~ClientBackend();

    /// Obiekt zarzadza gniazdem i watkiem odbiorczym, dlatego nie jest kopiowalny.
    ClientBackend(const ClientBackend &) = delete;
    ClientBackend &operator=(const ClientBackend &) = delete;

    /**
     * @brief Laczy z serwerem wskazanym w konfiguracji i uruchamia watek odbiorczy.
     * @throws std::runtime_error Gdy nie uda sie zestawic polaczenia.
     */
    void connect();

    /// @brief Rozlacza klienta i zatrzymuje watek odbiorczy.
    void disconnect();

    /// @brief Zwraca @c true, jesli gniazdo jest aktywne.
    bool isConnected() const;

    /// @brief Zwraca aktualny stan polaczenia klienta.
    connection::ConnectionState getConnectionState() const;

    /**
     * @brief Zwraca i usuwa z kolejki wszystkie odebrane komunikaty serwera.
     *
     * Metoda jest wywolywana przez UI cyklicznie, aby przetworzyc odpowiedzi
     * oraz zdarzenia wyslane przez serwer.
     */
    std::vector<json> drainQueue();

    /// @brief Wysyla zadanie logowania.
    void login(const std::string &username, const std::string &password);

    /// @brief Wysyla zadanie rejestracji nowego uzytkownika.
    void registerUser(const std::string &username, const std::string &password);

    /// @brief Wysyla wiadomosc do kanalu.
    void sendMessage(int channelId, const std::string &content);

    /// @brief Edytuje istniejaca wiadomosc.
    void editMessage(int messageId, const std::string &newContent);

    /// @brief Usuwa wiadomosc z kanalu.
    void removeMessage(int messageId, int channelId);

    /// @brief Tworzy kanal publiczny albo prywatny.
    void createChannel(const std::string &name, const std::vector<std::string> &members, bool isPrivate);

    /// @brief Zmienia nazwe kanalu.
    void editChannel(int channelId, const std::string &newName);

    /// @brief Usuwa kanal.
    void removeChannel(int channelId);

    /// @brief Dodaje uzytkownika do kanalu.
    void addUserToChannel(int channelId, const std::string &userName);

    /// @brief Pobiera pelny stan kanalow i wiadomosci z serwera.
    void synchronize();

    /**
     * @brief Wysyla gotowy tekst JSON bez dodatkowego mapowania.
     * @param rawJson Kompletny komunikat JSON do wyslania.
     */
    void sendRaw(const std::string &rawJson);

    /// @brief Zwraca komunikaty statusu utworzone na podstawie odpowiedzi serwera.
    std::vector<std::string> drainStatus();

    /// @brief Zwraca lokalny cache kanalow.
    const std::unordered_map<int, Channel> &getChannels() const { return channels; }

    /// @brief Zwraca lokalny cache wiadomosci.
    const std::unordered_map<int, Message> &getMessages() const { return messages; }

    /// @brief Zwraca nazwe aktualnie zalogowanego uzytkownika.
    const std::string &getCurrentUser() const { return currentUser; }

    /// @brief Udostepnia dispatcher odpowiedzi serwera.
    ResponseDispatcher &dispatcher() { return responseDispatcher; }

private:
    void sendJson(const json &msg);
    void sendPayload(const std::string &payload);
    void recvLoop();
    void setupDefaultHandlers();
    void pushStatus(const std::string &message);
    void markDisconnected();
    void markConnectionLost();
    void markConnected();
    void joinRecvThreadIfNeeded();

    ClientConfig config;
    SocketConnection conn;
    ResponseDispatcher responseDispatcher;

    std::queue<json> incomingQueue;
    std::mutex queueMutex;
    std::queue<std::string> statusQueue;
    std::mutex statusMutex;

    std::thread recvThread;
    std::atomic<bool> shouldStop{false};

    // Cache odczytywany przez UI po przetworzeniu odpowiedzi z kolejki.
    std::unordered_map<int, Channel> channels;
    std::unordered_map<int, Message> messages;
    std::string currentUser;

    // Stan polaczenia moze zmienic watek odbiorczy albo watek UI.
    std::atomic<connection::ConnectionState> currentConnectionState{
        connection::ConnectionState::Disconnected};
};
