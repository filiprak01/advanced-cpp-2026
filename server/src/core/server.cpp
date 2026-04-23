#include <server/server.hpp>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <poll.h>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <nlohmann/json.hpp>
#include <common/const/event_messages.hpp>

using json = nlohmann::json;

namespace
{
    void createParentDirectory(const std::string &filename)
    {
        const std::filesystem::path path(filename);
        const auto parent = path.parent_path();
        if (!parent.empty())
        {
            std::filesystem::create_directories(parent);
        }
    }

    json readArrayFile(const std::string &filename)
    {
        createParentDirectory(filename);
        if (!std::filesystem::exists(filename))
        {
            std::ofstream created(filename);
            created << "[]\n";
            return json::array();
        }

        std::ifstream file(filename);
        if (!file.is_open() || file.peek() == std::ifstream::traits_type::eof())
        {
            return json::array();
        }

        json data = json::parse(file);
        return data.is_array() ? data : json::array();
    }

    bool writeJsonFile(const std::string &filename, const json &data)
    {
        createParentDirectory(filename);
        std::ofstream file(filename);
        if (!file.is_open())
        {
            return false;
        }

        file << data.dump(4) << '\n';
        return true;
    }
} // namespace

int Server::createAndBindSocket(const std::string &host, int port)
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0)
        throw std::runtime_error("Failed to create socket");

    int opt = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    if (host == "0.0.0.0")
    {
        addr.sin_addr.s_addr = INADDR_ANY;
    }
    else if (inet_pton(AF_INET, host.c_str(), &addr.sin_addr) != 1)
    {
        close(fd);
        throw std::runtime_error("Invalid server host " + host);
    }
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

    std::cout << "Server listening on " << host << ":" << port << std::endl;
    return fd;
}

Server::Server(int port)
    : server_fd(createAndBindSocket("0.0.0.0", port)),
      config(ServerConfig()),
      persistenceEnabled(false),
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
    : server_fd(createAndBindSocket(cfg.getHost(), cfg.getPort())),
      config(cfg),
      persistenceEnabled(true),
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
      registrationManager(userRepo, passwordHasher,
                          cfg.getMinPasswordLength(),
                          cfg.getMaxPasswordLength(),
                          cfg.getMinUsernameLength(),
                          cfg.getMaxUsernameLength()),
      running(false)
{
    loadRepositories();
}

Server::~Server()
{
    if (persistenceEnabled)
    {
        saveRepositories();
    }

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
            if (errno == EINTR)
            {
                continue;
            }
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
                    fds.push_back({client_fd, POLLIN, 0});
                }
            }
            else
            {
                bool clientDisconnected = false;
                try
                {
                    json received = connectionManager.receiveMessage(fds[i].fd);
                    if (received.is_null() || (received.is_string() && received.get<std::string>().empty()))
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
                    connectionManager.closeConnection(fds[i].fd);
                    fds.erase(fds.begin() + static_cast<std::ptrdiff_t>(i));
                    --i; // adjust index after erase
                }
            }
        }
    }
}

void Server::loadRepositories()
{
    try
    {
        userRepo.fromJson(readArrayFile(config.getUserRepoFile()));
        sessionRepo.fromJson(readArrayFile(config.getSessionRepoFile()));
        channelRepo.fromJson(readArrayFile(config.getChannelRepoFile()));
        messageRepo.fromJson(readArrayFile(config.getMessageRepoFile()));

        sessionManager.rebuildIndexFromRepository();
        channelManager.rebuildIndexFromRepository();
        messageManager.rebuildIndexFromRepositories();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Failed to load repository files: " << e.what() << std::endl;
    }
}

void Server::saveRepositories() const
{
    bool ok = true;
    ok = writeJsonFile(config.getUserRepoFile(), userRepo.toJson()) && ok;
    ok = writeJsonFile(config.getSessionRepoFile(), sessionRepo.toJson()) && ok;
    ok = writeJsonFile(config.getChannelRepoFile(), channelRepo.toJson()) && ok;
    ok = writeJsonFile(config.getMessageRepoFile(), messageRepo.toJson()) && ok;

    if (!ok)
    {
        std::cerr << "Failed to save one or more repository files." << std::endl;
    }
}
