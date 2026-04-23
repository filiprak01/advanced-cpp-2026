#include <server/core/message_manager.hpp>

#include <algorithm>

DomainResult MessageManager::sendMessage(const std::string &content, int fd, int channelId, const std::chrono::steady_clock::time_point &timestamp, Message *createdMessage)
{
    std::string username = connectionManager.getUsernameFromFd(fd);
    if (username == "")
    {
        return DomainResult::domainError(errors::Code::unauthorized);
    }
    if (!userRepository.userExists(username))
    {
        return DomainResult::domainError(errors::Code::user_not_found);
    }
    if (!channelRepository.channelExists(channelId))
    {
        return DomainResult::domainError(errors::Code::channel_not_found);
    }
    bool userInChannel = false;
    std::unordered_set<std::string> channelUserIds = channelRepository.getChannel(channelId).getUserIds();
    for (std::string channelUserId : channelUserIds)
    {
        if (channelUserId == username)
        {
            userInChannel = true;
        }
    }
    if (!userInChannel)
    {
        return DomainResult::domainError(errors::Code::forbidden);
    }
    Message message = Message(nextMessageId++, content, username, timestamp);
    if (messageRepository.messageExists(message.getId()))
    {
        return DomainResult::domainError(errors::Code::forbidden);
    }
    messageMap[message.getId()] = channelId;
    if (!messageRepository.addMessage(message))
    {
        return DomainResult::domainError(errors::Code::forbidden);
    }
    Channel channel = channelRepository.getChannel(channelId);
    channelRepository.updateChannel(channel.addMessageId(message.getId()));

    if (createdMessage != nullptr)
    {
        *createdMessage = message;
    }
    return DomainResult::success(success::Code::message_added);
}
DomainResult MessageManager::deleteMessage(std::string requestorName, int messageId)
{
    if (!messageRepository.messageExists(messageId))
    {
        return DomainResult::domainError(errors::Code::message_not_found);
    }
    Message message = messageRepository.getMessage(messageId);
    if (message.getSenderName() != requestorName)
    {
        return DomainResult::domainError(errors::Code::forbidden);
    }
    auto channelItem = messageMap.find(messageId);
    if (channelItem != messageMap.end() && channelRepository.channelExists(channelItem->second))
    {
        Channel channel = channelRepository.getChannel(channelItem->second);
        channelRepository.updateChannel(channel.removeMessageId(messageId));
    }
    messageMap.erase(messageId);
    if (!messageRepository.removeMessage(messageId))
    {
        return DomainResult::domainError(errors::Code::forbidden);
    }

    return DomainResult::success(success::Code::message_removed);
}
DomainResult MessageManager::editMessage(std::string requestorName, int messageId, const std::string &newContent)
{
    if (!messageRepository.messageExists(messageId))
    {
        return DomainResult::domainError(errors::Code::message_not_found);
    }
    Message message = messageRepository.getMessage(messageId);
    if (message.getSenderName() != requestorName)
    {
        return DomainResult::domainError(errors::Code::forbidden);
    }
    Message oldMessage = messageRepository.getMessage(messageId);
    Message newMessage = oldMessage.withContent(newContent);
    if (!messageRepository.updateMessage(newMessage))
    {
        return DomainResult::domainError(errors::Code::forbidden);
    }

    return DomainResult::success(success::Code::message_edited);
}
std::optional<std::vector<Message>> MessageManager::getChannelMessages(int channelId)
{
    std::vector<Message> ret;
    if (!channelRepository.channelExists(channelId))
    {
        return std::nullopt;
    }
    for (auto &messageMapEntry : messageMap)
    {
        if (messageMapEntry.second == channelId)
        {
            if (messageRepository.messageExists(messageMapEntry.first))
            {
                ret.push_back(messageRepository.getMessage(messageMapEntry.first));
            }
        }
    }
    return ret;
}
std::optional<int> MessageManager::getChannelIdFromMessage(int messageId) const
{
    if (!messageRepository.messageExists(messageId))
    {
        return std::nullopt;
    }
    auto messageItem = messageMap.find(messageId);
    if (messageItem == messageMap.end())
    {
        return std::nullopt;
    }
    return messageItem->second;
}

void MessageManager::rebuildIndexFromRepositories()
{
    messageMap.clear();
    nextMessageId = 1;

    for (const auto &message : messageRepository.getAllMessages())
    {
        nextMessageId = std::max(nextMessageId, message.getId() + 1);
    }

    for (const auto &channel : channelRepository.getAllChannels())
    {
        for (int messageId : channel.getMessageIds())
        {
            if (messageRepository.messageExists(messageId))
            {
                messageMap[messageId] = channel.getChannelId();
            }
        }
    }
}
