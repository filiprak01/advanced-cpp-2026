#include <server/repos/user_repo.hpp>

bool UserRepository::userExists(const std::string &username) const
{
    std::shared_lock<std::shared_mutex> lock(mutex);
    return users.find(username) != users.end();
}

bool UserRepository::addUser(const User &user)
{
    std::unique_lock<std::shared_mutex> lock(mutex);
    if (users.find(user.getUsername()) != users.end())
    {
        return false;
    }
    users[user.getUsername()] = user;
    return true;
}
bool UserRepository::removeUser(const std::string &username)
{
    std::unique_lock<std::shared_mutex> lock(mutex);
    auto it = users.find(username);
    if (it == users.end())
    {
        return false;
    }
    users.erase(it);
    return true;
}
User UserRepository::getUser(const std::string &username)
{
    std::shared_lock<std::shared_mutex> lock(mutex);
    auto it = users.find(username);
    if (it == users.end())
    {
        return User();
    }
    return it->second;
}
std::vector<User> UserRepository::getAllUsers() const
{
    std::vector<User> result;
    std::shared_lock<std::shared_mutex> lock(mutex);
    for (const auto &userRepoEntry : users)
    {
        result.push_back(userRepoEntry.second);
    }
    return result;
}