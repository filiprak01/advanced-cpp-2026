#pragma once
#include <string_view>

namespace errors
{
    enum class Code
    {
        unknown_event_type,
        invalid_payload,
        missing_field,
        invalid_channel_id,
        invalid_message_id,
        empty_content,
        empty_channel_name,
        empty_user_name,
        channel_not_found,
        message_not_found,
        user_not_found,
        user_already_exists,
        forbidden,
        channel_full,
        user_already_in_channel,
        unauthorized,
        data_save_failed
    };

    constexpr std::string_view toString(Code code)
    {
        switch (code)
        {
        case Code::unknown_event_type:
            return "Unknown event type";
        case Code::invalid_payload:
            return "Invalid payload";
        case Code::missing_field:
            return "Missing field";
        case Code::invalid_channel_id:
            return "Invalid channel ID";
        case Code::invalid_message_id:
            return "Invalid message ID";
        case Code::empty_content:
            return "Empty content";
        case Code::empty_channel_name:
            return "Empty channel name";
        case Code::empty_user_name:
            return "Empty user name";
        case Code::channel_not_found:
            return "Channel not found";
        case Code::message_not_found:
            return "Message not found";
        case Code::user_not_found:
            return "User not found";
        case Code::user_already_exists:
            return "User already exists";
        case Code::forbidden:
            return "Forbidden";
        case Code::channel_full:
            return "Channel full";
        case Code::user_already_in_channel:
            return "User already in channel";
        case Code::unauthorized:
            return "Unauthorized";
        case Code::data_save_failed:
            return "Data save failed";
        }

        return "Unknown error";
    }
}

namespace success
{
    enum class Code
    {
        message_added,
        user_logged_in,
        user_registered,
        message_removed,
        message_edited,
        channel_created,
        channel_removed,
        user_joined_channel,
        user_removed_from_channel,
        channel_edited,
        channel_deactivated,
        data_saved,
        synchronized
    };

    constexpr std::string_view toString(Code code)
    {
        switch (code)
        {
        case Code::message_added:
            return "Message added";
        case Code::user_logged_in:
            return "User logged in";
        case Code::user_registered:
            return "User registered";
        case Code::message_removed:
            return "Message removed";
        case Code::message_edited:
            return "Message edited";
        case Code::channel_created:
            return "Channel created";
        case Code::channel_removed:
            return "Channel removed";
        case Code::user_joined_channel:
            return "User joined channel";
        case Code::user_removed_from_channel:
            return "User removed from channel";
        case Code::channel_edited:
            return "Channel edited";
        case Code::channel_deactivated:
            return "Channel deactivated";
        case Code::data_saved:
            return "Data saved";
        case Code::synchronized:
            return "Synchronized";
        }

        return "Unknown success";
    }
}
