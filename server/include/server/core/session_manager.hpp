#pragma once
#include <server/repos/session_repo.hpp>
#include <unordered_map>
class SessionManager
{
public:
    SessionManager(SessionRepository &sessionRepository, std::chrono::milliseconds sessionTimeout, std::unordered_map<int, int> userSessions, int nextSessionId = 1)
        : sessionRepository(sessionRepository), sessionTimeout(sessionTimeout), userSessions(userSessions), nextSessionId(nextSessionId) {}

    bool cleanInactiveSessions();
    bool createSession(const int &userId);
    bool removeUserSession(const int &userId);
    bool updateUserSession(const int &userId);

private:
    SessionRepository &sessionRepository;
    std::chrono::milliseconds sessionTimeout;
    std::unordered_map<int, int> userSessions;
    bool isSessionValid(const int &sessionId);
    int nextSessionId;
};