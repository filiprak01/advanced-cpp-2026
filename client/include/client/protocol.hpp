#pragma once

#include <nlohmann/json.hpp>
#include <string>
#include <vector>

using json = nlohmann::json;

/**
 * @brief Free functions that build client-to-server JSON request objects.
 *
 * Each function returns a well-formed JSON message matching the project
 * protocol. Pass the result to ClientBackend::send().
 */
namespace protocol
{
    inline json buildLoginRequest(const std::string &username, const std::string &password)
    {
        return {{"type", "login"}, {"payload", {{"username", username}, {"password", password}}}};
    }

    inline json buildRegisterRequest(const std::string &username, const std::string &password)
    {
        return {{"type", "register"}, {"payload", {{"username", username}, {"password", password}}}};
    }

    inline json buildAddMessageRequest(const std::string &senderName, int channelId, const std::string &content)
    {
        return {
            {"type", "add_message"},
            {"payload", {{"senderName", senderName}, {"channelId", channelId}, {"content", content}}}};
    }

    inline json buildEditMessageRequest(int messageId, const std::string &newContent)
    {
        return {{"type", "edit_message"}, {"payload", {{"messageId", messageId}, {"newContent", newContent}}}};
    }

    inline json buildRemoveMessageRequest(int messageId, int channelId)
    {
        return {{"type", "remove_message"}, {"payload", {{"messageId", messageId}, {"channelId", channelId}}}};
    }

    inline json buildCreateChannelRequest(const std::string &name,
                                          const std::vector<std::string> &userNames,
                                          bool isPrivate)
    {
        return {
            {"type", "create_channel"},
            {"payload", {{"name", name}, {"userNames", userNames}, {"isPrivate", isPrivate}}}};
    }

    inline json buildEditChannelRequest(int channelId, const std::string &newName)
    {
        return {{"type", "edit_channel"}, {"payload", {{"channelId", channelId}, {"newName", newName}}}};
    }

    inline json buildRemoveChannelRequest(int channelId)
    {
        return {{"type", "remove_channel"}, {"payload", {{"channelId", channelId}}}};
    }

    inline json buildAddUserToChannelRequest(int channelId, const std::string &userName)
    {
        return {{"type", "add_user_to_channel"}, {"payload", {{"channelId", channelId}, {"userName", userName}}}};
    }

    inline json buildSynchronizeRequest(int clientFd)
    {
        return {{"type", "synchronize"}, {"payload", {{"clientFd", clientFd}}}};
    }
} // namespace protocol
