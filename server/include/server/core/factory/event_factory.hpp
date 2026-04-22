#pragma once
#include <memory>
#include <server/auth/registration_event.hpp>
#include <server/auth/authentication_event.hpp>
#include <server/core/message_event.hpp>
#include <server/core/events/add_message_event.hpp>
#include <server/core/events/add_user_to_channel_event.hpp>
#include <server/core/events/create_channel_event.hpp>
#include <server/core/events/edit_channel_event.hpp>
#include <server/core/events/edit_message_event.hpp>
#include <server/core/events/remove_channel_event.hpp>
#include <server/core/events/remove_message_event.hpp>
#include <server/core/events/save_data_event.hpp>
#include <server/core/events/synchronize_data_event.hpp>
#include <server/core/events/invalid_event.hpp>
#include <common/const/event_messages.hpp>
class EventFactory
{
public:
    EventFactory() = default;
    static std::unique_ptr<Event> createEvent(const json &eventData);
};