#include <common/repos/channel_repo.hpp>

bool ChannelRepository::channelExists(const int &channelId) const
{
    std::shared_lock<std::shared_mutex> lock(mutex);
    return channels.find(channelId) != channels.end();
}
bool ChannelRepository::addChannel(const Channel &channel)
{
    std::unique_lock<std::shared_mutex> lock(mutex);
    if (channelExists(channel.getId()))
    {
        return false;
    }
    channels[channel.getId()] = channel;
    return true;
}
bool ChannelRepository::removeChannel(const int &channelId)
{
    std::unique_lock<std::shared_mutex> lock(mutex);
    if (!channelExists(channelId))
    {
        return false;
    }
    channels.erase(channelId);
    return true;
}
Channel ChannelRepository::getChannel(const int &channelId)
{
    std::shared_lock<std::shared_mutex> lock(mutex);
    if (!channelExists(channelId))
    {
        return Channel();
    }
    return channels[channelId];
}