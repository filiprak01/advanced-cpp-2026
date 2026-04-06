#pragma once
#include <common/repos/message_repo.hpp>
#include <common/repos/user_repo.hpp>
#include <common/repos/channel_repo.hpp>
#include <unordered_map>
#include <optional>
class MessageManager
{
public:
    MessageManager(MessageRepository &messageRepo, UserRepository &userRepo, ChannelRepository &channelRepo, int nextId, const std::unordered_map<int, int> &messageMap) : messageRepository(messageRepo), userRepository(userRepo), channelRepository(channelRepo), nextMessageId(nextId), messageMap(messageMap) {}
    bool sendMessage(const std::string &content, std::string senderId, int channelId, const std::chrono::steady_clock::time_point &timestamp);
    bool deleteMessage(int messageId);
    bool editMessage(int messageId, const std::string &newContent);
    std::optional<std::vector<Message>> getChannelMessages(int channelId);

private:
    MessageRepository &messageRepository;
    UserRepository &userRepository;
    ChannelRepository &channelRepository;
    int nextMessageId;
    std::unordered_map<int, int> messageMap; // mesageId -> channelId
};