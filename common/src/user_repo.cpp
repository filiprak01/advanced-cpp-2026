#include <common/repos/user_repo.hpp>

bool UserRepository::userExists(const std::string &username) const
{
    std::shared_lock<std::shared_mutex> lock(mutex);
    return users.find(username) != users.end();
}

bool UserRepository::addUser(const User &user)
{
    std::unique_lock<std::shared_mutex> lock(mutex);
    if (userExists(user.getUsername()))
    {
        return false;
    }
    users[user.getUsername()] = user;
    return true;
}
bool UserRepository::removeUser(const std::string &username)
{
    std::unique_lock<std::shared_mutex> lock(mutex);
    if (!userExists(username))
    {
        return false;
    }
    users.erase(username);
    return true;
}
User UserRepository::getUser(const std::string &username)
{
    std::shared_lock<std::shared_mutex> lock(mutex);
    if (!userExists(username))
    {
        return User();
    }
    return users[username];
}
std::vector<User> getAllUsers() const
{
    std::vector<User> users;
    std::shared_lock<std::shared_mutex> lock(mutex);
    for (auto &userRepoEntry : users)
    {
        users.push_back(userRepoEntry.second);
    }
}