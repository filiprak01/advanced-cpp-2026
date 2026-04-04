#pragma once
#include <string>
#include <unordered_set>
#include <vector>
#include <common/models/user.hpp>
#include <common/models/message.hpp>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

class Channel
{
public:
    Channel() = default;
    Channel(const int &channelId, const std::string &name, const std::vector<int> &messageIds, const std::unordered_set<int> &userIds, bool isPrivate) : channelId(channelId), name(name), messageIds(messageIds), userIds(userIds), isPrivate(isPrivate), maxUsers(isPrivate ? 2 : 32), isActive(true) {}
    const int &getChannelId() const
    {
        return channelId;
    }
    const std::vector<int> &getMessageIds() const
    {
        return messageIds;
    }
    const std::unordered_set<int> &getUserIds() const
    {
        return userIds;
    }
    const bool &getIsPrivate() const
    {
        return isPrivate;
    }
    const bool &getIsActive() const
    {
        return isActive;
    }
    const int &getMaxUsers() const
    {
        return maxUsers;
    }
    const Channel addMessageId(int messageId) const;
    const Channel addUserId(int userId) const;
    const Channel removeUserId(int userId) const;
    const Channel removeMessageId(int messageId) const;
    const Channel withName(const std::string &newName) const;
    const Channel toggleActive() const;
    const bool operator==(const Channel &other) const;

private:
    int channelId;
    bool isPrivate;
    bool isActive;
    std::string name;
    std::vector<int> messageIds;
    std::unordered_set<int> userIds;
    int maxUsers;
};
namespace std
{
    template <>
    struct hash<Channel>
    {
        std::size_t operator()(const Channel &channel) const
        {
            return std::hash<int>{}(channel.getChannelId());
        }
    };
}