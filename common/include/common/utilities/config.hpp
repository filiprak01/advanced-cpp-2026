#pragma once
#include <string>

/**
 * @brief Konfiguracja serwera PWChat wczytywana z pliku JSON.
 *
 * Zawiera wszystkie parametry sterujące zachowaniem serwera:
 * połączenia sieciowe, limity użytkowników, reguły haseł,
 * ustawienia kolejki zdarzeń oraz logowanie.
 */
class ServerConfig
{
public:
    /// @brief Tworzy konfigurację serwera z wartościami domyślnymi.
    ServerConfig()
        : host("0.0.0.0"), port(8090), maxConnections(100),
          sessionTimeout(1800), pingInterval(60), maxInactiveTime(300),
          minUsernameLength(3), maxUsernameLength(20),
          minPasswordLength(6), maxPasswordLength(64),
          maxChannels(50), maxUsersPerChannel(32),
          maxMessageLength(1000), maxMessagesPerSecond(5),
          maxEventQueueSize(1000), eventProcessingThreadPoolSize(4),
          logLevel(1), enableConsoleLogging(true) {}

    /**
     * @brief Tworzy pełną konfigurację serwera.
     * @param host                        Adres nasłuchu serwera.
     * @param port                        Numer portu serwera.
     * @param maxConnections              Maksymalna liczba jednoczesnych połączeń.
     * @param sessionTimeout              Czas wygaśnięcia sesji (sekundy).
     * @param pingInterval                Interwał wysyłania pingów (sekundy).
     * @param maxInactiveTime             Maksymalny czas bezczynności (sekundy).
     * @param minUsernameLength           Minimalna długość nazwy użytkownika.
     * @param maxUsernameLength           Maksymalna długość nazwy użytkownika.
     * @param minPasswordLength           Minimalna długość hasła.
     * @param maxPasswordLength           Maksymalna długość hasła.
     * @param maxChannels                 Maksymalna liczba kanałów.
     * @param maxUsersPerChannel          Maksymalna liczba użytkowników w kanale.
     * @param maxMessageLength            Maksymalna długość wiadomości.
     * @param maxMessagesPerSecond        Limit wiadomości na sekundę na użytkownika.
     * @param maxEventQueueSize           Maksymalny rozmiar kolejki zdarzeń.
     * @param eventProcessingThreadPoolSize Rozmiar puli wątków do przetwarzania zdarzeń.
     * @param logLevel                    Poziom logowania (0=ERROR, 1=WARN, 2=INFO, 3=DEBUG).
     * @param enableConsoleLogging        Czy logować na konsolę.
     */
    ServerConfig(const std::string &host, int port, int maxConnections,
                 int sessionTimeout, int pingInterval, int maxInactiveTime,
                 int minUsernameLength, int maxUsernameLength,
                 int minPasswordLength, int maxPasswordLength,
                 int maxChannels, int maxUsersPerChannel,
                 int maxMessageLength, int maxMessagesPerSecond,
                 int maxEventQueueSize, int eventProcessingThreadPoolSize,
                 int logLevel, bool enableConsoleLogging)
        : host(host), port(port), maxConnections(maxConnections),
          sessionTimeout(sessionTimeout), pingInterval(pingInterval), maxInactiveTime(maxInactiveTime),
          minUsernameLength(minUsernameLength), maxUsernameLength(maxUsernameLength),
          minPasswordLength(minPasswordLength), maxPasswordLength(maxPasswordLength),
          maxChannels(maxChannels), maxUsersPerChannel(maxUsersPerChannel),
          maxMessageLength(maxMessageLength), maxMessagesPerSecond(maxMessagesPerSecond),
          maxEventQueueSize(maxEventQueueSize), eventProcessingThreadPoolSize(eventProcessingThreadPoolSize),
          logLevel(logLevel), enableConsoleLogging(enableConsoleLogging) {}

    /**
     * @brief Wczytuje konfigurację serwera z pliku JSON.
     * @param filename Ścieżka do pliku konfiguracyjnego.
     * @return Obiekt ServerConfig z załadowanymi parametrami.
     */
    ServerConfig readFromFile(const std::string &filename);

    /**
     * @brief Zapisuje konfigurację serwera do pliku JSON.
     * @param filename Ścieżka docelowego pliku.
     * @return @c true jeśli zapis się powiódł.
     */
    bool exportToFile(const std::string &filename);

    /// @brief Zwraca adres hosta serwera.
    std::string getHost() const
    {
        return host;
    }
    /// @brief Zwraca numer portu serwera.
    int getPort() const { return port; }
    /// @brief Zwraca maksymalną liczbę połączeń.
    int getMaxConnections() const { return maxConnections; }
    /// @brief Zwraca czas wygaśnięcia sesji w sekundach.
    int getSessionTimeout() const { return sessionTimeout; }
    /// @brief Zwraca interwał pingów w sekundach.
    int getPingInterval() const { return pingInterval; }
    /// @brief Zwraca maksymalny czas bezczynności w sekundach.
    int getMaxInactiveTime() const { return maxInactiveTime; }
    /// @brief Zwraca minimalną długość nazwy użytkownika.
    int getMinUsernameLength() const { return minUsernameLength; }
    /// @brief Zwraca maksymalną długość nazwy użytkownika.
    int getMaxUsernameLength() const { return maxUsernameLength; }
    /// @brief Zwraca minimalną długość hasła.
    int getMinPasswordLength() const { return minPasswordLength; }
    /// @brief Zwraca maksymalną długość hasła.
    int getMaxPasswordLength() const { return maxPasswordLength; }
    /// @brief Zwraca maksymalną liczbę kanałów.
    int getMaxChannels() const { return maxChannels; }
    /// @brief Zwraca maksymalną liczbę użytkowników w kanale.
    int getMaxUsersPerChannel() const { return maxUsersPerChannel; }
    /// @brief Zwraca maksymalną długość wiadomości.
    int getMaxMessageLength() const { return maxMessageLength; }
    /// @brief Zwraca limit wiadomości na sekundę.
    int getMaxMessagesPerSecond() const { return maxMessagesPerSecond; }
    /// @brief Zwraca maksymalny rozmiar kolejki zdarzeń.
    int getMaxEventQueueSize() const { return maxEventQueueSize; }
    /// @brief Zwraca rozmiar puli wątków zdarzeń.
    int getEventProcessingThreadPoolSize() const { return eventProcessingThreadPoolSize; }
    /// @brief Zwraca poziom logowania (0=ERROR, 1=WARN, 2=INFO, 3=DEBUG).
    int getLogLevel() const { return logLevel; }
    /// @brief Zwraca @c true jeśli logowanie na konsolę jest włączone.
    bool isConsoleLoggingEnabled() const { return enableConsoleLogging; }

private:
    std::string host;                  ///< Adres hosta serwera.
    int port;                          ///< Numer portu.
    int maxConnections;                ///< Maks. liczba jednoczesnych połączeń.
    int sessionTimeout;                ///< Timeout sesji w sekundach.
    int pingInterval;                  ///< Interwał pingów w sekundach.
    int maxInactiveTime;               ///< Maks. bezczynność w sekundach.
    int minUsernameLength;             ///< Min. długość nazwy użytkownika.
    int maxUsernameLength;             ///< Maks. długość nazwy użytkownika.
    int minPasswordLength;             ///< Min. długość hasła.
    int maxPasswordLength;             ///< Maks. długość hasła.
    int maxChannels;                   ///< Maks. liczba kanałów.
    int maxUsersPerChannel;            ///< Maks. użytkowników w kanale.
    int maxMessageLength;              ///< Maks. długość wiadomości.
    int maxMessagesPerSecond;          ///< Maks. wiadomości na sekundę.
    int maxEventQueueSize;             ///< Maks. rozmiar kolejki zdarzeń.
    int eventProcessingThreadPoolSize; ///< Rozmiar puli wątków.
    int logLevel;                      ///< Poziom logowania (0-3).
    bool enableConsoleLogging;         ///< Czy logować na konsolę.
};

/**
 * @brief Konfiguracja klienta PWChat wczytywana z pliku JSON.
 *
 * Zawiera parametry połączenia z serwerem oraz ustawienia interfejsu użytkownika.
 */
class ClientConfig
{
public:
    ClientConfig() = default;

    /**
     * @brief Tworzy pełną konfigurację klienta.
     * @param serverIp             Adres IP serwera.
     * @param serverPort           Port serwera.
     * @param autoReconnect        Czy automatycznie wznawiać połączenie.
     * @param reconnectIntervalMs  Interwał ponownego łączenia (ms).
     * @param maxMessageLength     Maks. długość wiadomości.
     * @param sendOnEnter          Czy wysyłać wiadomość po Enter.
     * @param messageCacheSize     Liczba wiadomości w pamięci podręcznej.
     * @param showConnectionStatus Czy wyświetlać stan połączenia.
     */
    ClientConfig(int serverIp, int serverPort, bool autoReconnect,
                 int reconnectIntervalMs, int maxMessageLength,
                 bool sendOnEnter, int messageCacheSize, bool showConnectionStatus)
        : serverIp(serverIp), serverPort(serverPort), autoReconnect(autoReconnect),
          reconnectIntervalMs(reconnectIntervalMs), maxMessageLength(maxMessageLength),
          sendOnEnter(sendOnEnter), messageCacheSize(messageCacheSize),
          showConnectionStatus(showConnectionStatus) {}

    /**
     * @brief Wczytuje konfigurację klienta z pliku JSON.
     * @param filename Ścieżka do pliku konfiguracyjnego.
     * @return Obiekt ClientConfig z załadowanymi parametrami.
     */
    ClientConfig readFromFile(const std::string &filename);

    /**
     * @brief Zapisuje konfigurację klienta do pliku JSON.
     * @param filename Ścieżka docelowego pliku.
     * @return @c true jeśli zapis się powiódł.
     */
    bool exportToFile(const std::string &filename);

    /// @brief Zwraca adres IP serwera.
    int getServerIp() const { return serverIp; }
    /// @brief Zwraca port serwera.
    int getServerPort() const { return serverPort; }
    /// @brief Zwraca @c true jeśli automatyczne wznawianie połączenia jest włączone.
    bool isAutoReconnectEnabled() const { return autoReconnect; }
    /// @brief Zwraca interwał ponownego łączenia w milisekundach.
    int getReconnectIntervalMs() const { return reconnectIntervalMs; }
    /// @brief Zwraca maksymalną długość wiadomości klienta.
    int getMaxMessageLength() const { return maxMessageLength; }
    /// @brief Zwraca @c true jeśli wysyłanie Enter jest aktywne.
    bool isSendOnEnterEnabled() const { return sendOnEnter; }
    /// @brief Zwraca rozmiar pamięci podręcznej wiadomości.
    int getMessageCacheSize() const { return messageCacheSize; }
    /// @brief Zwraca @c true jeśli wyświetlanie stanu połączenia jest włączone.
    bool isShowConnectionStatusEnabled() const { return showConnectionStatus; }

private:
    int serverIp;              ///< Adres IP serwera.
    int serverPort;            ///< Port serwera.
    bool autoReconnect;        ///< Czy automatycznie wznawiać połączenie.
    int reconnectIntervalMs;   ///< Interwał wznawiania połączenia w ms.
    int maxMessageLength;      ///< Maks. długość wiadomości.
    bool sendOnEnter;          ///< Czy wysyłać po Enter.
    int messageCacheSize;      ///< Rozmiar pamięci podręcznej wiadomości.
    bool showConnectionStatus; ///< Czy wyświetlać stan połączenia.
};