#include <server/repos/channel_repo.hpp>

bool ChannelRepository::channelExists(int channelId) const
{
    std::shared_lock<std::shared_mutex> lock(mutex);
    return channels.find(channelId) != channels.end();
}
bool ChannelRepository::addChannel(const Channel &channel)
{
    std::unique_lock<std::shared_mutex> lock(mutex);
    channels[channel.getChannelId()] = channel;
    return true;
}
bool ChannelRepository::removeChannel(int channelId)
{
    std::unique_lock<std::shared_mutex> lock(mutex);
    channels.erase(channelId);
    return true;
}
Channel ChannelRepository::getChannel(int channelId)
{
    std::shared_lock<std::shared_mutex> lock(mutex);
    return channels[channelId];
}
bool ChannelRepository::isChannelActive(int channelId) const
{
    std::shared_lock<std::shared_mutex> lock(mutex);
    return channels.find(channelId)->second.getIsActive();
}
bool ChannelRepository::isChannelPrivate(int channelId) const
{
    std::shared_lock<std::shared_mutex> lock(mutex);
    return channels.find(channelId)->second.getIsPrivate();
}
bool ChannelRepository::updateChannel(const Channel &channel)
{
    std::unique_lock<std::shared_mutex> lock(mutex);
    channels[channel.getChannelId()] = channel;
    return true;
}