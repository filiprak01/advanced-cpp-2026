#include <common/models/session.hpp>

Session Session::withNewLastActive(const std::chrono::time_point<std::chrono::steady_clock> &newDeadline) const
{
    return Session(
        this->sessionId,
        this->userId,
        newDeadline);
}

bool Session::operator==(const Session &other) const
{
    return sessionId == other.getSessionId();
}