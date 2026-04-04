#include <common/models/channel.hpp>

const Channel Channel::addMessageId(int messageId) const
{

    if (std::find(messageIds.begin(), messageIds.end(), messageId) != messageIds.end())
    {
        return *this; // Message ID already exists
    }
    Channel copy = *this;
    copy.messageIds.push_back(messageId);
    return copy;
}
const Channel Channel::addUserId(int userId) const
{
    Channel copy = *this;
    copy.userIds.insert(userId);
    return copy;
}
const Channel Channel::removeUserId(int userId) const
{
    Channel copy = *this;
    copy.userIds.erase(userId);
    return copy;
}
const Channel Channel::removeMessageId(int messageId) const
{
    auto it = std::find(messageIds.begin(), messageIds.end(), messageId);
    if (it == messageIds.end())
    {
        return *this;
    }
    Channel copy = *this;

    copy.messageIds.erase(std::remove(copy.messageIds.begin(), copy.messageIds.end(), messageId), copy.messageIds.end());
    return copy;
}
const bool Channel::operator==(const Channel &other) const
{
    return channelId == other.getChannelId();
}
const Channel Channel::withName(const std::string &newName) const
{
    Channel copy = *this;
    copy.name = newName;
    return copy;
}
const Channel Channel::toggleActive() const
{
    Channel copy = *this;
    copy.isActive = !copy.isActive;
    return copy;
}
