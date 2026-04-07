#include <server/core/events/add_user_to_channel_event.hpp>

void AddUserToChannelEvent::perform(ManagerContext &context)
{
    context.channelManager.addUserToChannel(channelId, userName);
}
