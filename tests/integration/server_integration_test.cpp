/**
 * @file server_integration_test.cpp
 * @brief Testy integracyjne serwera PWChat dla aktualnego protokolu MVP.
 *
 * Test uruchamia serwer na porcie testowym, laczy sie przez surowe gniazdo
 * POSIX i sprawdza odpowiedzi JSON przesylane protokolem:
 * 4-bajtowa dlugosc w network byte order + payload JSON.
 */
#include <gtest/gtest.h>
#include <server/server.hpp>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <chrono>
#include <stdexcept>
#include <string>
#include <thread>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

namespace
{
constexpr int TEST_PORT = 19001;
constexpr int RECV_TIMEOUT_MS = 3000;

Server *g_server = nullptr;
std::thread g_serverThread;

class TestSocket
{
public:
    explicit TestSocket(int port)
    {
        fd_ = ::socket(AF_INET, SOCK_STREAM, 0);
        if (fd_ < 0)
        {
            throw std::runtime_error("socket() failed");
        }

        timeval timeout{};
        timeout.tv_sec = RECV_TIMEOUT_MS / 1000;
        timeout.tv_usec = (RECV_TIMEOUT_MS % 1000) * 1000;
        ::setsockopt(fd_, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        addr.sin_port = htons(static_cast<uint16_t>(port));

        if (::connect(fd_, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) < 0)
        {
            throw std::runtime_error("connect() failed");
        }
    }

    ~TestSocket()
    {
        if (fd_ >= 0)
        {
            ::close(fd_);
        }
    }

    TestSocket(const TestSocket &) = delete;
    TestSocket &operator=(const TestSocket &) = delete;

    void sendJson(const json &message)
    {
        const std::string payload = message.dump();
        const uint32_t length = htonl(static_cast<uint32_t>(payload.size()));
        sendAll(reinterpret_cast<const char *>(&length), sizeof(length));
        sendAll(payload.data(), payload.size());
    }

    json recvJson()
    {
        uint32_t lengthNetwork = 0;
        if (!recvAll(reinterpret_cast<char *>(&lengthNetwork), sizeof(lengthNetwork)))
        {
            throw std::runtime_error("recv() failed reading length");
        }

        const uint32_t length = ntohl(lengthNetwork);
        std::string payload(length, '\0');
        if (!recvAll(payload.data(), length))
        {
            throw std::runtime_error("recv() failed reading body");
        }

        return json::parse(payload);
    }

private:
    int fd_{-1};

    void sendAll(const char *buffer, std::size_t length)
    {
        while (length > 0)
        {
            const ssize_t sent = ::send(fd_, buffer, length, MSG_NOSIGNAL);
            if (sent <= 0)
            {
                throw std::runtime_error("send() failed");
            }
            buffer += sent;
            length -= static_cast<std::size_t>(sent);
        }
    }

    bool recvAll(char *buffer, std::size_t length)
    {
        while (length > 0)
        {
            const ssize_t received = ::recv(fd_, buffer, length, 0);
            if (received <= 0)
            {
                return false;
            }
            buffer += received;
            length -= static_cast<std::size_t>(received);
        }
        return true;
    }
};

void expectOk(const json &response, const std::string &type)
{
    EXPECT_EQ(response.value("type", ""), type);
    EXPECT_EQ(response.value("status", ""), "ok");
}

void expectError(const json &response)
{
    EXPECT_EQ(response.value("type", ""), "error");
    EXPECT_EQ(response.value("status", ""), "error");
}
} // namespace

class ServerIntegrationTest : public ::testing::Test
{
public:
    static void SetUpTestSuite()
    {
        g_server = new Server(TEST_PORT);
        g_serverThread = std::thread([] { g_server->run(); });
        std::this_thread::sleep_for(std::chrono::milliseconds(150));
    }

    static void TearDownTestSuite()
    {
        g_server->stop();
        g_serverThread.join();
        delete g_server;
        g_server = nullptr;
    }

protected:
    TestSocket makeSocket()
    {
        return TestSocket(TEST_PORT);
    }
};

TEST_F(ServerIntegrationTest, RegisterNewUserReturnsRegisterOk)
{
    auto socket = makeSocket();
    socket.sendJson({{"type", "register"}, {"payload", {{"username", "reg_user1"}, {"password", "Password1!"}}}});

    expectOk(socket.recvJson(), "register_response");
    expectOk(socket.recvJson(), "login_response");
}

TEST_F(ServerIntegrationTest, RegisterDuplicateUserReturnsRegisterFailed)
{
    auto socket = makeSocket();
    const json request = {{"type", "register"}, {"payload", {{"username", "dup_user"}, {"password", "Password1!"}}}};

    socket.sendJson(request);
    expectOk(socket.recvJson(), "register_response");
    expectOk(socket.recvJson(), "login_response");

    auto secondSocket = makeSocket();
    secondSocket.sendJson(request);
    expectError(secondSocket.recvJson());
}

TEST_F(ServerIntegrationTest, LoginWithValidCredentialsReturnsLoginOk)
{
    auto socket = makeSocket();
    socket.sendJson({{"type", "register"}, {"payload", {{"username", "auth_user1"}, {"password", "Password1!"}}}});
    expectOk(socket.recvJson(), "register_response");
    expectOk(socket.recvJson(), "login_response");

    socket.sendJson({{"type", "login"}, {"payload", {{"username", "auth_user1"}, {"password", "Password1!"}}}});
    expectOk(socket.recvJson(), "login_response");
}

TEST_F(ServerIntegrationTest, LoginWithWrongPasswordReturnsLoginFailed)
{
    auto socket = makeSocket();
    socket.sendJson({{"type", "register"}, {"payload", {{"username", "auth_user2"}, {"password", "Password1!"}}}});
    expectOk(socket.recvJson(), "register_response");
    expectOk(socket.recvJson(), "login_response");

    socket.sendJson({{"type", "login"}, {"payload", {{"username", "auth_user2"}, {"password", "WrongPass!"}}}});
    expectError(socket.recvJson());
}

TEST_F(ServerIntegrationTest, LoginNonexistentUserReturnsLoginFailed)
{
    auto socket = makeSocket();
    socket.sendJson({{"type", "login"}, {"payload", {{"username", "ghost_999"}, {"password", "anypass"}}}});
    expectError(socket.recvJson());
}

TEST_F(ServerIntegrationTest, AddMessageWithoutLoginReturnsUnauthorized)
{
    auto socket = makeSocket();
    socket.sendJson({{"type", "add_message"},
                     {"payload", {{"senderName", "nobody"}, {"channelId", 1}, {"content", "hello"}}}});
    expectError(socket.recvJson());
}

TEST_F(ServerIntegrationTest, CreateChannelWithoutLoginReturnsUnauthorized)
{
    auto socket = makeSocket();
    socket.sendJson({{"type", "create_channel"},
                     {"payload", {{"name", "test-ch"}, {"userNames", json::array()}, {"isPrivate", false}}}});
    expectError(socket.recvJson());
}

TEST_F(ServerIntegrationTest, SynchronizeWithoutLoginReturnsUnauthorized)
{
    auto socket = makeSocket();
    socket.sendJson({{"type", "synchronize"}, {"payload", {{"clientFd", -1}}}});
    expectError(socket.recvJson());
}

TEST_F(ServerIntegrationTest, SynchronizeAfterLoginReturnsSynchronizeOk)
{
    auto socket = makeSocket();
    socket.sendJson({{"type", "register"}, {"payload", {{"username", "sync_user1"}, {"password", "Syncpass1!"}}}});
    expectOk(socket.recvJson(), "register_response");
    expectOk(socket.recvJson(), "login_response");

    socket.sendJson({{"type", "login"}, {"payload", {{"username", "sync_user1"}, {"password", "Syncpass1!"}}}});
    expectOk(socket.recvJson(), "login_response");

    socket.sendJson({{"type", "synchronize"}, {"payload", {{"clientFd", -1}}}});
    expectOk(socket.recvJson(), "sync_response");
}

TEST_F(ServerIntegrationTest, CreateChannelAfterLoginSucceeds)
{
    auto socket = makeSocket();
    socket.sendJson({{"type", "register"}, {"payload", {{"username", "chan_user1"}, {"password", "Chanpass1!"}}}});
    expectOk(socket.recvJson(), "register_response");
    expectOk(socket.recvJson(), "login_response");

    socket.sendJson({{"type", "login"}, {"payload", {{"username", "chan_user1"}, {"password", "Chanpass1!"}}}});
    expectOk(socket.recvJson(), "login_response");

    socket.sendJson({{"type", "create_channel"},
                     {"payload", {{"name", "integration-ch"}, {"userNames", {"chan_user1"}}, {"isPrivate", false}}}});
    expectOk(socket.recvJson(), "channel_created");
}
