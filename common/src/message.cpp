#include <common/models/message.hpp>

bool Message::operator==(const Message &other) const
{
    return id == other.getId();
}
Message Message::withContent(const std::string &newContent) const
{
    Message copy = *this;
    copy.content = newContent;
    copy.timestamp = std::chrono::steady_clock::now();
    return copy;
}