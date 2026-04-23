#include <server/core/channel_manager.hpp>

#include <algorithm>
#include <unordered_set>

namespace
{
    bool isMember(const Channel &channel, const std::string &userName)
    {
        return channel.getUserIds().find(userName) != channel.getUserIds().end();
    }
} // namespace

DomainResult ChannelManager::createPublicChannel(const std::string &requestorName, const std::string &name, const std::vector<std::string> &userNames, Channel *createdChannel)
{
    if (requestorName.empty())
    {
        return DomainResult::domainError(errors::Code::unauthorized);
    }

    std::unordered_set<std::string> members(userNames.begin(), userNames.end());
    members.insert(requestorName);

    for (const auto &userName : members)
    {
        if (!userRepository.userExists(userName))
        {
            return DomainResult::domainError(errors::Code::user_not_found);
        }
    }

    Channel newChannel(
        nextChannelId++,
        name,
        std::vector<int>{},
        members,
        false);

    if (channelRepository.channelExists(newChannel.getChannelId()))
    {
        return DomainResult::domainError(errors::Code::forbidden);
    }

    for (const auto &userName : members)
    {
        channelMap[userName].push_back(newChannel.getChannelId());
    }

    if (!channelRepository.addChannel(newChannel))
    {
        return DomainResult::domainError(errors::Code::forbidden);
    }

    if (createdChannel != nullptr)
    {
        *createdChannel = newChannel;
    }

    return DomainResult::success(success::Code::channel_created);
}

DomainResult ChannelManager::createPrivateConversation(const std::string &requestorName, const std::vector<std::string> &userNames, Channel *createdChannel)
{
    if (requestorName.empty())
    {
        return DomainResult::domainError(errors::Code::unauthorized);
    }

    std::unordered_set<std::string> members(userNames.begin(), userNames.end());
    members.insert(requestorName);

    if (members.size() != 2)
    {
        return DomainResult::formatError(errors::Code::invalid_payload);
    }

    std::vector<std::string> orderedMembers(members.begin(), members.end());
    std::sort(orderedMembers.begin(), orderedMembers.end());

    for (const auto &userName : orderedMembers)
    {
        if (!userRepository.userExists(userName))
        {
            return DomainResult::domainError(errors::Code::user_not_found);
        }
    }

    std::string channelName = "Private: " +
                              userRepository.getUser(orderedMembers[0]).getUsername() +
                              " & " +
                              userRepository.getUser(orderedMembers[1]).getUsername();

    Channel newChannel(
        nextChannelId++,
        channelName,
        std::vector<int>{},
        members,
        true);

    if (channelRepository.channelExists(newChannel.getChannelId()))
    {
        return DomainResult::domainError(errors::Code::forbidden);
    }

    for (const auto &userName : members)
    {
        channelMap[userName].push_back(newChannel.getChannelId());
    }

    if (!channelRepository.addChannel(newChannel))
    {
        return DomainResult::domainError(errors::Code::forbidden);
    }

    if (createdChannel != nullptr)
    {
        *createdChannel = newChannel;
    }

    return DomainResult::success(success::Code::channel_created);
}

DomainResult ChannelManager::deleteChannel(const std::string &requestorName, int channelId)
{
    if (!channelRepository.channelExists(channelId))
    {
        return DomainResult::domainError(errors::Code::channel_not_found);
    }

    Channel channel = channelRepository.getChannel(channelId);
    if (!isMember(channel, requestorName))
    {
        return DomainResult::domainError(errors::Code::forbidden);
    }

    for (const auto &userName : channel.getUserIds())
    {
        auto entry = channelMap.find(userName);
        if (entry == channelMap.end())
        {
            continue;
        }

        auto &channels = entry->second;
        channels.erase(std::remove(channels.begin(), channels.end(), channelId), channels.end());
    }

    if (!channelRepository.removeChannel(channelId))
    {
        return DomainResult::domainError(errors::Code::forbidden);
    }

    return DomainResult::success(success::Code::channel_removed);
}

DomainResult ChannelManager::removeUserFromChannel(const std::string &requestorName, int channelId, const std::string &userName)
{
    if (!channelRepository.channelExists(channelId))
    {
        return DomainResult::domainError(errors::Code::channel_not_found);
    }

    Channel channel = channelRepository.getChannel(channelId);
    if (!isMember(channel, requestorName) || !isMember(channel, userName))
    {
        return DomainResult::domainError(errors::Code::forbidden);
    }

    Channel updatedChannel = channel.removeUserId(userName);
    auto &channels = channelMap[userName];
    channels.erase(std::remove(channels.begin(), channels.end(), channelId), channels.end());

    if (!channelRepository.updateChannel(updatedChannel))
    {
        return DomainResult::domainError(errors::Code::forbidden);
    }

    return DomainResult::success(success::Code::user_removed_from_channel);
}

DomainResult ChannelManager::addUserToChannel(const std::string &requestorName, int channelId, const std::string &userName, Channel *updatedChannel)
{
    if (!channelRepository.channelExists(channelId))
    {
        return DomainResult::domainError(errors::Code::channel_not_found);
    }

    Channel channel = channelRepository.getChannel(channelId);
    if (!isMember(channel, requestorName))
    {
        return DomainResult::domainError(errors::Code::forbidden);
    }

    if (!userRepository.userExists(userName))
    {
        return DomainResult::domainError(errors::Code::user_not_found);
    }

    if (channel.getUserIds().size() >= channel.getMaxUsers())
    {
        return DomainResult::domainError(errors::Code::channel_full);
    }

    if (isMember(channel, userName))
    {
        return DomainResult::domainError(errors::Code::user_already_in_channel);
    }

    Channel newChannel = channel.addUserId(userName);
    channelMap[userName].push_back(channelId);

    if (!channelRepository.updateChannel(newChannel))
    {
        return DomainResult::domainError(errors::Code::forbidden);
    }

    if (updatedChannel != nullptr)
    {
        *updatedChannel = newChannel;
    }

    return DomainResult::success(success::Code::user_joined_channel);
}

DomainResult ChannelManager::editChannelName(const std::string &requestorName, int channelId, const std::string &newName)
{
    if (!channelRepository.channelExists(channelId))
    {
        return DomainResult::domainError(errors::Code::channel_not_found);
    }

    Channel channel = channelRepository.getChannel(channelId);
    if (!isMember(channel, requestorName))
    {
        return DomainResult::domainError(errors::Code::forbidden);
    }

    Channel updatedChannel = channel.withName(newName);
    if (!channelRepository.updateChannel(updatedChannel))
    {
        return DomainResult::domainError(errors::Code::forbidden);
    }

    return DomainResult::success(success::Code::channel_edited);
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

void ChannelManager::rebuildIndexFromRepository()
{
    channelMap.clear();
    nextChannelId = 1;
    for (const auto &channel : channelRepository.getAllChannels())
    {
        nextChannelId = std::max(nextChannelId, channel.getChannelId() + 1);
        for (const auto &userName : channel.getUserIds())
        {
            channelMap[userName].push_back(channel.getChannelId());
        }
    }
}

DomainResult ChannelManager::deactivateChannel(int channelId)
{
    if (!channelRepository.channelExists(channelId))
    {
        return DomainResult::domainError(errors::Code::channel_not_found);
    }

    Channel channel = channelRepository.getChannel(channelId);
    if (!channel.getIsPrivate())
    {
        return DomainResult::success(success::Code::channel_deactivated);
    }

    Channel updatedChannel = channel.toggleActive();
    if (!channelRepository.updateChannel(updatedChannel))
    {
        return DomainResult::domainError(errors::Code::forbidden);
    }

    return DomainResult::success(success::Code::channel_deactivated);
}
std::optional<std::unordered_set<std::string>> ChannelManager::getChannelUsernames(int channelId) const
{
    if (!channelRepository.channelExists(channelId))
    {
        return std::nullopt;
    }
    return channelRepository.getChannel(channelId).getUserIds();
}

std::optional<Channel> ChannelManager::getChannel(int channelId) const
{
    if (!channelRepository.channelExists(channelId))
    {
        return std::nullopt;
    }
    return channelRepository.getChannel(channelId);
}
