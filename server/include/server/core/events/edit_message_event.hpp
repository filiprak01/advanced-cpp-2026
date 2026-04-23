#pragma once
#include <server/core/event.hpp>
#include <server/core/manager_context.hpp>
#include <common/const/event_messages.hpp>
/**
 * @brief Zdarzenie edycji treści wiadomości.
 *
 * Wywoływane gdy autor wiadomości żąda zmiany jej treści.
 * Deleguje operację do MessageManager.
 */
class EditMessageEvent : public Event
{
public:
    /**
     * @brief Tworzy zdarzenie edycji wiadomości.
     * @param messageId  Identyfikator edytowanej wiadomości.
     * @param newContent Nowa treść wiadomości.
     */
    EditMessageEvent(int messageId, const std::string &newContent)
        : messageId(messageId), newContent(newContent) {}

    /**
     * @brief Wykonuje edycję wiadomości w podanym kontekście.
     * @param context Kontekst menedżerów serwera.
     */
    void perform(ManagerContext &context, int clientFd) override;

private:
    int messageId;          ///< Identyfikator edytowanej wiadomości.
    std::string newContent; ///< Nowa treść wiadomości.
};
