#include <server/core/factory/event_factory.hpp>

std::unique_ptr<Event> EventFactory::createEvent(const json &eventData)
{
    std::string type = eventData.value("type", "");
    json payload = eventData.value("payload", json::object());

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
    return nullptr;
}