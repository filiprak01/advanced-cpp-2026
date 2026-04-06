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
    bool removeChannel(int channelId);
    Channel getChannel(int channelId);
    bool channelExists(int channelId) const;
    bool isChannelActive(int channelId) const;
    bool isChannelPrivate(int channelId) const;
    bool updateChannel(const Channel &newChanneld);

private:
    mutable std::shared_mutex mutex;
    std::unordered_map<int, Channel> channels;
};