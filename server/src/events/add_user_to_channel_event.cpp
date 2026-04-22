#include <server/core/events/add_user_to_channel_event.hpp>

void AddUserToChannelEvent::perform(ManagerContext &context, int clientFd)
{
    if (channelId < 1)
    {
        context.connectionManager.sendErrorMessage(clientFd, DomainResult::formatError(errors::Code::invalid_channel_id));
        return;
    }
    if (userName.empty())
    {
        context.connectionManager.sendErrorMessage(clientFd, DomainResult::formatError(errors::Code::empty_user_name));
        return;
    }
    if (!context.sessionManager.hasSession(clientFd))
    {
        context.connectionManager.sendErrorMessage(clientFd, DomainResult::domainError(errors::Code::unauthorized));
        return;
    }

    context.sessionManager.updateUserSession(clientFd);
    const std::string requestorName = context.connectionManager.getUsernameFromFd(clientFd);
    Channel updatedChannel;
    DomainResult result = context.channelManager.addUserToChannel(requestorName, channelId, this->userName, &updatedChannel);
    if (result.isSuccess())
    {
        json payload = updatedChannel.toJson();
        payload["userName"] = this->userName;
        context.connectionManager.sendSuccessMessage(clientFd, "user_joined_channel", result, payload);
        for (const auto &connectionItem : context.connectionManager.getConnectionMap())
        {
            if (connectionItem.first == clientFd)
            {
                continue;
            }
            if (updatedChannel.getUserIds().find(connectionItem.second) != updatedChannel.getUserIds().end())
            {
                context.connectionManager.sendSuccessMessage(connectionItem.first, "user_joined_channel", result, payload);
            }
        }
        return;
    }

    context.connectionManager.sendErrorMessage(clientFd, result);
}
