#pragma once
#include <string>

/**
 * @brief Abstrakcyjna klasa bazowa zdarzenia.
 *
 * Wspólna dla serwera (zdarzenia wejściowe) i klienta (zdarzenia odpowiedzi).
 * Parametr szablonu Context pozwala serwerowi używać ManagerContext,
 * a klientowi ResponseDispatcher — bez wzajemnych zależności.
 */
template <typename Context>
class BaseEvent
{
public:
    virtual ~BaseEvent() = default;

    /**
     * @brief Wykonuje zdarzenie w podanym kontekście.
     * @param context Kontekst specyficzny dla strony (serwer / klient).
     */
    virtual void perform(Context &context) = 0;
};
