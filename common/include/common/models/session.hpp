#pragma once
#include <string>
#include <common/models/user.hpp>
#include <chrono>
class Session
{
    Session() = default;
    Session(const int &sessionId, const int &userId, const std::chrono::_V2::steady_clock::time_point &lastActive) : sessionId(sessionId), userId(userId), lastActive(lastActive) {}
    const int &getSessionId() const
    {
        return sessionId;
    }
    const int &getUserId() const
    {
        return userId;
    }
    const std::chrono::_V2::steady_clock::time_point &getLastActive() const
    {
        return lastActive;
    }
    bool operator==(const Session &other) const;
    bool updateSession();

private:
    int sessionId;
    int userId;
    std::chrono::_V2::steady_clock::time_point lastActive;
};