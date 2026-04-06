#include <server/core/message_manager.hpp>

bool MessageManager::sendMessage(const std::string &content, std::string senderName, int channelId, const std::chrono::steady_clock::time_point &timestamp)
{
    if (!userRepository.userExists(senderName))
    {
        return false;
    }
    if (!channelRepository.channelExists(channelId))
    {
        return false;
    }
    Message message = Message(nextMessageId++, content, senderName, timestamp);
    if (messageRepository.messageExists(message.getId()))
    {
        return false;
    }
    messageMap[message.getId()] = channelId;
    return messageRepository.addMessage(message);
}
bool MessageManager::deleteMessage(int messageId)
{
    if (!messageRepository.messageExists(messageId))
    {
        return false;
    }
    messageMap.erase(messageId);
    return messageRepository.removeMessage(messageId);
}
bool MessageManager::editMessage(int messageId, const std::string &newContent)
{
    Message oldMessage = messageRepository.getMessage(messageId);
    Message newMessage = oldMessage.withContent(newContent);
    return messageRepository.updateMessage(newMessage);
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
