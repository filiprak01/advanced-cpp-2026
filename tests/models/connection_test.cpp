#include <gtest/gtest.h>
#include <common/models/connection.hpp>

/// @brief Fixture dla testów klasy Connection.
class ConnectionTest : public ::testing::Test
{
};

/// @test getSocket zwraca ustawiony deskryptor gniazda.
TEST_F(ConnectionTest, GetSocketReturnsSocket) {}

/// @test getSession zwraca przypiąsaną sesję.
TEST_F(ConnectionTest, GetSessionReturnsSession) {}

/// @test operator== porównuje połączenia po gnieździe.
TEST_F(ConnectionTest, OperatorEqualBySocket) {}
