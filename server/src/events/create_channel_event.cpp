#include <server/core/events/create_channel_event.hpp>

void CreateChannelEvent::perform(ManagerContext &context)
{
    if (isPrivate)
    {
        context.channelManager.createPrivateConversation(userNames);
    }
    else
    {
        context.channelManager.createPublicChannel(name, userNames);
    }
}
