#include <client/client_backend.hpp>
#include <common/utilities/config.hpp>

#include <chrono>
#include <iostream>
#include <stdexcept>
#include <string>
#include <thread>

/**
 * CLI smoke test for the client backend.
 *
 * Reads config/client.json, connects to the server, logs in with the
 * credentials supplied as command-line arguments, sends one hard-coded
 * message, then waits briefly for the echo/response before exiting.
 *
 * Usage:  PWChatClient <username> <password> [config_path]
 */
int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        std::cerr << "Usage: " << argv[0] << " <username> <password> [config_path]\n";
        return 1;
    }

    const std::string username = argv[1];
    const std::string password = argv[2];
    const std::string configPath = argc >= 4 ? argv[3] : "config/client.json";

    ClientConfig cfg;
    try
    {
        cfg = ClientConfig().readFromFile(configPath);
    }
    catch (const std::exception &e)
    {
        std::cerr << "Failed to load config from '" << configPath << "': " << e.what()
                  << "\nUsing default config.\n";
    }

    ClientBackend backend(cfg);

    // Register a simple printer for all responses
    backend.dispatcher().setFallbackHandler([](const json &msg)
                                            { std::cout << "[response] " << msg.dump(2) << "\n"; });

    try
    {
        std::cout << "Connecting…\n";
        backend.connect();
        std::cout << "Connected. Logging in as '" << username << "'…\n";
        backend.login(username, password);

        // Give the server a moment to respond
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        backend.drainQueue(); // handlers fired inside recvLoop already; drain flushes the queue

        std::cout << "Sending test message…\n";
        backend.sendMessage(1, "Hello from PWChatClient smoke test!");

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        backend.drainQueue();

        std::cout << "Done. Disconnecting.\n";
        backend.disconnect();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
