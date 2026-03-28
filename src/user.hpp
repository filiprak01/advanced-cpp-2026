#pragma once
#include "message.hpp" 
#include <string>
#include <functional>


class User {
    public:
        User() = default;
        User(const std::string& username, const std::string& email);
        const std::string& getUsername() const;
        const std::string& getEmail() const;
        bool operator==(const User& other) const;
    private:
        std::string username_;
        std::string email_;
};

namespace std {
    template<>
    struct hash<User> {
        std::size_t operator()(const User& user) const {
            std::string combined = user.getUsername() + user.getEmail();
            return std::hash<std::string>{}(combined);
        }
    };
}