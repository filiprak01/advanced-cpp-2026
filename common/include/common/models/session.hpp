#pragma once
#include <string>
#include <common/models/user.hpp>
#include <chrono>
class Session
{
public:
    Session() = default;
    Session(const int &sessionId, const int &userId, const std::chrono::time_point<std::chrono::steady_clock> &lastActive) : sessionId(sessionId), userId(userId), lastActive(lastActive) {}
    Session withNewLastActive(const std::chrono::time_point<std::chrono::steady_clock> &newDeadline) const;
    const int &getSessionId() const
    {
        return sessionId;
    }
    const int &getUserId() const
    {
        return userId;
    }
    const std::chrono::time_point<std::chrono::steady_clock> &getLastActive() const
    {
        return lastActive;
    }
    bool operator==(const Session &other) const;

private:
    int sessionId;
    int userId;
    std::chrono::time_point<std::chrono::steady_clock> lastActive;
};