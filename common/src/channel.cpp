#include <common/models/channel.hpp>

const bool Channel::addMessageId(const int &messageId)
{
    if (std::find(messageIds.begin(), messageIds.end(), messageId) != messageIds.end())
    {
        return false; // Message ID already exists
    }
    messageIds.push_back(messageId);
    return true;
}
const bool Channel::addUserId(const int &userId)
{
    return userIds.insert(userId).second; // Returns true if insertion took place
}
const bool Channel::removeUserId(const int &userId)
{
    return userIds.erase(userId) > 0; // Returns true if an element was removed
}
const bool Channel::removeMessageId(const int &messageId)
{
    auto it = std::find(messageIds.begin(), messageIds.end(), messageId);
    if (it != messageIds.end())
    {
        messageIds.erase(it);
        return true; // Message ID removed
    }
    return false; // Message ID not found
}
const bool Channel::operator==(const Channel &other) const
{
    return channelId == other.getChannelId();
}