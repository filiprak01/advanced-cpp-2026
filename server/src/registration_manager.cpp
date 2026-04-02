#include <server/auth/registration_manager.hpp>

RegistrationManager::RegistrationManager(UserRepository &userRepo, PasswordHasher &passwordHasher, int passwordMinLength, int passwordMaxLength)
    : userRepo(userRepo), passwordHasher(passwordHasher), passwordMinLength(passwordMinLength), passwordMaxLength(passwordMaxLength) {}

bool RegistrationManager::registerUser(const std::string &username, const std::string &password)
{
    if (!validatePassword(password))
    {
        return false;
    }

    const std::string base64salt = passwordHasher.generateBase64Salt();
    const std::string passwordHash = passwordHasher.generatePasswordHash(password, base64salt);
    return userRepo.addUser(User(username, passwordHash, base64salt));
}
