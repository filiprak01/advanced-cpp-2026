#pragma once
#include <server/repos/channel_repo.hpp>
#include <unordered_map>
#include <vector>
class ChannelManager
{
public:
    ChannelManager(ChannelRepository &channelRepository) : channelRepository(channelRepository), nextChannelId(1) {}
    bool createPublicChannel(const std::string &name, const std::vector<int> &userIds);
    bool createPrivateConversation(const std::vector<int> &userIds);
    bool deleteChannel(int channelId);
    bool removeUserFromChannel(int channelId, int userId);
    bool addUserToChannel(int channelId, int userId);
    bool editChannelName(int channelId, const std::string &newName);

    std::vector<Channel> getUserActiveChannels(int userId);
    std::vector<Channel> getAllUserChannels(int userId);
    bool deactivateChannel(int channelId);

private:
    int nextChannelId;
    ChannelRepository &channelRepository;
    std::unordered_map<int, std::vector<int>> channelMap; // map user id into vector of active channels
};