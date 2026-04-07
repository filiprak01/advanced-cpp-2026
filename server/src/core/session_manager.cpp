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
bool SessionManager::createSession(const std::string &userName)
{
    Session session = Session(nextSessionId++, userName, std::chrono::steady_clock::now());
    if (sessionRepository.sessionExists(session.getSessionId()))
    {
        return false;
    }
    sessionRepository.addSession(session);
    userSessions[userName] = session.getSessionId();
    return true;
}
bool SessionManager::removeUserSession(const std::string &userName)
{
    int session_id = userSessions[userName];
    if (!sessionRepository.sessionExists(session_id))
    {
        return false;
    }
    sessionRepository.removeSession(session_id);
    userSessions.erase(userName);
    return true;
}
bool SessionManager::updateUserSession(const std::string &userName)
{
    int session_id = userSessions[userName];
    if (!sessionRepository.sessionExists(session_id))
    {
        return false;
    }
    Session oldSession = sessionRepository.getSession(session_id);
    Session newSession = oldSession.withNewLastActive(std::chrono::steady_clock::now());
    return sessionRepository.updateSession(newSession, session_id);
}