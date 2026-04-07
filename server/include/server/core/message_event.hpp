#pragma once
#include <server/core/event.hpp>
#include <server/core/manager_context.hpp>
#include <chrono>
/**
 * @brief Zdarzenie wysyłania wiadomości do kanału.
 * Wywoływane gdy klient żąda wysłania wiadomości do określonego kanału.
 * Deleguje walidację i dystrybucję wiadomości do MessageManager.
 */
class MessageEvent : public Event
{
public:
    MessageEvent(std::string senderName, int channelId, const std::string &content) : senderName(senderName), channelId(channelId), content(content) {}
    void perform(ManagerContext &context) override;

private:
    std::string senderName;                          ///< Identyfikator nadawcy wiadomości (do uzupełnienia podczas wykonania).
    int channelId;                                   ///< Identyfikator kanału, do którego wysyłana
    std::string content;                             ///< Treść wiadomości do wysłania.
    std::chrono::steady_clock::time_point timestamp; ///< Znacznik czasu wysłania wiadomości (do uzupełnienia podczas wykonania).
};