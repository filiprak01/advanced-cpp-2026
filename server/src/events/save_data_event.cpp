#include <server/core/events/save_data_event.hpp>
#include <fstream>

void SaveDataEvent::perform(ManagerContext &context, int clientFd)
{
    if (!context.sessionManager.hasSession(clientFd))
    {
        context.connectionManager.sendErrorMessage(clientFd, DomainResult::domainError(errors::Code::unauthorized));
        return;
    }

    context.sessionManager.updateUserSession(clientFd);
    std::ofstream file(filePath);
    if (file.is_open())
    {
        file << "{}";
        file.close();
        context.connectionManager.sendSuccessMessage(clientFd, "save_data_response", DomainResult::success(success::Code::data_saved));
        return;
    }

    context.connectionManager.sendErrorMessage(clientFd, DomainResult::domainError(errors::Code::data_save_failed));
}
