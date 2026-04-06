#pragma once
#include <unordered_map>
#include <common/models/session.hpp>
#include <shared_mutex>
#include <mutex>

class SessionRepository
{
public:
    SessionRepository() = default;

    bool addSession(const Session &session);
    bool removeSession(const int &sessionId);
    Session getSession(const int &sessionId);
    bool updateSession(const Session &newSession, const int &sessionId);
    bool sessionExists(const int &sessionId) const;

private:
    mutable std::shared_mutex mutex;
    std::unordered_map<int, Session> sessions;
};