#include <common/models/user.hpp>

bool User::operator==(const User &other) const
{
    return username == other.getUsername();
}

json User::toJson() const
{
    json j;
    j["username"] = username;
    j["passwordHash"] = passwordHash;
    j["base64salt"] = base64salt;
    return j;
}

User User::fromJson(const json &j)
{
    std::string username = j.at("username").get<std::string>();
    std::string passwordHash = j.at("passwordHash").get<std::string>();
    std::string base64salt = j.at("base64salt").get<std::string>();
    return User(username, passwordHash, base64salt);
}
