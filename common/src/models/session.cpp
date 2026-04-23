#include <common/models/session.hpp>

Session Session::withNewLastActive(const std::chrono::time_point<std::chrono::steady_clock> &newDeadline) const
{
    return Session(
        this->sessionId,
        this->userName,
        newDeadline);
}

bool Session::operator==(const Session &other) const
{
    return sessionId == other.getSessionId();
}

json Session::toJson() const
{
    json j;
    j["sessionId"] = sessionId;
    j["userName"] = userName;
    j["lastActiveMs"] = std::chrono::duration_cast<std::chrono::milliseconds>(
                            lastActive.time_since_epoch())
                            .count();
    return j;
}

void Session::fromJson(const json &j)
{
    sessionId = j.at("sessionId").get<int>();
    userName = j.at("userName").get<std::string>();
    long long ms = j.value("lastActiveMs", 0LL);
    lastActive = std::chrono::steady_clock::time_point(std::chrono::milliseconds(ms));
}
