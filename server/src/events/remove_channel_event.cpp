#include <server/core/events/remove_channel_event.hpp>

void RemoveChannelEvent::perform(ManagerContext &context, int clientFd)
{
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
    std::optional<std::unordered_set<std::string>> channelUsers = context.channelManager.getChannelUsernames(channelId);
    DomainResult result = context.channelManager.deleteChannel(requestorName, channelId);
    if (result.isSuccess())
    {
        json payload = {{"channelId", channelId}};
        context.connectionManager.sendSuccessMessage(clientFd, "channel_removed", result, payload);
        if (channelUsers.has_value())
        {
            for (const auto &connectionItem : context.connectionManager.getConnectionMap())
            {
                if (connectionItem.first == clientFd)
                {
                    continue;
                }
                if (channelUsers->find(connectionItem.second) != channelUsers->end())
                {
                    context.connectionManager.sendSuccessMessage(connectionItem.first, "channel_removed", result, payload);
                }
            }
        }
        return;
    }

    context.connectionManager.sendErrorMessage(clientFd, result);
}
