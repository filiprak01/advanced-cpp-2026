#include <common/models/user.hpp>

bool User::operator==(const User &other) const
{
    return username == other.getUsername();
}