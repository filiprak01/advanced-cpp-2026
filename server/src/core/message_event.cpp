#include <server/core/message_event.hpp>

void MessageEvent::perform(ManagerContext &context, int clientFd)
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
    DomainResult result = context.messageManager.sendMessage(content, clientFd, channelId, std::chrono::steady_clock::now());
    if (result.isSuccess())
    {
        context.connectionManager.sendSuccessMessage(clientFd, "message_response", result);
        return;
    }

    context.connectionManager.sendErrorMessage(clientFd, result);
}
