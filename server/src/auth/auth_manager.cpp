#include <server/auth/auth_manager.hpp>
#include <iostream>
bool AuthManager::authenticate(const std::string &username, const std::string &password)
{
    User userOpt = userRepository.getUser(username);
    if (userOpt.getUsername() == "")
    {
        return false;
    }
    return verifyPassword(password, userOpt);
}
bool AuthManager::verifyPassword(const std::string password, const User &user)
{
    std::string passwordHash = passwordHasher.generatePasswordHash(password, user.getBase64salt());
    std::cout << "Password hash: " << passwordHash << std::endl;
    std::cout << "Stored hash: " << user.getPasswordHash() << std::endl;
    return passwordHash == user.getPasswordHash();
}
