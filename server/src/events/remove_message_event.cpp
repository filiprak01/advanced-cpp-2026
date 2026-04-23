#include <server/core/events/remove_message_event.hpp>

void RemoveMessageEvent::perform(ManagerContext &context, int clientFd)
{
    if (messageId < 1)
    {
        context.connectionManager.sendErrorMessage(clientFd, DomainResult::formatError(errors::Code::invalid_message_id));
        return;
    }
    if (channelId < 1)
    {
        context.connectionManager.sendErrorMessage(clientFd, DomainResult::formatError(errors::Code::invalid_channel_id));
        return;
    }
    if (!context.sessionManager.hasSession(clientFd))
    {
        context.connectionManager.sendErrorMessage(clientFd, DomainResult::domainError(errors::Code::unauthorized));
        return;
    }

    context.sessionManager.updateUserSession(clientFd);
    const std::string requestorName = context.connectionManager.getUsernameFromFd(clientFd);
    DomainResult result = context.messageManager.deleteMessage(requestorName, messageId);
    if (result.isSuccess())
    {
        std::unordered_set<std::string> channelUsers = context.channelManager.getChannelUsernames(channelId).value();
        json payload = {
            {"messageId", messageId}};
        context.connectionManager.sendSuccessMessage(clientFd, "message_removed", result, payload);
        for (const auto &connectionItem : context.connectionManager.getConnectionMap())
        {
            if (connectionItem.first == clientFd)
            {
                continue;
            }
            if (channelUsers.find(connectionItem.second) != channelUsers.end())
            {
                context.connectionManager.sendSuccessMessage(connectionItem.first, "message_removed", result, payload);
            }
        }
        return;
    }

    context.connectionManager.sendErrorMessage(clientFd, result);
}
