#include <server/core/events/edit_channel_event.hpp>

void EditChannelEvent::perform(ManagerContext &context, int clientFd)
{
    if (channelId < 1)
    {
        context.connectionManager.sendErrorMessage(clientFd, DomainResult::formatError(errors::Code::invalid_channel_id));
        return;
    }
    if (newName.empty())
    {
        context.connectionManager.sendErrorMessage(clientFd, DomainResult::formatError(errors::Code::empty_channel_name));
        return;
    }
    if (!context.sessionManager.hasSession(clientFd))
    {
        context.connectionManager.sendErrorMessage(clientFd, DomainResult::domainError(errors::Code::unauthorized));
        return;
    }

    context.sessionManager.updateUserSession(clientFd);
    const std::string requestorName = context.connectionManager.getUsernameFromFd(clientFd);
    DomainResult result = context.channelManager.editChannelName(requestorName, channelId, newName);
    if (result.isSuccess())
    {
        std::unordered_set<std::string> channelUsers = context.channelManager.getChannelUsernames(channelId).value();
        json payload = {{"channelId", channelId}, {"newName", newName}};
        context.connectionManager.sendSuccessMessage(clientFd, "channel_edited", result, payload);
        for (const auto &connectionItem : context.connectionManager.getConnectionMap())
        {
            if (connectionItem.first == clientFd)
            {
                continue;
            }
            if (channelUsers.find(connectionItem.second) != channelUsers.end())
            {
                context.connectionManager.sendSuccessMessage(connectionItem.first, "channel_edited", result, payload);
            }
        }
        return;
    }

    context.connectionManager.sendErrorMessage(clientFd, result);
}
