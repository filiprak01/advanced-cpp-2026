#include <common/models/channel.hpp>
#include <algorithm>

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
const Channel Channel::addUserId(const std::string &userName) const
{
    Channel copy = *this;
    copy.userIds.insert(userName);
    return copy;
}
const Channel Channel::removeUserId(const std::string &userName) const
{
    Channel copy = *this;
    copy.userIds.erase(userName);
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

json Channel::toJson() const
{
    json j;
    j["channelId"] = channelId;
    j["name"] = name;
    j["isPrivate"] = isPrivate;
    j["isActive"] = isActive;
    j["maxUsers"] = maxUsers;
    j["messageIds"] = messageIds;
    j["userIds"] = std::vector<std::string>(userIds.begin(), userIds.end());
    return j;
}

void Channel::fromJson(const json &j)
{
    channelId = j.at("channelId").get<int>();
    name = j.at("name").get<std::string>();
    isPrivate = j.value("isPrivate", false);
    isActive = j.value("isActive", true);
    maxUsers = j.value("maxUsers", isPrivate ? 2 : 32);
    messageIds = j.value("messageIds", std::vector<int>{});
    std::vector<std::string> userIdsVec = j.value("userIds", std::vector<std::string>{});
    userIds = std::unordered_set<std::string>(userIdsVec.begin(), userIdsVec.end());
}
