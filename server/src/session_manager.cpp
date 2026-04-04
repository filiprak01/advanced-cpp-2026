#include <server/core/session_manager.hpp>

bool SessionManager::isSessionValid(const int &sessionId)
{
    Session sessionOpt = sessionRepository.getSession(sessionId);
    if (!sessionRepository.sessionExists(sessionId))
    {
        return false;
    }
    auto now = std::chrono::steady_clock::now();
    auto lastActive = sessionOpt.getLastActive();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastActive);
    return duration < sessionTimeout;
}
bool SessionManager::cleanInactiveSessions()
{
    for (const auto &userSessionId : userSessions)
    {
        int sessionId = userSessionId.second;
        if (!sessionRepository.sessionExists(sessionId) || !isSessionValid(sessionId))
        {
            removeUserSession(userSessionId.first);
        }
    }
    return true;
}
bool SessionManager::createSession(const int &userId)
{
    Session session = Session(nextSessionId++, userId, std::chrono::steady_clock::now());
    if (sessionRepository.sessionExists(session.getSessionId()))
    {
        return false;
    }
    sessionRepository.addSession(session);
    userSessions[userId] = session.getSessionId();
    return true;
}
bool SessionManager::removeUserSession(const int &userId)
{
    int session_id = userSessions[userId];
    if (!sessionRepository.sessionExists(session_id))
    {
        return false;
    }
    sessionRepository.removeSession(session_id);
    userSessions.erase(userId);
    return true;
}
bool SessionManager::updateUserSession(const int &userId)
{
    int session_id = userSessions[userId];
    if (!sessionRepository.sessionExists(session_id))
    {
        return false;
    }
    Session oldSession = sessionRepository.getSession(session_id);
    Session newSession = oldSession.withNewLastActive(std::chrono::steady_clock::now());
    return sessionRepository.updateSession(newSession, session_id);
}