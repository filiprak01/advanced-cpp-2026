#include <common/models/message.hpp>

bool Message::operator==(const Message &other) const
{
    return id == other.getId();
}