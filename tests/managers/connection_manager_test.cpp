#include <gtest/gtest.h>
#include <server/core/connection_manager.hpp>

/// @brief Fixture dla testów ConnectionManager.
class ConnectionManagerTest : public ::testing::Test
{
};

/// @test acceptConnection akceptuje nowe połączenie.
TEST_F(ConnectionManagerTest, AcceptConnectionReturnsValidFd) {}

/// @test receiveMessage odbiera wiadomość jako JSON.
TEST_F(ConnectionManagerTest, ReceiveMessageReturnsJson) {}

/// @test sendMessage wysyła JSON do klienta.
TEST_F(ConnectionManagerTest, SendMessageSendsJson) {}

/// @test disconnect zamyka deskryptor gniazda.
TEST_F(ConnectionManagerTest, DisconnectClosesFd) {}
