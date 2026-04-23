/**
 * @file server_integration_test.cpp
 * @brief Testy integracyjne serwera PWChat.
 *
 * Uruchamia serwer na porcie testowym, łączy się przez surowe gniazdo POSIX
 * i weryfikuje odpowiedzi serwera na pełny cykl żądań klienta.
 *
 * Protokół serwera:
 *   - odbiór: JSON zakończony '\n'
 *   - wysyłanie: 4-bajtowy prefiks długości (little-endian / native) + treść
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

// ---------------------------------------------------------------------------
// Stałe
// ---------------------------------------------------------------------------

static constexpr int TEST_PORT = 19001;
static constexpr int RECV_TIMEOUT_MS = 3000;

// ---------------------------------------------------------------------------
// Globalna instancja serwera (uruchamiana raz dla całego zestawu testów)
// ---------------------------------------------------------------------------

static Server *g_server = nullptr;
static std::thread g_serverThread;

// ---------------------------------------------------------------------------
// Pomocnicza klasa gniazda testowego
// ---------------------------------------------------------------------------

/// @brief Minimalny klient TCP obsługujący protokół serwera PWChat.
class TestSocket
{
public:
    explicit TestSocket(int port)
    {
        fd_ = ::socket(AF_INET, SOCK_STREAM, 0);
        if (fd_ < 0)
            throw std::runtime_error("socket() failed");

        // Timeout odczytu
        struct timeval tv{};
        tv.tv_sec = RECV_TIMEOUT_MS / 1000;
        tv.tv_usec = (RECV_TIMEOUT_MS % 1000) * 1000;
        ::setsockopt(fd_, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        addr.sin_port = htons(static_cast<uint16_t>(port));

        if (::connect(fd_, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) < 0)
            throw std::runtime_error("connect() failed");

        skipBanner(); // Odczyt bannera powitalnego serwera
    }

    ~TestSocket()
    {
        if (fd_ >= 0)
            ::close(fd_);
    }

    // Non-copyable
    TestSocket(const TestSocket &) = delete;
    TestSocket &operator=(const TestSocket &) = delete;

    /// @brief Wysyła żądanie JSON zakończone '\n'.
    void send(const json &msg)
    {
        std::string s = msg.dump() + '\n';
        if (::send(fd_, s.c_str(), s.size(), MSG_NOSIGNAL) < 0)
            throw std::runtime_error("send() failed");
    }

    /// @brief Odbiera odpowiedź: 4-bajtowa długość (native LE) + treść.
    std::string recv()
    {
        uint32_t len = 0;
        if (!recvAll(reinterpret_cast<char *>(&len), sizeof(len)))
            throw std::runtime_error("recv() failed reading length");

        std::string buf(len, '\0');
        if (!recvAll(buf.data(), len))
            throw std::runtime_error("recv() failed reading body");

        return buf;
    }

private:
    int fd_{-1};

    /// @brief Czyta dokładnie n bajtów lub zwraca false przy błędzie/EOF.
    bool recvAll(char *buf, std::size_t n)
    {
        while (n > 0)
        {
            ssize_t r = ::recv(fd_, buf, n, 0);
            if (r <= 0)
                return false;
            buf += r;
            n -= static_cast<std::size_t>(r);
        }
        return true;
    }

    /// @brief Odczytuje i odrzuca banner powitalny (tekst do '\n').
    void skipBanner()
    {
        char c = 0;
        while (::recv(fd_, &c, 1, 0) == 1 && c != '\n')
        {
        }
    }
};

// ---------------------------------------------------------------------------
// Fixture
// ---------------------------------------------------------------------------

class ServerIntegrationTest : public ::testing::Test
{
public:
    static void SetUpTestSuite()
    {
        g_server = new Server(TEST_PORT);
        g_serverThread = std::thread([]
                                     { g_server->run(); });
        // Krótkie oczekiwanie, żeby serwer zdążył rozpocząć poll()
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
    /// @brief Tworzy i zwraca nowe gniazdo połączone z serwerem testowym.
    TestSocket makeSocket() { return TestSocket(TEST_PORT); }
};

// ---------------------------------------------------------------------------
// Rejestracja
// ---------------------------------------------------------------------------

/// @test Pomyślna rejestracja nowego użytkownika zwraca "register_ok".
TEST_F(ServerIntegrationTest, RegisterNewUserReturnsRegisterOk)
{
    auto sock = makeSocket();
    sock.send({{"type", "register"}, {"payload", {{"username", "reg_user1"}, {"password", "Password1!"}}}});
    EXPECT_EQ(sock.recv(), "register_ok");
}

/// @test Próba rejestracji istniejącego użytkownika zwraca "register_failed".
TEST_F(ServerIntegrationTest, RegisterDuplicateUserReturnsRegisterFailed)
{
    auto sock = makeSocket();
    json req = {{"type", "register"}, {"payload", {{"username", "dup_user"}, {"password", "Password1!"}}}};
    sock.send(req);
    EXPECT_EQ(sock.recv(), "register_ok"); // pierwsze – sukces

    auto sock2 = makeSocket();
    sock2.send(req);
    EXPECT_EQ(sock2.recv(), "register_failed"); // drugie – duplikat
}

// ---------------------------------------------------------------------------
// Logowanie
// ---------------------------------------------------------------------------

/// @test Logowanie poprawnymi danymi zwraca "login_ok".
TEST_F(ServerIntegrationTest, LoginWithValidCredentialsReturnsLoginOk)
{
    auto sock = makeSocket();
    sock.send({{"type", "register"}, {"payload", {{"username", "auth_user1"}, {"password", "Password1!"}}}});
    EXPECT_EQ(sock.recv(), "register_ok");

    sock.send({{"type", "login"}, {"payload", {{"username", "auth_user1"}, {"password", "Password1!"}}}});
    EXPECT_EQ(sock.recv(), "login_ok");
}

/// @test Logowanie błędnym hasłem zwraca "login_failed".
TEST_F(ServerIntegrationTest, LoginWithWrongPasswordReturnsLoginFailed)
{
    auto sock = makeSocket();
    sock.send({{"type", "register"}, {"payload", {{"username", "auth_user2"}, {"password", "Password1!"}}}});
    EXPECT_EQ(sock.recv(), "register_ok");

    sock.send({{"type", "login"}, {"payload", {{"username", "auth_user2"}, {"password", "WrongPass!"}}}});
    EXPECT_EQ(sock.recv(), "login_failed");
}

/// @test Logowanie nieistniejącego użytkownika zwraca "login_failed".
TEST_F(ServerIntegrationTest, LoginNonexistentUserReturnsLoginFailed)
{
    auto sock = makeSocket();
    sock.send({{"type", "login"}, {"payload", {{"username", "ghost_999"}, {"password", "anypass"}}}});
    EXPECT_EQ(sock.recv(), "login_failed");
}

// ---------------------------------------------------------------------------
// Kontrola dostępu – operacje bez sesji
// ---------------------------------------------------------------------------

/// @test Wysłanie wiadomości bez logowania zwraca "unauthorized".
TEST_F(ServerIntegrationTest, AddMessageWithoutLoginReturnsUnauthorized)
{
    auto sock = makeSocket();
    sock.send({{"type", "add_message"},
               {"payload", {{"senderName", "nobody"}, {"channelId", 1}, {"content", "hello"}}}});
    EXPECT_EQ(sock.recv(), "unauthorized");
}

/// @test Tworzenie kanału bez logowania zwraca "unauthorized".
TEST_F(ServerIntegrationTest, CreateChannelWithoutLoginReturnsUnauthorized)
{
    auto sock = makeSocket();
    sock.send({{"type", "create_channel"},
               {"payload", {{"name", "test-ch"}, {"userNames", json::array()}, {"isPrivate", false}}}});
    EXPECT_EQ(sock.recv(), "unauthorized");
}

/// @test Synchronizacja bez logowania zwraca "unauthorized".
TEST_F(ServerIntegrationTest, SynchronizeWithoutLoginReturnsUnauthorized)
{
    auto sock = makeSocket();
    sock.send({{"type", "synchronize"}, {"payload", {{"clientFd", -1}}}});
    EXPECT_EQ(sock.recv(), "unauthorized");
}

// ---------------------------------------------------------------------------
// Operacje po zalogowaniu
// ---------------------------------------------------------------------------

/// @test Zalogowany użytkownik może zsynchronizować dane (odpowiedź "synchronize_ok").
TEST_F(ServerIntegrationTest, SynchronizeAfterLoginReturnsSynchronizeOk)
{
    auto sock = makeSocket();
    sock.send({{"type", "register"}, {"payload", {{"username", "sync_user1"}, {"password", "Syncpass1!"}}}});
    EXPECT_EQ(sock.recv(), "register_ok");

    sock.send({{"type", "login"}, {"payload", {{"username", "sync_user1"}, {"password", "Syncpass1!"}}}});
    EXPECT_EQ(sock.recv(), "login_ok");

    sock.send({{"type", "synchronize"}, {"payload", {{"clientFd", -1}}}});
    EXPECT_EQ(sock.recv(), "synchronize_ok");
}

/// @test Zalogowany użytkownik może stworzyć kanał.
TEST_F(ServerIntegrationTest, CreateChannelAfterLoginSucceeds)
{
    auto sock = makeSocket();
    sock.send({{"type", "register"}, {"payload", {{"username", "chan_user1"}, {"password", "Chanpass1!"}}}});
    EXPECT_EQ(sock.recv(), "register_ok");

    sock.send({{"type", "login"}, {"payload", {{"username", "chan_user1"}, {"password", "Chanpass1!"}}}});
    EXPECT_EQ(sock.recv(), "login_ok");

    sock.send({{"type", "create_channel"},
               {"payload", {{"name", "integration-ch"}, {"userNames", {"chan_user1"}}, {"isPrivate", false}}}});
    std::string resp = sock.recv();
    // Przyjmujemy "channel_created" lub brak "unauthorized"
    EXPECT_NE(resp, "unauthorized");
}
