#pragma once
#include <string>
#include <server/core/event.hpp>
#include <server/core/manager_context.hpp>

/**
 * @brief Zdarzenie edycji nazwy kanału.
 *
 * Wywoływane gdy uprawniony użytkownik żąda zmiany nazwy kanału.
 * Deleguje operację do ChannelManager.
 */
class EditChannelEvent : public Event
{
public:
    /**
     * @brief Tworzy zdarzenie edycji kanału.
     * @param channelId Identyfikator edytowanego kanału.
     * @param newName   Nowa nazwa kanału.
     */
    EditChannelEvent(int channelId, const std::string &newName)
        : channelId(channelId), newName(newName) {}

    /**
     * @brief Wykonuje edycję kanału w podanym kontekście.
     * @param context Kontekst menedżerów serwera.
     */
    void perform(ManagerContext &context) override;

private:
    int channelId;       ///< Identyfikator kanału.
    std::string newName; ///< Nowa nazwa kanału.
};
