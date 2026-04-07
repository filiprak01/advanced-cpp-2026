#include <server/auth/registration_event.hpp>
#include <iostream>

void RegistrationEvent::perform(ManagerContext &context)
{
    std::cout << "Performing registration for user: " << username << std::endl;
    if (context.registrationManager.registerUser(username, password))
    {
        std::cout << "Registration successful for user: " << username << std::endl;
    }
    else
    {
        std::cout << "Registration failed for user: " << username << std::endl;
    }
}