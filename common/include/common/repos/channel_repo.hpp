#pragma once
#include <common/models/channel.hpp>
#include <unordered_map>
#include <shared_mutex>
#include <mutex>

/**
 * @brief Repozytorium kanałów.
 *
 * Przechowuje wszystkie kanały w pamięci operacyjnej.
 * Dostęp jest synchronizowany współdzieloną blokadą.
 */
class ChannelRepository
{
public:
    ChannelRepository() = default;

    /**
     * @brief Dodaje kanał do repozytorium.
     * @param channel Kanał do dodania.
     * @return @c true jeśli dodanie się powiódło.
     */
    bool addChannel(const Channel &channel);

    /**
     * @brief Usuwa kanał z repozytorium.
     * @param channelId Identyfikator kanału do usunięcia.
     * @return @c true jeśli usunięcie się powiódło.
     */
    bool removeChannel(int channelId);

    /**
     * @brief Zwraca kanał o podanym identyfikatorze.
     * @param channelId Identyfikator kanału.
     * @return Obiekt kanału.
     */
    Channel getChannel(int channelId);

    /**
     * @brief Sprawdza, czy kanał istnieje w repozytorium.
     * @param channelId Identyfikator kanału.
     * @return @c true jeśli kanał istnieje.
     */
    bool channelExists(int channelId) const;

    /// @brief Zwraca @c true jeśli kanał jest aktywny.
    bool isChannelActive(int channelId) const;

    /// @brief Zwraca @c true jeśli kanał jest prywatny.
    bool isChannelPrivate(int channelId) const;

    /**
     * @brief Aktualizuje dane kanału.
     * @param newChannel Kanał z nowymi danymi (identyfikator musi pasować).
     * @return @c true jeśli aktualizacja się powiódła.
     */
    bool updateChannel(const Channel &newChannel);

private:
    mutable std::shared_mutex mutex;           ///< Blokada synchronizująca dostęp wielowątkowy.
    std::unordered_map<int, Channel> channels; ///< Mapa: identyfikator kanału → obiekt kanału.
};
