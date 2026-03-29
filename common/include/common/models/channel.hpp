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
    Channel(const int &channelId) : channelId(channelId) {}
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
    const bool addMessageId(const int &messageId);
    const bool addUserId(const int &userId);
    const bool removeUserId(const int &userId);
    const bool removeMessageId(const int &messageId);
    const bool operator==(const Channel &other) const;

private:
    int channelId;
    std::vector<int> messageIds;
    std::unordered_set<int> userIds;
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