#pragma once
#include <common/models/message.hpp>
#include <unordered_map>
#include <shared_mutex>
#include <mutex>

class MessageRepository
{
public:
    MessageRepository() = default;
    bool addMessage(const Message &message);
    bool removeMessage(const int &messageId);
    Message getMessage(const int &messageId);
    bool messageExists(const int &messageId) const;

private:
    mutable std::shared_mutex mutex;
    std::unordered_map<int, Message> messages;
};