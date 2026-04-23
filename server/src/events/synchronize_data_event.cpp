#include <server/core/events/synchronize_data_event.hpp>

void SynchronizeDataEvent::perform(ManagerContext &context, int clientFd)
{
    if (!context.sessionManager.hasSession(clientFd))
    {
        context.connectionManager.sendErrorMessage(clientFd, DomainResult::domainError(errors::Code::unauthorized));
        return;
    }

    context.sessionManager.updateUserSession(clientFd);
    const std::string userName = context.connectionManager.getUsernameFromFd(clientFd);

    json channelsJson = json::array();
    for (const auto &channel : context.channelManager.getAllUserChannels(userName))
    {
        channelsJson.push_back(channel.toJson());
    }

    json messagesJson = json::array();
    for (const auto &channel : context.channelManager.getAllUserChannels(userName))
    {
        auto messageOpt = context.messageManager.getChannelMessages(channel.getChannelId());
        if (!messageOpt)
        {
            continue;
        }

        for (const auto &message : messageOpt.value())
        {
            messagesJson.push_back(message.toJson());
        }
    }

    context.connectionManager.sendSuccessMessage(
        clientFd,
        "sync_response",
        DomainResult::success(success::Code::synchronized),
        {{"channels", channelsJson}, {"messages", messagesJson}});
}
