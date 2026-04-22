#include <server/core/events/edit_message_event.hpp>

void EditMessageEvent::perform(ManagerContext &context, int clientFd)
{
    if (messageId < 1)
    {
        context.connectionManager.sendErrorMessage(clientFd, DomainResult::formatError(errors::Code::invalid_message_id));
        return;
    }
    if (newContent.empty())
    {
        context.connectionManager.sendErrorMessage(clientFd, DomainResult::formatError(errors::Code::empty_content));
        return;
    }
    if (!context.sessionManager.hasSession(clientFd))
    {
        context.connectionManager.sendErrorMessage(clientFd, DomainResult::domainError(errors::Code::unauthorized));
        return;
    }

    context.sessionManager.updateUserSession(clientFd);
    const std::string requestorName = context.connectionManager.getUsernameFromFd(clientFd);
    DomainResult result = context.messageManager.editMessage(requestorName, messageId, newContent);
    if (result.isSuccess())
    {
        int channelId = context.messageManager.getChannelIdFromMessage(messageId).value();
        std::unordered_set<std::string> channelUsers = context.channelManager.getChannelUsernames(channelId).value();
        json payload = {
            {"messageId", messageId},
            {"newContent", newContent}};
        context.connectionManager.sendSuccessMessage(clientFd, "message_edited", result, payload);
        for (const auto &connectionItem : context.connectionManager.getConnectionMap())
        {
            if (connectionItem.first == clientFd)
            {
                continue;
            }
            if (channelUsers.find(connectionItem.second) != channelUsers.end())
            {
                context.connectionManager.sendSuccessMessage(connectionItem.first, "message_edited", result, payload);
            }
        }
        return;
    }

    context.connectionManager.sendErrorMessage(clientFd, result);
}
