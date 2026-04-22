#pragma once
#include <string>
#include <server/core/event.hpp>
#include <server/core/manager_context.hpp>

/**
 * @brief Zdarzenie dodania użytkownika do kanału.
 *
 * Wywoływane gdy klient żąda dołączenia do kanału lub zaproszenia innego uczestnika.
 * Deleguje operację do ChannelManager.
 */
class AddUserToChannelEvent : public Event
{
public:
    /**
     * @brief Tworzy zdarzenie dodania użytkownika do kanału.
     * @param channelId Identyfikator kanału.
     * @param userId    Identyfikator użytkownika do dodania.
     */
    AddUserToChannelEvent(int channelId, const std::string &userName)
        : channelId(channelId), userName(userName) {}

    /**
     * @brief Wykonuje dodanie użytkownika do kanału w podanym kontekście.
     * @param context Kontekst menedżerów serwera.
     */
    void perform(ManagerContext &context, int clientFd) override;

private:
    int channelId;        ///< Identyfikator kanału.
    std::string userName; ///< Nazwa dodawanego użytkownika.
};
