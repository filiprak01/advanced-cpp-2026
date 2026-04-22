#include <server/core/factory/event_factory.hpp>
#include <chrono>
#include <vector>
#include <string>

std::unique_ptr<Event> EventFactory::createEvent(const json &eventData)
{
    std::string type = eventData.value("type", "");
    json payload = eventData.value("payload", json::object());

    if (!payload.is_object() || payload.empty())
    {
        return std::make_unique<InvalidEvent>(DomainResult::formatError(errors::Code::invalid_payload));
    }
    

    if (type == "login")
    {
        return std::make_unique<AuthenticationEvent>(payload.value("username", ""), payload.value("password", ""));
    }
    else if (type == "register")
    {
        return std::make_unique<RegistrationEvent>(payload.value("username", ""), payload.value("password", ""));
    }
    else if (type == "message")
    {
        return std::make_unique<MessageEvent>(payload.value("senderName", ""), payload.value("channelId", 0), payload.value("content", ""));
    }
    else if (type == "add_message")
    {
        return std::make_unique<AddMessageEvent>(
            payload.value("senderName", ""),
            payload.value("channelId", 0),
            payload.value("content", ""),
            std::chrono::steady_clock::now());
    }
    else if (type == "edit_message")
    {
        return std::make_unique<EditMessageEvent>(
            payload.value("messageId", 0),
            payload.value("newContent", ""));
    }
    else if (type == "remove_message")
    {
        return std::make_unique<RemoveMessageEvent>(
            payload.value("messageId", 0),
            payload.value("channelId", 0));
    }
    else if (type == "create_channel")
    {
        return std::make_unique<CreateChannelEvent>(
            payload.value("name", ""),
            payload.value("userNames", std::vector<std::string>{}),
            payload.value("isPrivate", false));
    }
    else if (type == "create_private_channel")
    {
        return std::make_unique<CreateChannelEvent>(
            "",
            payload.value("userNames", std::vector<std::string>{}),
            true);
    }
    else if (type == "edit_channel")
    {
        return std::make_unique<EditChannelEvent>(
            payload.value("channelId", 0),
            payload.value("newName", ""));
    }
    else if (type == "remove_channel")
    {
        return std::make_unique<RemoveChannelEvent>(payload.value("channelId", 0));
    }
    else if (type == "add_user_to_channel")
    {
        return std::make_unique<AddUserToChannelEvent>(
            payload.value("channelId", 0),
            payload.value("userName", ""));
    }
    else if (type == "synchronize")
    {
        return std::make_unique<SynchronizeDataEvent>();
    }
    else if (type == "save_data")
    {
        return std::make_unique<SaveDataEvent>(payload.value("filePath", ""));
    }
    return std::make_unique<InvalidEvent>(DomainResult::formatError(errors::Code::unknown_event_type));
}
