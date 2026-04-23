#include <server/repos/session_repo.hpp>

bool SessionRepository::sessionExists(const int &sessionId) const
{
    std::shared_lock<std::shared_mutex> lock(mutex);
    return sessions.find(sessionId) != sessions.end();
}
bool SessionRepository::addSession(const Session &session)
{
    std::unique_lock<std::shared_mutex> lock(mutex);
    sessions[session.getSessionId()] = session;
    return true;
}
bool SessionRepository::removeSession(const int &sessionId)
{
    std::unique_lock<std::shared_mutex> lock(mutex);
    sessions.erase(sessionId);
    return true;
}
Session SessionRepository::getSession(const int &sessionId)
{
    std::shared_lock<std::shared_mutex> lock(mutex);
    return sessions[sessionId];
}
bool SessionRepository::updateSession(const Session &newSession, const int &sessionId)
{
    std::unique_lock<std::shared_mutex> lock(mutex);
    sessions[sessionId] = newSession;
    return true;
}

std::vector<Session> SessionRepository::getAllSessions() const
{
    std::vector<Session> result;
    std::shared_lock<std::shared_mutex> lock(mutex);
    for (const auto &entry : sessions)
    {
        result.push_back(entry.second);
    }
    return result;
}

json SessionRepository::toJson() const
{
    json result = json::array();
    std::shared_lock<std::shared_mutex> lock(mutex);
    for (const auto &entry : sessions)
    {
        result.push_back(entry.second.toJson());
    }
    return result;
}

void SessionRepository::fromJson(const json &j)
{
    std::unique_lock<std::shared_mutex> lock(mutex);
    sessions.clear();
    for (const auto &item : j)
    {
        Session session;
        session.fromJson(item);
        sessions[session.getSessionId()] = session;
    }
}
