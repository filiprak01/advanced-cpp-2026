#pragma once
#include <string>
#include <vector>
#include <unordered_set>
#include "user.hpp"
#include "message.hpp"
#include <nlohmann/json.hpp>
using json = nlohmann::json;

class Channel {
    public:
        Channel() = default;
        Channel(const std::string& channel_id);
        const std::string& getChannelId() const;
        void addUser(const User& user);
        void removeUser(const std::string& username);
        const std::unordered_set<User>& getUsers() const;
        void addMessage(const Message& message);
        const std::vector<Message>& getMessages() const;
        const std::vector<Message> getMessagesByUser(const std::string& username) const;
        bool removeMessageByHash(const std::size_t hash);
        json serializeByUser(const std::string& username) const;
        bool operator==(const Channel& other) const;
    private:
        std::string channel_id_;
        std::vector<Message> messages_;
        std::unordered_set<User> users_;
};
namespace std {
    template<>
    struct hash<Channel> {
        std::size_t operator()(const Channel& channel) const {
            return std::hash<std::string>{}(channel.getChannelId());
        }
    };
}