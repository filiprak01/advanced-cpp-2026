#include <common/repos/message_repo.hpp>
bool MessageRepository::messageExists(const int &messageId) const
{
    std::shared_lock<std::shared_mutex> lock(mutex);
    return messages.find(messageId) != messages.end();
}
bool MessageRepository::addMessage(const Message &message)
{
    std::unique_lock<std::shared_mutex> lock(mutex);
    messages[message.getId()] = message;
    return true;
}
bool MessageRepository::removeMessage(const int &messageId)
{
    std::unique_lock<std::shared_mutex> lock(mutex);
    messages.erase(messageId);
    return true;
}
Message MessageRepository::getMessage(const int &messageId)
{
    std::shared_lock<std::shared_mutex> lock(mutex);
    return messages[messageId];
}
