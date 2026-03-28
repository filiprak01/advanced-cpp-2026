#pragma once
#include <string>
#include <nlohmann/json.hpp>
#include <functional>
using json = nlohmann::json;

class Message {
public:
    Message() = default;
    Message(const std::string& content,
        const std::string& sender,
        const std::string& receiver,
        const std::string& timestamp,
        const std::string& channel_id);

    const std::string& getContent() const;
    const std::string& getSender() const;
    const std::string& getReceiver() const;
    const std::string& getTimestamp() const;
    const std::string& getChannelId() const;
    json serialize() const;
    std::size_t getHash() const;
    static Message deserialize(const json& j);
    bool operator==(const Message& other) const;
private:
    std::string content_;
    std::string sender_;
    std::string receiver_;
    std::string timestamp_;
    std::string channel_id_;
};
namespace std {
    template<>
    struct hash<Message> {
        std::size_t operator()(const Message& msg) const {
            return msg.getHash();
        }
    };
}