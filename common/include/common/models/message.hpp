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
        const std::string &ssenderName,
        const std::chrono::steady_clock::time_point &timestamp) : id(id), content(content), senderName(senderName), timestamp(timestamp) {}
    // getters for data members
    const int &getId() const
    {
        return id;
    }
    const std::string &getContent() const
    {
        return content;
    }
    const std::string &getSenderName() const
    {
        return senderName;
    }
    const std::chrono::steady_clock::time_point &getTimestamp() const
    {
        return timestamp;
    }
    bool operator==(const Message &other) const;
    Message withContent(const std::string &newContent) const;

private:
    int id;
    std::string content;
    std::string senderName;
    std::chrono::steady_clock::time_point timestamp;
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