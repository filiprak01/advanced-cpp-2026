#pragma once
#include <string>
#include <nlohmann/json.hpp>
#include <functional>
using json = nlohmann::json;

class Message
{
public:
    Message() = default;
    Message(
        const int &id,
        const std::string &content,
        const std::string &sender,
        const int &receiverChannelId,
        const std::string &timestamp,
        const std::string &channel_id) : id(id), content(content), sender(sender), timestamp(timestamp), receiverChannelId(receiverChannelId) {}
    // getters for data members
    const int &getChannelId() const
    {
        return receiverChannelId;
    }
    const int &getId() const
    {
        return id;
    }
    const std::string &getContent() const
    {
        return content;
    }
    const std::string &getSender() const
    {
        return sender;
    }
    const std::string &getTimestamp() const
    {
        return timestamp;
    }
    bool operator==(const Message &other) const;

private:
    int id;
    std::string content;
    std::string sender;
    int receiverChannelId;
    std::string timestamp;
};
namespace std
{
    template <>
    struct hash<Message>
    {
        std::size_t operator()(const Message &msg) const
        {
            return std::hash<int>{}(msg.getId());
        }
    };
}