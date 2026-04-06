#pragma once
#include <server/auth/password_hasher.hpp>
#include <server/repos/user_repo.hpp>

class AuthManager
{
public:
    AuthManager(UserRepository &userRepository, PasswordHasher &passwordHasher)
        : userRepository(userRepository), passwordHasher(passwordHasher) {}

    bool authenticate(const std::string &username, const std::string &password);

private:
    UserRepository &userRepository;
    PasswordHasher &passwordHasher;
    bool verifyPassword(const std::string password, const User &user);
};