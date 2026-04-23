/**
 * @file main.cpp
 * @brief Punkt wejściowy serwera PWChat.
 *
 * Inicjalizuje serwer, rejestruje obsługę sygnałów SIGINT/SIGTERM
 * umożliwiając bezpieczne zamknięcie, a następnie uruchamia główną pętlę.
 */
#include <common/utilities/config.hpp>
#include <server/server.hpp>
#include <cstdlib>
#include <csignal>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

/// @brief Globalny wskaźnik do aktywnej instancji serwera (używany przez obsługę sygnałów).
static Server *g_server = nullptr;

/**
 * @brief Obsługa sygnałów systemowych (SIGINT, SIGTERM).
 * @param sig Numer odebranego sygnału.
 */
static void signalHandler(int sig)
{
    std::cout << "\nSignal " << sig << " received, shutting down..." << std::endl;
    if (g_server)
        g_server->stop();
}

/**
 * @brief Wybiera plik konfiguracyjny serwera niezależnie od katalogu uruchomienia.
 *
 * Kolejność:
 * 1. argument CLI,
 * 2. zmienna środowiskowa PWCHAT_SERVER_CONFIG,
 * 3. ścieżka repozytorium wstrzyknięta przez CMake,
 * 4. lokalne config/server.json jako fallback.
 */
static std::string resolveServerConfigPath(int argc, char *argv[])
{
    std::vector<std::string> candidates;

    if (argc >= 2)
    {
        candidates.emplace_back(argv[1]);
    }

    if (const char *envPath = std::getenv("PWCHAT_SERVER_CONFIG"))
    {
        if (envPath[0] != '\0')
        {
            candidates.emplace_back(envPath);
        }
    }

#ifdef PWCHAT_SERVER_CONFIG_PATH
    candidates.emplace_back(PWCHAT_SERVER_CONFIG_PATH);
#endif

    candidates.emplace_back("config/server.json");
    candidates.emplace_back("../advanced-cpp-2026/config/server.json");

    for (const std::string &candidate : candidates)
    {
        if (std::filesystem::exists(candidate))
        {
            return candidate;
        }
    }

    return candidates.empty() ? "config/server.json" : candidates.front();
}

/**
 * @brief Główna funkcja programu.
 * @return 0 przy poprawnym zamknięciu, 1 przy błędzie krytycznym.
 */
int main(int argc, char *argv[])
{
    try
    {
        const std::string configPath = resolveServerConfigPath(argc, argv);
        ServerConfig config = ServerConfig().readFromFile(configPath);
        std::cout << "Using server config: " << configPath << std::endl;
        Server server(config);
        g_server = &server;
        std::signal(SIGINT, signalHandler);
        std::signal(SIGTERM, signalHandler);
        server.run();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Fatal: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
