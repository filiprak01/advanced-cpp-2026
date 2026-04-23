#pragma once
#include <string>
#include <server/core/event.hpp>
#include <server/core/manager_context.hpp>

/**
 * @brief Zdarzenie synchronizacji danych z klientem.
 *
 * Wywoływane gdy klient prosi o aktualny stan kanałów i wiadomości
 * (np. po ponownym połączeniu). Wysyła aktualny snapshot stanu.
 */
class SynchronizeDataEvent : public Event
{
public:
    /**
     * @brief Tworzy zdarzenie synchronizacji.
     * @param userName Nazwa użytkownika żądającego synchronizacji.
     */
    SynchronizeDataEvent() = default;

    /**
     * @brief Wykonuje synchronizację danych w podanym kontekście.
     * @param context Kontekst menedżerów serwera.
     */
    void perform(ManagerContext &context, int clientFd) override;
};
