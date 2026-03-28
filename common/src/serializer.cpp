#include <common/utilities/serializer.hpp>
#include <common/messages/message.hpp>

json Serializer::serialize(const Message &message)
{
    json j;
    j["content"] = message.getContent();
    j["sender"] = message.getSender();
    j["receiver"] = message.getReceiver();
    j["timestamp"] = message.getTimestamp();
    j["channel_id"] = message.getChannelId();
    return j;
}
Message Serializer::deserialize(const json &j)
{
    std::string content = j.at("content").get<std::string>();
    std::string sender = j.at("sender").get<std::string>();
    std::string receiver = j.at("receiver").get<std::string>();
    std::string timestamp = j.at("timestamp").get<std::string>();
    std::string channel_id = j.at("channel_id").get<std::string>();
    return Message(content, sender, receiver, timestamp, channel_id);
}