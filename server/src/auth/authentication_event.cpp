#include <server/auth/authentication_event.hpp>
#include <iostream>

void AuthenticationEvent::perform(ManagerContext &context)
{
    std::cout << "Performing authentication for user: " << username << std::endl;
    if (context.authenticationManager.authenticate(username, password))
    {
        std::cout << "Authentication successful for user: " << username << std::endl;
    }
    else
    {
        std::cout << "Authentication failed for user: " << username << std::endl;
    }
}