#include <server/auth/auth_manager.hpp>

bool AuthManager::authenticate(const std::string &username, const std::string &password)
{
    User userOpt = userRepository.getUser(username);
    if (userOpt.getUsername() == "")
    {
        return false; // User not found
    }
    return verifyPassword(password, userOpt);
}
bool AuthManager::verifyPassword(const std::string password, const User &user)
{
    std::string passwordHash = passwordHasher.generatePasswordHash(password, user.getBase64salt());
    return passwordHash == user.getPasswordHash();
}
