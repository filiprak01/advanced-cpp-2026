#include <server/core/session_manager.hpp>

#include <algorithm>

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
    std::vector<std::string> users;
    for (const auto &userSessionId : userSessions)
    {
        int sessionId = userSessionId.second;
        if (!sessionRepository.sessionExists(sessionId) || !isSessionValid(sessionId))
        {
            users.push_back(userSessionId.first);
        }
    }
    for (std::string &user : users)
    {
        removeUserSession(user);
    }
    return true;
}
bool SessionManager::hasSession(int fd)
{
    cleanInactiveSessions();
    std::string username = connectionManager.getUsernameFromFd(fd);
    if (username == "")
    {
        return false;
    }
    return userSessions.count(username) > 0;
}

bool SessionManager::createSession(int fd)
{
    std::string username = connectionManager.getUsernameFromFd(fd);
    if (username == "")
    {
        return false;
    }
    Session session = Session(nextSessionId++, username, std::chrono::steady_clock::now());
    if (sessionRepository.sessionExists(session.getSessionId()))
    {
        return false;
    }

    auto userSession = userSessions.find(username);
    if (userSession != userSessions.end())
    {
        if (sessionRepository.sessionExists(userSession->second))
        {
            sessionRepository.removeSession(userSession->second);
        }
        userSessions.erase(username);
    }

    sessionRepository.addSession(session);
    userSessions[username] = session.getSessionId();
    return true;
}
bool SessionManager::removeUserSession(const std::string &userName)
{
    auto session = userSessions.find(userName);
    if (session == userSessions.end())
    {
        return true;
    }
    int session_id = session->second;
    if (!sessionRepository.sessionExists(session_id))
    {
        userSessions.erase(userName);
        return false;
    }
    sessionRepository.removeSession(session_id);
    userSessions.erase(userName);
    return true;
}
bool SessionManager::updateUserSession(int fd)
{
    std::string username = connectionManager.getUsernameFromFd(fd);
    if (username == "")
    {
        return false;
    }
    auto session = userSessions.find(username);
    if (session == userSessions.end())
    {
        return false;
    }
    int session_id = session->second;
    if (!sessionRepository.sessionExists(session_id))
    {
        return false;
    }
    Session oldSession = sessionRepository.getSession(session_id);
    Session newSession = oldSession.withNewLastActive(std::chrono::steady_clock::now());
    return sessionRepository.updateSession(newSession, session_id);
}

void SessionManager::rebuildIndexFromRepository()
{
    userSessions.clear();
    nextSessionId = 1;

    for (const auto &session : sessionRepository.getAllSessions())
    {
        nextSessionId = std::max(nextSessionId, session.getSessionId() + 1);
        userSessions[session.getUserName()] = session.getSessionId();
    }
}
