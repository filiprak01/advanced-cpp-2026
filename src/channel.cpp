#include "channel.hpp"

Channel::Channel(const std::string& channel_id) : channel_id_(channel_id) {}
const std::string& Channel::getChannelId() const
{
    return channel_id_;
}
void Channel::addUser(const User& user)
{
    users_.insert(user);
}
void Channel::removeUser(const std::string& username)
{
    auto it = std::find_if(users_.begin(), users_.end(),
                           [&username](const User& user) { return user.getUsername() == username; });
    if (it != users_.end()) {
        users_.erase(it);
    }
}
const std::unordered_set<User>& Channel::getUsers() const
{
    return users_;
}
void Channel::addMessage(const Message& message)
{
    messages_.push_back(message);
}
const std::vector<Message>& Channel::getMessages() const
{
    return messages_;
}
const std::vector<Message> Channel::getMessagesByUser(const std::string& username) const
{
    std::vector<Message> userMessages;
    for (const auto& msg : messages_) {
        if (msg.getSender() == username) {
            userMessages.push_back(msg);
        }
    }
    return userMessages;
}
bool Channel::removeMessageByHash(const std::size_t hash)
{
    auto it = std::remove_if(messages_.begin(), messages_.end(),
                             [hash](const Message& msg) { return msg.getHash() == hash; });
    if (it != messages_.end()) {
        messages_.erase(it, messages_.end());
        return true;
    }
    return false;
}
json Channel::serializeByUser(const std::string& username) const
{   json j;
    auto messages = getMessagesByUser(username);
    auto jsons = json::array();
    for (const auto& msg : messages) {
        jsons.push_back(msg.serialize());
    }
    j["messages"] = jsons;
    return j;
}
bool Channel::operator==(const Channel& other) const
{
    return channel_id_ == other.channel_id_;
}