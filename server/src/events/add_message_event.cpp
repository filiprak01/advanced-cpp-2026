#include <server/core/events/add_message_event.hpp>
#include <chrono>

void AddMessageEvent::perform(ManagerContext &context)
{
    context.messageManager.sendMessage(content, senderName, channelId, timestamp);
}
