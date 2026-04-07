#pragma once
#include <server/core/manager_context.hpp>

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
     * @param context Kontekst zawierający wszystkie menedżery serwera.
     */
    virtual void perform(ManagerContext &context) = 0;
};