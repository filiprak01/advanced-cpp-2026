#pragma once
#include <string>
#include <server/core/event.hpp>
#include <server/core/manager_context.hpp>

/**
 * @brief Zdarzenie zapisu stanu serwera do pliku.
 *
 * Wywoływane ręcznie lub automatycznie w celu utrwalenia
 * bieżącego stanu repozytoriów (użytkownicy, kanały, wiadomości).
 */
class SaveDataEvent : public Event
{
public:
    /**
     * @brief Tworzy zdarzenie zapisu danych.
     * @param filePath Ścieżka do pliku docelowego.
     */
    explicit SaveDataEvent(const std::string &filePath) : filePath(filePath) {}

    /**
     * @brief Wykonuje zapis danych w podanym kontekście.
     * @param context Kontekst menedżerów serwera.
     */
    void perform(ManagerContext &context) override;

private:
    std::string filePath; ///< Ścieżka do pliku zapisu.
};
