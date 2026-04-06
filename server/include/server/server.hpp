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

class Server
{
public:
    explicit Server(int port);
    ~Server();
    void run();
    void stop();

private:
    static int createAndBindSocket(int port);

    int server_fd;

    // Repositories (owned)
    UserRepository userRepo;
    SessionRepository sessionRepo;
    ChannelRepository channelRepo;
    MessageRepository messageRepo;

    // Auth utility (owned)
    PasswordHasher passwordHasher;

    // Managers (owned, hold refs to repos above)
    ConnectionManager connectionManager;
    SessionManager sessionManager;
    ChannelManager channelManager;
    MessageManager messageManager;
    AuthManager authManager;
    RegistrationManager registrationManager;

    bool running;
};
