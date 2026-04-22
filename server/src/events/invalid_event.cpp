#include <server/core/events/invalid_event.hpp>

void InvalidEvent::perform(ManagerContext &context, int clientFd)
{
    context.connectionManager.sendErrorMessage(clientFd, result);
}
