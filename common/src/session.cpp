#include <common/models/session.hpp>

bool Session::updateSession()
{
    lastActive = std::chrono::steady_clock::now();
    return true;
}

bool Session::operator==(const Session &other) const
{
    return sessionId == other.getSessionId();
}
