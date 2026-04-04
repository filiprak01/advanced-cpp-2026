#include <server/core/channel_manager.hpp>

bool ChannelManager::createPublicChannel(const std::string &name, const std::vector<int> &userIds)
{
    Channel newChannel = Channel(
        nextChannelId++,
        name,
        std::vector<int>{},
        std::unordered_set<int>(userIds.begin(), userIds.end()), false);
    if (channelRepository.channelExists(newChannel.getChannelId()))
    {
        return false;
    }
    for (auto &userId : userIds)
    {
        channelMap[userId].push_back(newChannel.getChannelId());
    }
    return channelRepository.addChannel(newChannel);
}
bool ChannelManager::createPrivateConversation(const std::vector<int> &userIds)
{
    if (userIds.size() != 2)
    {
        return false;
    }
    Channel newChannel = Channel(
        nextChannelId++,
        "",
        std::vector<int>{},
        std::unordered_set<int>(userIds.begin(), userIds.end()), true);
    if (channelRepository.channelExists(newChannel.getChannelId()))
    {
        return false;
    }
    for (auto &userId : userIds)
    {
        channelMap[userId].push_back(newChannel.getChannelId());
    }
    return channelRepository.addChannel(newChannel);
}
bool ChannelManager::deleteChannel(int channelId)
{
    if (!channelRepository.channelExists(channelId))
    {
        return false;
    }
    for (auto &entry : channelMap)
    {
        auto &channels = entry.second;
        channels.erase(std::remove(channels.begin(), channels.end(), channelId), channels.end());
    }
    return channelRepository.removeChannel(channelId);
}
bool ChannelManager::removeUserFromChannel(int channelId, int userId)
{
    if (!channelRepository.channelExists(channelId))
    {
        return false;
    }
    Channel channel = channelRepository.getChannel(channelId);
    if (channel.getUserIds().find(userId) == channel.getUserIds().end())
    {
        return false;
    }
    Channel updatedChannel = channel.removeUserId(userId);
    auto &channels = channelMap[userId];
    channels.erase(std::remove(channels.begin(), channels.end(), channelId), channels.end());
    return channelRepository.updateChannel(updatedChannel, channelId);
}
bool ChannelManager::addUserToChannel(int channelId, int userId)
{
    if (!channelRepository.channelExists(channelId))
    {
        return false;
    }
    Channel channel = channelRepository.getChannel(channelId);
    if (channel.getUserIds().size() >= channel.getMaxUsers())
    {
        return false;
    }
    if (channel.getUserIds().find(userId) != channel.getUserIds().end())
    {
        return false;
    }
    Channel updatedChannel = channel.addUserId(userId);
    channelMap[userId].push_back(channelId);
    return channelRepository.updateChannel(updatedChannel, channelId);
}
bool ChannelManager::editChannelName(int channelId, const std::string &newName)
{
    if (!channelRepository.channelExists(channelId))
    {
        return false;
    }
    Channel channel = channelRepository.getChannel(channelId);
    Channel updatedChannel = channel.withName(newName);
    return channelRepository.updateChannel(updatedChannel, channelId);
}
std::vector<Channel> ChannelManager::getUserActiveChannels(int userId)
{
    std::vector<Channel> activeChannels;
    for (int channelId : channelMap[userId])
    {
        Channel channel = channelRepository.getChannel(channelId);
        if (channel.getIsActive())
        {
            activeChannels.push_back(channel);
        }
    }
    return activeChannels;
}
std::vector<Channel> ChannelManager::getAllUserChannels(int userId)
{
    std::vector<Channel> allChannels;
    for (int channelId : channelMap[userId])
    {
        allChannels.push_back(channelRepository.getChannel(channelId));
    }
    return allChannels;
}
bool ChannelManager::deactivateChannel(int channelId)
{
    if (!channelRepository.channelExists(channelId))
    {
        return false;
    }
    Channel channel = channelRepository.getChannel(channelId);
    if (!channel.getIsPrivate())
    {
        return true;
    }
    Channel updatedChannel = channel.toggleActive();
    return channelRepository.updateChannel(updatedChannel, channelId);
}
