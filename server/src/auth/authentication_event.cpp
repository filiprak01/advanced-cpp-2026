#include <server/auth/authentication_event.hpp>

void AuthenticationEvent::perform(ManagerContext &context, int clientFd)
{
    context.sessionManager.cleanInactiveSessions();
    if (username.empty())
    {
        context.connectionManager.sendErrorMessage(clientFd, DomainResult::formatError(errors::Code::empty_user_name));
        return;
    }

    DomainResult result = context.authenticationManager.authenticate(username, password);
    if (!result.isSuccess())
    {
        context.connectionManager.sendErrorMessage(clientFd, result);
        return;
    }

    context.connectionManager.updateConnectionMapping(clientFd, username);
    bool sessionCreated = context.sessionManager.createSession(clientFd);
    if (!sessionCreated)
    {
        context.sessionManager.updateUserSession(clientFd);
    }

    context.connectionManager.sendSuccessMessage(clientFd, "login_response", result, {{"userName", username}});
}
