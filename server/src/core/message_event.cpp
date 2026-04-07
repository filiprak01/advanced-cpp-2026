#include <server/core/message_event.hpp>
#include <iostream>

void MessageEvent::perform(ManagerContext &context)
{
    std::cout << "Performing MessageEvent for channelId: " << channelId << " with content: " << content << std::endl;
    if (context.messageManager.sendMessage(content, senderName, channelId, timestamp))
    {
        std::cout << "Message sent successfully to channelId: " << channelId << std::endl;
    }
    else
    {
        std::cout << "Failed to send message to channelId: " << channelId << std::endl;
    }
}