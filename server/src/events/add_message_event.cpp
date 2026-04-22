#include <server/core/events/add_message_event.hpp>

void AddMessageEvent::perform(ManagerContext &context, int clientFd)
{
    if (channelId < 1)
    {
        context.connectionManager.sendErrorMessage(clientFd, DomainResult::formatError(errors::Code::invalid_channel_id));
        return;
    }
    if (content.empty())
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
    Message createdMessage;
    DomainResult result = context.messageManager.sendMessage(content, clientFd, channelId, timestamp, &createdMessage);
    if (result.isSuccess())
    {
        std::unordered_set<std::string> channelUsers = context.channelManager.getChannelUsernames(channelId).value();
        json payload = createdMessage.toJson();
        payload["messageId"] = createdMessage.getId();
        payload["channelId"] = channelId;
        context.connectionManager.sendSuccessMessage(clientFd, "new_message", result, payload);
        for (const auto &connectionItem : context.connectionManager.getConnectionMap())
        {
            if (connectionItem.first == clientFd)
            {
                continue;
            }
            if (channelUsers.find(connectionItem.second) != channelUsers.end())
            {
                context.connectionManager.sendSuccessMessage(connectionItem.first, "new_message", result, payload);
            }
        }
        return;
    }

    context.connectionManager.sendErrorMessage(clientFd, result);
}
