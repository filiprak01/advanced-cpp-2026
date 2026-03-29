#pragma once
#include <common/models/channel.hpp>
#include <unordered_map>
#include <shared_mutex>
#include <mutex>

class ChannelRepository
{
public:
    ChannelRepository() = default;
    bool addChannel(const Channel &channel);
    bool removeChannel(const int &channelId);
    Channel getChannel(const int &channelId);
    bool channelExists(const int &channelId) const;

private:
    mutable std::shared_mutex mutex;
    std::unordered_map<int, Channel> channels;
};