#pragma once
#include <string>
#include <vector>
#include <server/core/event.hpp>
#include <server/core/manager_context.hpp>
#include <common/const/event_messages.hpp>
/**
 * @brief Zdarzenie tworzenia nowego kanału publicznego.
 *
 * Wywoływane gdy klient żąda założenia nowego kanału.
 * Deleguje operację do ChannelManager.
 */
class CreateChannelEvent : public Event
{
public:
    /**
     * @brief Tworzy zdarzenie tworzenia kanału.
     * @param name    Nazwa nowego kanału.
     * @param userIds Początkowa lista identyfikatorów uczestników.
     */
    CreateChannelEvent(const std::string &name, const std::vector<std::string> &userNames, bool isPrivate)
        : name(name), userNames(userNames), isPrivate(isPrivate) {}

    /**
     * @brief Wykonuje tworzenie kanału w podanym kontekście.
     * @param context Kontekst menedżerów serwera.
     */
    void perform(ManagerContext &context, int clientFd) override;

private:
    std::string name;                   ///< Nazwa nowego kanału.
    std::vector<std::string> userNames; ///< Lista nazw uczestników.
    bool isPrivate = false;             ///< Kanał publiczny (domyślnie).
};
