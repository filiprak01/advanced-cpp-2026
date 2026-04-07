#pragma once
#include <server/repos/channel_repo.hpp>
#include <server/repos/user_repo.hpp>
#include <unordered_map>
#include <vector>

/**
 * @brief Menedżer kanałów komunikacyjnych serwera.
 *
 * Odpowiada za tworzenie, usuwanie oraz zarządzanie uczestnikami
 * kanałów publicznych i prywatnych.
 */
class ChannelManager
{
public:
    /**
     * @brief Tworzy menedżer kanałów z podanym repozytorium.
     * @param channelRepository Repozytorium kanałów.
     */
    ChannelManager(ChannelRepository &channelRepository, UserRepository &userRepository) : channelRepository(channelRepository), nextChannelId(1), userRepository(userRepository) {}

    /**
     * @brief Tworzy publiczny kanał.
     * @param name    Nazwa kanału.
     * @param userIds Początkowa lista użytkowników.
     * @return @c true jeśli tworzenie się powiódło.
     */
    bool createPublicChannel(const std::string &name, const std::vector<std::string> &userNames);

    /**
     * @brief Tworzy prywatną konwersację między dwoma użytkownikami.
     * @param userIds Para identyfikatorów użytkowników.
     * @return @c true jeśli tworzenie się powiódło.
     */
    bool createPrivateConversation(const std::vector<std::string> &userNames);

    /**
     * @brief Usuwa kanał z serwera.
     * @param channelId Identyfikator kanału do usunięcia.
     * @return @c true jeśli usunięcie się powiódło.
     */
    bool deleteChannel(int channelId);

    /**
     * @brief Usuwa użytkownika z kanału.
     * @param channelId Identyfikator kanału.
     * @param userId    Identyfikator użytkownika.
     * @return @c true jeśli operacja się powiódła.
     */
    bool removeUserFromChannel(int channelId, const std::string &userName);

    /**
     * @brief Dodaje użytkownika do kanału.
     * @param channelId Identyfikator kanału.
     * @param userId    Identyfikator użytkownika.
     * @return @c true jeśli operacja się powiódła.
     */
    bool addUserToChannel(int channelId, const std::string &userName);

    /**
     * @brief Zmienia nazwę kanału.
     * @param channelId Identyfikator kanału.
     * @param newName   Nowa nazwa kanału.
     * @return @c true jeśli operacja się powiódła.
     */
    bool editChannelName(int channelId, const std::string &newName);

    /**
     * @brief Zwraca aktywne kanały użytkownika.
     * @param userId Identyfikator użytkownika.
     * @return Lista aktywnych kanałów użytkownika.
     */
    std::vector<Channel> getUserActiveChannels(const std::string &userName);

    /**
     * @brief Zwraca wszystkie kanały użytkownika (także nieaktywne).
     * @param userId Identyfikator użytkownika.
     * @return Lista wszystkich kanałów użytkownika.
     */
    std::vector<Channel> getAllUserChannels(const std::string &userName);

    /**
     * @brief Dezaktywuje kanał (nie usuwa go).
     * @param channelId Identyfikator kanału.
     * @return @c true jeśli dezaktywacja się powiódła.
     */
    bool deactivateChannel(int channelId);

private:
    int nextChannelId;                                            ///< Licznik do generowania identyfikatorów kanałów.
    ChannelRepository &channelRepository;                         ///< Repozytorium kanałów.
    UserRepository &userRepository;                               ///< Repozytorium użytkowników.
    std::unordered_map<std::string, std::vector<int>> channelMap; ///< Mapa: userId → lista id aktywnych kanałów.
};