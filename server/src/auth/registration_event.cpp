#include <server/auth/registration_event.hpp>

void RegistrationEvent::perform(ManagerContext &context, int clientFd)
{
    if (username.empty())
    {
        context.connectionManager.sendErrorMessage(clientFd, DomainResult::formatError(errors::Code::empty_user_name));
        return;
    }

    DomainResult result = context.registrationManager.registerUser(username, password);
    if (result.isSuccess())
    {
        context.connectionManager.sendSuccessMessage(clientFd, "register_response", result);
        return;
    }

    context.connectionManager.sendErrorMessage(clientFd, result);
}
