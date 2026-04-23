#pragma once
#include <server/repos/user_repo.hpp>
#include <server/repos/session_repo.hpp>
#include <server/repos/channel_repo.hpp>
#include <server/repos/message_repo.hpp>
#include <server/auth/password_hasher.hpp>
#include <server/core/connection_manager.hpp>
#include <server/core/session_manager.hpp>
#include <server/core/channel_manager.hpp>
#include <server/core/message_manager.hpp>
#include <server/auth/auth_manager.hpp>
#include <server/auth/registration_manager.hpp>
#include <server/core/event.hpp>
#include <server/core/factory/event_factory.hpp>
#include <common/utilities/config.hpp>
#include <queue>

/**
 * @brief Główna klasa serwera PWChat.
 *
 * Inicjalizuje wszystkie repozytoria, menedżery i nasłuchuje
 * na połączenia klientów TCP na podanym porcie. Obsługuje sygnały
 * systemowe (SIGINT, SIGTERM) w celu bezpiecznego zamknięcia.
 */
class Server
{
public:
    /**
     * @brief Tworzy serwer na podstawie konfiguracji.
     * @param config Konfiguracja serwera (port, limity, timeouty itd.).
     * @throws std::runtime_error jeśli nie można związać gniazda z portem.
     */
    explicit Server(const ServerConfig &config);

    /**
     * @brief Tworzy serwer nasłuchujący na podanym porcie z domyślną konfiguracją.
     * @param port Numer portu TCP do nasłuchu.
     * @throws std::runtime_error jeśli nie można związać gniazda z portem.
     */
    explicit Server(int port);

    ~Server();

    /// @brief Uruchamia główną pętlę serwera (blokujące).
    void run();

    /// @brief Zatrzymuje serwer i zamyka wszystkie połączenia.
    void stop();

private:
    /**
     * @brief Tworzy i konfiguruje gniazdo nasłuchowe TCP.
     * @param port Numer portu.
     * @return Deskryptor gniazda serwera.
     */
    static int createAndBindSocket(const std::string &host, int port);

    void loadRepositories();
    void saveRepositories() const;

    int server_fd; ///< Deskryptor gniazda nasłuchowego.

    ServerConfig config; ///< Konfiguracja serwera (port, limity, timeouty).
    bool persistenceEnabled; ///< Czy zapisywać i wczytywać repozytoria z plików.

    // Repozytoria (własnościowe)
    UserRepository userRepo;       ///< Repozytorium użytkowników.
    SessionRepository sessionRepo; ///< Repozytorium sesji.
    ChannelRepository channelRepo; ///< Repozytorium kanałów.
    MessageRepository messageRepo; ///< Repozytorium wiadomości.

    // Narzędzia autoryzacyjne (własnościowe)
    PasswordHasher passwordHasher; ///< Narzędzie do hashowania haseł.

    // Menedżery (własnościowe, trzymają referencje do repozytoriów)
    ConnectionManager connectionManager;     ///< Menedżer połączeń sieciowych.
    SessionManager sessionManager;           ///< Menedżer sesji użytkowników.
    ChannelManager channelManager;           ///< Menedżer kanałów.
    MessageManager messageManager;           ///< Menedżer wiadomości.
    AuthManager authManager;                 ///< Menedżer uwierzytelniania.
    RegistrationManager registrationManager; ///< Menedżer rejestracji.

    bool running; ///< Flaga sterująca główną pętlą serwera.
};
