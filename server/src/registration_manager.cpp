#include <server/auth/registration_manager.hpp>

RegistrationManager::RegistrationManager(UserRepository &userRepo, PasswordHasher &passwordHasher,
                                         int passwordMinLength, int passwordMaxLength,
                                         int usernameMinLength, int usernameMaxLength)
    : userRepo(userRepo), passwordHasher(passwordHasher),
      passwordMinLength(passwordMinLength), passwordMaxLength(passwordMaxLength),
      usernameMinLength(usernameMinLength), usernameMaxLength(usernameMaxLength) {}

bool RegistrationManager::validatePassword(const std::string &password) const
{
    int len = static_cast<int>(password.length());
    return len >= passwordMinLength && len <= passwordMaxLength;
}

bool RegistrationManager::validateUsername(const std::string &username) const
{
    int len = static_cast<int>(username.length());
    return len >= usernameMinLength && len <= usernameMaxLength;
}

bool RegistrationManager::registerUser(const std::string &username, const std::string &password)
{
    if (!validateUsername(username))
    {
        return false;
    }
    if (!validatePassword(password))
    {
        return false;
    }
    if (userRepo.userExists(username))
    {
        return false;
    }

    const std::string base64salt = passwordHasher.generateBase64Salt();
    const std::string passwordHash = passwordHasher.generatePasswordHash(password, base64salt);
    return userRepo.addUser(User(username, passwordHash, base64salt));
}
