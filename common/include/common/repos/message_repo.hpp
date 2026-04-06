#pragma once
#include <common/models/message.hpp>
#include <unordered_map>
#include <shared_mutex>
#include <mutex>

class MessageRepository
{
    MessageRepository() = default;
public:
    bool addMessage(const Message &message);
    bool removeMessage(int messageId);
    Message getMessage(int messageId);
    bool messageExists(int messageId);
    bool updateMessage(const Message &message);

private:
    mutable std::shared_mutex mutex;
    std::unordered_map<int, Message> messages;
};