#include <common/repos/message_repo.hpp>
bool MessageRepository::messageExists(int messageId)
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
bool MessageRepository::removeMessage(int messageId)
{
    std::unique_lock<std::shared_mutex> lock(mutex);
    messages.erase(messageId);
    return true;
}
Message MessageRepository::getMessage(int messageId)
{
    std::shared_lock<std::shared_mutex> lock(mutex);
    return messages[messageId];
}
bool MessageRepository::updateMessage(const Message &message)
{
    std::unique_lock<std::shared_mutex> lock(mutex);
    messages[message.getId()] = message;
    return true;
}

std::vector<Message> MessageRepository::getAllMessages() const
{
    std::vector<Message> result;
    std::shared_lock<std::shared_mutex> lock(mutex);
    for (const auto &entry : messages)
    {
        result.push_back(entry.second);
    }
    return result;
}

json MessageRepository::toJson() const
{
    json result = json::array();
    std::shared_lock<std::shared_mutex> lock(mutex);
    for (const auto &entry : messages)
    {
        result.push_back(entry.second.toJson());
    }
    return result;
}

void MessageRepository::fromJson(const json &j)
{
    std::unique_lock<std::shared_mutex> lock(mutex);
    messages.clear();
    for (const auto &item : j)
    {
        Message message;
        message.fromJson(item);
        messages[message.getId()] = message;
    }
}
