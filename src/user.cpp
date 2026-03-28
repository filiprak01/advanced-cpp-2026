#include "user.hpp"


User::User(const std::string& username, const std::string& email) : username_(username), email_(email) {}

const std::string& User::getUsername() const
{
    return username_;
}
const std::string& User::getEmail() const
{
    return email_;
}
bool User::operator==(const User& other) const
{
    return username_ == other.username_ && email_ == other.email_;
}
