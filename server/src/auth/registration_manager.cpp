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

DomainResult RegistrationManager::registerUser(const std::string &username, const std::string &password)
{
    if (!validateUsername(username))
    {
        return DomainResult::formatError(errors::Code::invalid_payload);
    }
    if (!validatePassword(password))
    {
        return DomainResult::formatError(errors::Code::invalid_payload);
    }
    if (userRepo.userExists(username))
    {
        return DomainResult::domainError(errors::Code::user_already_exists);
    }

    const std::string base64salt = passwordHasher.generateBase64Salt();
    const std::string passwordHash = passwordHasher.generatePasswordHash(password, base64salt);
    if (!userRepo.addUser(User(username, passwordHash, base64salt)))
    {
        return DomainResult::domainError(errors::Code::forbidden);
    }

    return DomainResult::success(success::Code::user_registered);
}

std::vector<std::string> RegistrationManager::getRegisteredUsernames() const
{
    std::vector<std::string> usernames;
    for (const auto &user : userRepo.getAllUsers())
    {
        usernames.push_back(user.getUsername());
    }
    return usernames;
}
