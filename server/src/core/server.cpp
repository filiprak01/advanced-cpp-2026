#include <server/server.hpp>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <poll.h>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <nlohmann/json.hpp>
#include <common/const/event_messages.hpp>

using json = nlohmann::json;
int Server::createAndBindSocket(int port)
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0)
        throw std::runtime_error("Failed to create socket");

    int opt = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(static_cast<uint16_t>(port));

    if (bind(fd, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) < 0)
    {
        close(fd);
        throw std::runtime_error("Failed to bind to port " + std::to_string(port));
    }

    if (listen(fd, 10) < 0)
    {
        close(fd);
        throw std::runtime_error("Failed to listen on socket");
    }

    std::cout << "Server listening on port " << port << std::endl;
    return fd;
}

Server::Server(int port)
    : config(ServerConfig()),
      server_fd(createAndBindSocket(port)),
      userRepo(),
      sessionRepo(),
      channelRepo(),
      messageRepo(),
      passwordHasher(),
      connectionManager(server_fd),
      sessionManager(sessionRepo, connectionManager, std::chrono::milliseconds(30 * 60 * 1000), {}),
      channelManager(channelRepo, userRepo),
      messageManager(connectionManager, messageRepo, userRepo, channelRepo, 1, {}),
      authManager(userRepo, passwordHasher),
      registrationManager(userRepo, passwordHasher),
      running(false)
{
}

Server::Server(const ServerConfig &cfg)
    : config(cfg),
      server_fd(createAndBindSocket(cfg.getPort())),
      userRepo(),
      sessionRepo(),
      channelRepo(),
      messageRepo(),
      passwordHasher(),
      connectionManager(server_fd),
      sessionManager(sessionRepo, connectionManager,
                     std::chrono::milliseconds(
                         static_cast<long long>(cfg.getSessionTimeout()) * 1000),
                     {}),
      channelManager(channelRepo, userRepo),
      messageManager(connectionManager, messageRepo, userRepo, channelRepo, 1, {}),
      authManager(userRepo, passwordHasher),
      registrationManager(userRepo, passwordHasher),
      running(false)
{
}

Server::~Server()
{
    if (server_fd >= 0)
        close(server_fd);
}

void Server::stop()
{
    running = false;
}

void Server::run()
{
    EventFactory eventFactory = EventFactory();
    ManagerContext managerContext(authManager, registrationManager, connectionManager, sessionManager, channelManager, messageManager);
    running = true;

    std::vector<pollfd> fds;
    fds.push_back({server_fd, POLLIN, 0});

    std::cout << "Waiting for connections..." << std::endl;

    while (running)
    {
        int ret = poll(fds.data(), static_cast<nfds_t>(fds.size()), 1000);
        if (ret < 0)
        {
            std::cerr << "poll error: " << strerror(errno) << std::endl;
            break;
        }

        for (size_t i = 0; i < fds.size(); ++i)
        {
            if (!(fds[i].revents & POLLIN))
                continue;

            if (fds[i].fd == server_fd)
            {
                int client_fd = connectionManager.acceptConnection();
                if (client_fd >= 0)
                {
                    const std::string banner =
                        "PWChat server ready. fd=" + std::to_string(client_fd) + "\n";
                    send(client_fd, banner.c_str(), banner.size(), 0);
                    fds.push_back({client_fd, POLLIN, 0});
                }
            }
            else
            {
                bool clientDisconnected = false;
                try
                {
                    json received = connectionManager.receiveMessage(fds[i].fd);
                    // Empty string returned by receiveMessage means client disconnected
                    if (received.is_string() && received.get<std::string>().empty())
                    {
                        clientDisconnected = true;
                    }
                    else
                    {
                        std::cout << "Received JSON from fd " << fds[i].fd << ": " << received.dump() << std::endl;
                        std::unique_ptr<Event> event = eventFactory.createEvent(received);
                        if (event)
                        {
                            event->perform(managerContext, fds[i].fd);
                        }
                        else
                        {
                            connectionManager.sendErrorMessage(fds[i].fd, DomainResult::formatError(errors::Code::unknown_event_type));
                        }
                    }
                }
                catch (const std::exception &e)
                {
                    std::cout << "Error handling fd " << fds[i].fd << ": " << e.what() << std::endl;
                    clientDisconnected = true;
                }
                catch (...)
                {
                    std::cout << "Unknown error handling fd " << fds[i].fd << std::endl;
                    clientDisconnected = true;
                }

                if (clientDisconnected)
                {
                    fds.erase(fds.begin() + static_cast<std::ptrdiff_t>(i));
                    --i; // adjust index after erase
                }
            }
        }
    }
}
