#pragma once
#include <memory>
#include <server/auth/registration_event.hpp>
#include <server/auth/authentication_event.hpp>
#include <server/core/message_event.hpp>

class EventFactory
{
public:
    EventFactory() = default;
    static std::unique_ptr<Event> createEvent(const json &eventData);
};