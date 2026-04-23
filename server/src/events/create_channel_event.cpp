#include <server/core/events/create_channel_event.hpp>

void CreateChannelEvent::perform(ManagerContext &context, int clientFd)
{
    if (!isPrivate && name.empty())
    {
        context.connectionManager.sendErrorMessage(clientFd, DomainResult::formatError(errors::Code::empty_channel_name));
        return;
    }
    for (const auto &userName : userNames)
    {
        if (userName.empty())
        {
            context.connectionManager.sendErrorMessage(clientFd, DomainResult::formatError(errors::Code::empty_user_name));
            return;
        }
    }
    if (!context.sessionManager.hasSession(clientFd))
    {
        context.connectionManager.sendErrorMessage(clientFd, DomainResult::domainError(errors::Code::unauthorized));
        return;
    }

    context.sessionManager.updateUserSession(clientFd);
    const std::string requestorName = context.connectionManager.getUsernameFromFd(clientFd);

    DomainResult result = DomainResult::formatError(errors::Code::invalid_payload);
    Channel createdChannel;
    if (isPrivate)
    {
        if (userNames.empty())
        {
            context.connectionManager.sendErrorMessage(clientFd, DomainResult::formatError(errors::Code::invalid_payload));
            return;
        }
        result = context.channelManager.createPrivateConversation(requestorName, userNames, &createdChannel);
    }
    else
    {
        result = context.channelManager.createPublicChannel(requestorName, name, userNames, &createdChannel);
    }

    if (result.isSuccess())
    {
        json payload = createdChannel.toJson();
        context.connectionManager.sendSuccessMessage(clientFd, "channel_created", result, payload);
        for (const auto &connectionItem : context.connectionManager.getConnectionMap())
        {
            if (connectionItem.first == clientFd)
            {
                continue;
            }
            if (createdChannel.getUserIds().find(connectionItem.second) != createdChannel.getUserIds().end())
            {
                context.connectionManager.sendSuccessMessage(connectionItem.first, "channel_created", result, payload);
            }
        }
        return;
    }

    context.connectionManager.sendErrorMessage(clientFd, result);
}
