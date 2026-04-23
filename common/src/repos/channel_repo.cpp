#include <common/repos/channel_repo.hpp>

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

std::vector<Channel> ChannelRepository::getAllChannels() const
{
    std::vector<Channel> result;
    std::shared_lock<std::shared_mutex> lock(mutex);
    for (const auto &entry : channels)
    {
        result.push_back(entry.second);
    }
    return result;
}

json ChannelRepository::toJson() const
{
    json result = json::array();
    std::shared_lock<std::shared_mutex> lock(mutex);
    for (const auto &entry : channels)
    {
        result.push_back(entry.second.toJson());
    }
    return result;
}

void ChannelRepository::fromJson(const json &j)
{
    std::unique_lock<std::shared_mutex> lock(mutex);
    channels.clear();
    for (const auto &item : j)
    {
        Channel channel;
        channel.fromJson(item);
        channels[channel.getChannelId()] = channel;
    }
}
