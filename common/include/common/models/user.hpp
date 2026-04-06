#pragma once
#include <string>
#include <functional>

class User
{
public:
    User() = default;
    User(const std::string &username, const std::string &passwordHash, const std::string &base64salt) : username(username), passwordHash(passwordHash), base64salt(base64salt) {}
    const std::string &getUsername() const
    {
        return username;
    }
    const std::string &getPasswordHash() const
    {
        return passwordHash;
    }
    const std::string &getBase64salt() const
    {
        return base64salt;
    }
    bool operator==(const User &other) const;

private:
    std::string username;
    std::string passwordHash;
    std::string base64salt;
};

namespace std
{
    template <>
    struct hash<User>
    {
        std::size_t operator()(const User &user) const
        {
            return std::hash<std::string>{}(user.getUsername());
        }
    };
}