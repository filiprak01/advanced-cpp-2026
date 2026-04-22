#pragma once
#include <server/core/event.hpp>
#include <server/core/manager_context.hpp>

/**
 * @brief Zdarzenie usunięcia kanału.
 *
 * Wywoływane gdy uprawniony użytkownik żąda usunięcia kanału.
 * Deleguje operację do ChannelManager.
 */
class RemoveChannelEvent : public Event
{
public:
    /**
     * @brief Tworzy zdarzenie usunięcia kanału.
     * @param channelId Identyfikator kanału do usunięcia.
     */
    explicit RemoveChannelEvent(int channelId) : channelId(channelId) {}

    /**
     * @brief Wykonuje usunięcie kanału w podanym kontekście.
     * @param context Kontekst menedżerów serwera.
     */
    void perform(ManagerContext &context, int clientFd) override;

private:
    int channelId; ///< Identyfikator usuwanego kanału.
};
