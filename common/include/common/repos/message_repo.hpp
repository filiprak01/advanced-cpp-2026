#pragma once
#include <common/models/message.hpp>
#include <unordered_map>
#include <shared_mutex>
#include <mutex>

/**
 * @brief Repozytorium wiadomości.
 *
 * Przechowuje wiadomości w pamięci operacyjnej.
 * Dostęp jest synchronizowany współdzieloną blokadą.
 */
class MessageRepository
{
public:
    MessageRepository() = default;

    /**
     * @brief Dodaje wiadomość do repozytorium.
     * @param message Wiadomość do dodania.
     * @return @c true jeśli dodanie się powiódło.
     */
    bool addMessage(const Message &message);

    /**
     * @brief Usuwa wiadomość z repozytorium.
     * @param messageId Identyfikator wiadomości do usunięcia.
     * @return @c true jeśli usunięcie się powiódło.
     */
    bool removeMessage(int messageId);

    /**
     * @brief Zwraca wiadomość o podanym identyfikatorze.
     * @param messageId Identyfikator wiadomości.
     * @return Obiekt wiadomości.
     */
    Message getMessage(int messageId);

    /**
     * @brief Sprawdza, czy wiadomość istnieje w repozytorium.
     * @param messageId Identyfikator wiadomości.
     * @return @c true jeśli wiadomość istnieje.
     */
    bool messageExists(int messageId);

    /**
     * @brief Aktualizuje treść wiadomości.
     * @param message Wiadomość z nowymi danymi.
     * @return @c true jeśli aktualizacja się powiódła.
     */
    bool updateMessage(const Message &message);

private:
    mutable std::shared_mutex mutex;           ///< Blokada synchronizująca dostęp wielowątkowy.
    std::unordered_map<int, Message> messages; ///< Mapa: identyfikator wiadomości → obiekt wiadomości.
};
