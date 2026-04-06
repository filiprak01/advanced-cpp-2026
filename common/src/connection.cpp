#include <common/models/connection.hpp>

bool Connection::operator==(const Connection &other) const
{
    return socket == other.getSocket();
}