#pragma once
#include <common/event.hpp>
#include <server/core/manager_context.hpp>
#include <string>
#include <nlohmann/json.hpp>
using json = nlohmann::json;
/**
 * @brief Abstrakcyjna klasa bazowa zdarzenia serwera.
 *
 * Wszystkie typy zdarzeń (uwierzytelnianie, rejestracja, wiadomości itp.)
 * dziedziczą po tej klasie i implementują metodę @c perform.
 */
class Event
{
public:
    virtual ~Event() = default;

    /**
     * @brief Wykonuje zdarzenie w podanym kontekście menedżerów.
     * @param context   Kontekst zawierający wszystkie menedżery serwera.
     * @param clientFd  Deskryptor gniazda klienta który wywołał zdarzenie.
     * @param userName  Nazwa użytkownika (pusta jeśli nie zalogowany).
     */
    virtual void perform(ManagerContext &context, int clientFd) = 0;
};