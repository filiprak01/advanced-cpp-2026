#pragma once
#include <server/core/event.hpp>
#include <server/core/manager_context.hpp>
#include <common/const/event_messages.hpp>

/**
 * @brief Zdarzenie dodania nowej wiadomości do kanału.
 *
 * Wywoływane gdy klient wysyła nową wiadomość.
 * Deleguje operację do MessageManager.
 */
class AddMessageEvent : public Event
{
public:
    /**
     * @brief Tworzy zdarzenie dodania wiadomości.
     * @param senderName Nazwa nadawcy.
     * @param channelId  Identyfikator kanału docelowego.
     * @param content    Treść wiadomości.
     */
    AddMessageEvent(const std::string &senderName, int channelId, const std::string &content, std::chrono::time_point<std::chrono::steady_clock> timestamp)
        : senderName(senderName), channelId(channelId), content(content), timestamp(timestamp) {}

    /**
     * @brief Wykonuje dodanie wiadomości w podanym kontekście.
     * @param context Kontekst menedżerów serwera.
     */
    void perform(ManagerContext &context, int clientFd) override;

private:
    std::string senderName;                          ///< Nazwa nadawcy wiadomości.
    int channelId;                                   ///< Identyfikator kanału docelowego.
    std::string content;                             ///< Treść wiadomości.
    std::chrono::steady_clock::time_point timestamp; ///< Timestamp wiadomości
};
