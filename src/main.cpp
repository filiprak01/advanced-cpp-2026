/**
 * @file main.cpp
 * @brief Punkt wejściowy serwera PWChat.
 *
 * Inicjalizuje serwer, rejestruje obsługę sygnałów SIGINT/SIGTERM
 * umożliwiając bezpieczne zamknięcie, a następnie uruchamia główną pętlę.
 */
#include <server/server.hpp>
#include <csignal>
#include <iostream>

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
 * @brief Główna funkcja programu.
 * @return 0 przy poprawnym zamknięciu, 1 przy błędzie krytycznym.
 */
int main()
{
    try
    {
        Server server(8090);
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
