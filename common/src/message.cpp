#include <common/messages/message.hpp>

Message::Message(const std::string& content, const std::string& sender, const std::string& receiver,
                 const std::string& timestamp, const std::string& channel_id)
    : content_(content), sender_(sender), receiver_(receiver), timestamp_(timestamp), channel_id_(channel_id)
{
}

const std::string& Message::getContent() const
{
    return content_;
}
const std::string& Message::getSender() const
{
    return sender_;
}
const std::string& Message::getReceiver() const
{
    return receiver_;
}
const std::string& Message::getTimestamp() const
{
    return timestamp_;
}
const std::string& Message::getChannelId() const
{
    return channel_id_;
}
json Message::serialize() const
{
    return json{{"content", content_},
                {"sender", sender_},
                {"receiver", receiver_},
                {"timestamp", timestamp_},
                {"channel_id", channel_id_}};
}
Message Message::deserialize(const json& j)
{
    Message msg;
    msg.content_ = j.at("content").get<std::string>();
    msg.sender_ = j.at("sender").get<std::string>();
    msg.receiver_ = j.at("receiver").get<std::string>();
    msg.timestamp_ = j.at("timestamp").get<std::string>();
    msg.channel_id_ = j.at("channel_id").get<std::string>();
    return msg;
}
std::size_t Message::getHash() const
{
    std::string combined = content_ + sender_ + receiver_ + timestamp_ + channel_id_;
    return std::hash<std::string>{}(combined);
}
bool Message::operator==(const Message& other) const
{
    return content_ == other.content_ &&
           sender_ == other.sender_ &&
           receiver_ == other.receiver_ &&
           timestamp_ == other.timestamp_ &&
           channel_id_ == other.channel_id_;
}