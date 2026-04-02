#include <server/auth/password_hasher.hpp>
#include <common/repos/user_repo.hpp>

class RegistrationManager
{
public:
    RegistrationManager(UserRepository &userRepo, PasswordHasher &passwordHasher, int passwordMinLength = 8, int passwordMaxLength = 64);
    bool registerUser(const std::string &username, const std::string &password);

private:
    int passwordMinLength;
    int passwordMaxLength;
    UserRepository &userRepo;
    PasswordHasher &passwordHasher;
    bool validatePassword(const std::string &password) const;
};