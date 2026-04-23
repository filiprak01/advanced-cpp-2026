#pragma once
#include <server/core/domain_result.hpp>
#include <server/repos/channel_repo.hpp>
#include <server/repos/user_repo.hpp>
#include <unordered_map>
#include <vector>
#include <optional>
/**
 * @brief Menedżer kanałów komunikacyjnych serwera.
 *
 * Odpowiada za tworzenie, usuwanie oraz zarządzanie uczestnikami
 * kanałów publicznych i prywatnych.
 */
class ChannelManager
{
public:
    /**
     * @brief Tworzy menedżer kanałów z podanym repozytorium.
     * @param channelRepository Repozytorium kanałów.
     */
    ChannelManager(ChannelRepository &channelRepository, UserRepository &userRepository)
        : channelRepository(channelRepository), nextChannelId(1), userRepository(userRepository) {}

    DomainResult createPublicChannel(const std::string &requestorName, const std::string &name, const std::vector<std::string> &userNames, Channel *createdChannel = nullptr);
    DomainResult createPrivateConversation(const std::string &requestorName, const std::vector<std::string> &userNames, Channel *createdChannel = nullptr);
    DomainResult deleteChannel(const std::string &requestorName, int channelId);
    DomainResult removeUserFromChannel(const std::string &requestorName, int channelId, const std::string &userName);
    DomainResult addUserToChannel(const std::string &requestorName, int channelId, const std::string &userName, Channel *updatedChannel = nullptr);
    DomainResult editChannelName(const std::string &requestorName, int channelId, const std::string &newName);
    std::optional<std::unordered_set<std::string>> getChannelUsernames(int channelId) const;
    std::optional<Channel> getChannel(int channelId) const;
    std::vector<Channel> getUserActiveChannels(const std::string &userName);
    std::vector<Channel> getAllUserChannels(const std::string &userName);

    DomainResult deactivateChannel(int channelId);

private:
    int nextChannelId;
    ChannelRepository &channelRepository;
    UserRepository &userRepository;
    std::unordered_map<std::string, std::vector<int>> channelMap;
};
