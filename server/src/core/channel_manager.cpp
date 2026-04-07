#include <server/core/channel_manager.hpp>

bool ChannelManager::createPublicChannel(const std::string &name, const std::vector<std::string> &userNames)
{
    Channel newChannel = Channel(
        nextChannelId++,
        name,
        std::vector<int>{},
        std::unordered_set<std::string>(userNames.begin(), userNames.end()), false);
    if (channelRepository.channelExists(newChannel.getChannelId()))
    {
        return false;
    }
    for (auto &userName : userNames)
    {
        channelMap[userName].push_back(newChannel.getChannelId());
    }
    return channelRepository.addChannel(newChannel);
}
bool ChannelManager::createPrivateConversation(const std::vector<std::string> &userNames)
{
    if (userNames.size() != 2)
    {
        return false;
    }
    for (const auto &userName : userNames)
    {
        if (!userRepository.userExists(userName))
        {
            return false;
        }
    }
    std::string channelName = "Private: " + userRepository.getUser(userNames[0]).getUsername() + " & " + userRepository.getUser(userNames[1]).getUsername();
    Channel newChannel = Channel(
        nextChannelId++,
        channelName,
        std::vector<int>{},
        std::unordered_set<std::string>(userNames.begin(), userNames.end()), true);
    if (channelRepository.channelExists(newChannel.getChannelId()))
    {
        return false;
    }
    for (auto &userName : userNames)
    {
        channelMap[userName].push_back(newChannel.getChannelId());
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
bool ChannelManager::removeUserFromChannel(int channelId, const std::string &userName)
{
    if (!channelRepository.channelExists(channelId))
    {
        return false;
    }
    Channel channel = channelRepository.getChannel(channelId);
    if (channel.getUserIds().find(userName) == channel.getUserIds().end())
    {
        return false;
    }
    Channel updatedChannel = channel.removeUserId(userName);
    auto &channels = channelMap[userName];
    channels.erase(std::remove(channels.begin(), channels.end(), channelId), channels.end());
    return channelRepository.updateChannel(updatedChannel);
}
bool ChannelManager::addUserToChannel(int channelId, const std::string &userName)
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
    if (channel.getUserIds().find(userName) != channel.getUserIds().end())
    {
        return false;
    }
    Channel updatedChannel = channel.addUserId(userName);
    channelMap[userName].push_back(channelId);
    return channelRepository.updateChannel(updatedChannel);
}
bool ChannelManager::editChannelName(int channelId, const std::string &newName)
{
    if (!channelRepository.channelExists(channelId))
    {
        return false;
    }
    Channel channel = channelRepository.getChannel(channelId);
    Channel updatedChannel = channel.withName(newName);
    return channelRepository.updateChannel(updatedChannel);
}
std::vector<Channel> ChannelManager::getUserActiveChannels(const std::string &userName)
{
    std::vector<Channel> activeChannels;
    for (int channelId : channelMap[userName])
    {
        Channel channel = channelRepository.getChannel(channelId);
        if (channel.getIsActive())
        {
            activeChannels.push_back(channel);
        }
    }
    return activeChannels;
}
std::vector<Channel> ChannelManager::getAllUserChannels(const std::string &userName)
{
    std::vector<Channel> allChannels;
    for (int channelId : channelMap[userName])
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
    return channelRepository.updateChannel(updatedChannel);
}
