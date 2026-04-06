#pragma once
#include <server/auth/password_hasher.hpp>
#include <server/repos/user_repo.hpp>

class RegistrationManager
{
public:
    RegistrationManager(UserRepository &userRepo, PasswordHasher &passwordHasher,
                        int passwordMinLength = 8, int passwordMaxLength = 64,
                        int usernameMinLength = 3, int usernameMaxLength = 20);
    bool registerUser(const std::string &username, const std::string &password);

private:
    int passwordMinLength;
    int passwordMaxLength;
    int usernameMinLength;
    int usernameMaxLength;
    UserRepository &userRepo;
    PasswordHasher &passwordHasher;
    bool validatePassword(const std::string &password) const;
    bool validateUsername(const std::string &username) const;
};