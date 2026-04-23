#pragma once
#include <server/core/event.hpp>
#include <server/core/manager_context.hpp>

/**
 * @brief Zdarzenie usunięcia wiadomości z kanału.
 *
 * Wywoływane gdy uprawniony klient żąda usunięcia wiadomości.
 * Deleguje operację do MessageManager.
 */
class RemoveMessageEvent : public Event
{
public:
    /**
     * @brief Tworzy zdarzenie usunięcia wiadomości.
     * @param messageId Identyfikator wiadomości do usunięcia.
     * @param channelId Identyfikator kanału zawierającego wiadomość.
     */
    RemoveMessageEvent(int messageId, int channelId)
        : messageId(messageId), channelId(channelId) {}

    /**
     * @brief Wykonuje usunięcie wiadomości w podanym kontekście.
     * @param context Kontekst menedżerów serwera.
     * @param clientFd Deskryptor klienta wysyłającego zadanie.
     */
    void perform(ManagerContext &context, int clientFd) override;

private:
    int messageId; ///< Identyfikator usuwanej wiadomości.
    int channelId; ///< Identyfikator kanału.
};
