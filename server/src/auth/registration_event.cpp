#include <server/auth/registration_event.hpp>

namespace
{
json usersPayload(const RegistrationManager &registrationManager)
{
    return {{"users", registrationManager.getRegisteredUsernames()}};
}
} // namespace

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
        const json payload = usersPayload(context.registrationManager);
        context.connectionManager.updateConnectionMapping(clientFd, username);
        if (!context.sessionManager.createSession(clientFd))
        {
            context.sessionManager.updateUserSession(clientFd);
        }

        context.connectionManager.sendSuccessMessage(
            clientFd,
            "register_response",
            result,
            {{"userName", username}, {"users", payload["users"]}});

        context.connectionManager.sendSuccessMessage(
            clientFd,
            "login_response",
            DomainResult::success(success::Code::user_logged_in),
            {{"userName", username}});

        for (const auto &connectionItem : context.connectionManager.getConnectionMap())
        {
            if (connectionItem.first == clientFd || connectionItem.second.empty())
            {
                continue;
            }

            context.connectionManager.sendSuccessMessage(
                connectionItem.first,
                "users_updated",
                result,
                payload);
        }
        return;
    }

    context.connectionManager.sendErrorMessage(clientFd, result);
}
