#include <common/models/message.hpp>

bool Message::operator==(const Message &other) const
{
    return id == other.getId();
}
Message Message::withContent(const std::string &newContent) const
{
    Message copy = *this;
    copy.content = newContent;
    copy.timestamp = std::chrono::steady_clock::now();
    return copy;
}

json Message::toJson() const
{
    json j;
    j["id"] = id;
    j["content"] = content;
    j["senderName"] = senderName;
    // steady_clock has no epoch anchor; store relative milliseconds since start
    j["timestampMs"] = std::chrono::duration_cast<std::chrono::milliseconds>(
                           timestamp.time_since_epoch())
                           .count();
    return j;
}

Message Message::fromJson(const json &j)
{
    int id = j.at("id").get<int>();
    std::string content = j.at("content").get<std::string>();
    std::string senderName = j.at("senderName").get<std::string>();
    long long ms = j.value("timestampMs", 0LL);
    auto timestamp = std::chrono::steady_clock::time_point(std::chrono::milliseconds(ms));
    return Message(id, content, senderName, timestamp);
}
