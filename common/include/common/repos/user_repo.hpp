#pragma once
#include <string>
#include <common/models/user.hpp>
#include <unordered_map>
#include <shared_mutex>
#include <mutex>
#include <vector>
class UserRepository
{
    UserRepository() = default;

public:
    // implement check for existing and CRUD operations for users
    bool addUser(const User &user);
    bool removeUser(const std::string &username);
    User getUser(const std::string &username);
    bool userExists(const std::string &username) const;
    std::vector<User> getAllUsers() const;

private:
    mutable std::shared_mutex mutex;
    std::unordered_map<std::string, User> users;
};