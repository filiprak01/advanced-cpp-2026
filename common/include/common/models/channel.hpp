#pragma once
#include <string>
#include <unordered_set>
#include <vector>
#include <common/models/user.hpp>
#include <common/models/message.hpp>
#include <common/utilities/json_serializable.hpp>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

/**
 * @brief Reprezentuje kanał komunikacyjny w systemie czatu.
 *
 * Kanał może być publiczny (do 32 użytkowników) lub prywatny (konwersacja 1:1).
 * Przechowuje listę wiadomości i uczestników oraz stan aktywności kanału.
 */
class Channel : public JSONSerializable
{
public:
    Channel() = default;

    /**
     * @brief Tworzy kanał z podanymi parametrami.
     * @param channelId  Unikalny identyfikator kanału.
     * @param name       Nazwa kanału.
     * @param messageIds Lista identyfikatorów wiadomości w kanale.
     * @param userIds    Zbiór identyfikatorów uczestników kanału.
     * @param isPrivate  @c true jeśli kanał jest prywatny (2 osoby), @c false jeśli publiczny.
     */
    Channel(const int &channelId, const std::string &name, const std::vector<int> &messageIds, const std::unordered_set<std::string> &userIds, bool isPrivate) : channelId(channelId), name(name), messageIds(messageIds), userIds(userIds), isPrivate(isPrivate), maxUsers(isPrivate ? 2 : 32), isActive(true) {}

    /// @brief Zwraca unikalny identyfikator kanału.
    const int &getChannelId() const
    {
        return channelId;
    }

    /// @brief Zwraca listę identyfikatorów wiadomości w kanale.
    const std::vector<int> &getMessageIds() const
    {
        return messageIds;
    }

    /// @brief Zwraca zbiór identyfikatorów uczestników kanału.
    const std::unordered_set<std::string> &getUserIds() const
    {
        return userIds;
    }

    /// @brief Zwraca nazwę kanału.
    const std::string &getName() const
    {
        return name;
    }

    /// @brief Zwraca @c true jeśli kanał jest prywatny.
    const bool &getIsPrivate() const
    {
        return isPrivate;
    }

    /// @brief Zwraca @c true jeśli kanał jest aktywny.
    const bool &getIsActive() const
    {
        return isActive;
    }

    /// @brief Zwraca maksymalną liczbę użytkowników w kanale.
    const int &getMaxUsers() const
    {
        return maxUsers;
    }

    /**
     * @brief Zwraca nowy kanał z dodanym identyfikatorem wiadomości.
     * @param messageId Identyfikator wiadomości do dodania.
     */
    const Channel addMessageId(int messageId) const;

    /**
     * @brief Zwraca nowy kanał z dodanym uczestnikiem.
     * @param userId Identyfikator użytkownika do dodania.
     */
    const Channel addUserId(const std::string &userName) const;

    /**
     * @brief Zwraca nowy kanał bez wskazanego uczestnika.
     * @param userName Nazwa użytkownika do usunięcia.
     */
    const Channel removeUserId(const std::string &userName) const;

    /**
     * @brief Zwraca nowy kanał bez wskazanej wiadomości.
     * @param messageId Identyfikator wiadomości do usunięcia.
     */
    const Channel removeMessageId(int messageId) const;

    /**
     * @brief Zwraca nowy kanał z nową nazwą.
     * @param newName Nowa nazwa kanału.
     */
    const Channel withName(const std::string &newName) const;

    /// @brief Zwraca nowy kanał z przełączonym stanem aktywności.
    const Channel toggleActive() const;

    /// @brief Porównuje dwa kanały po identyfikatorze.
    const bool operator==(const Channel &other) const;

    /// @brief Serializuje kanał do formatu JSON.
    json toJson() const override;

    /// @brief Deserializuje kanał z formatu JSON.
    void fromJson(const json &j) override;

private:
    int channelId;                           ///< Unikalny identyfikator kanału.
    bool isPrivate;                          ///< Czy kanał jest prywatny.
    bool isActive;                           ///< Czy kanał jest aktywny.
    std::string name;                        ///< Nazwa kanału.
    std::vector<int> messageIds;             ///< Identyfikatory wiadomości w kanale.
    std::unordered_set<std::string> userIds; ///< Nazwy uczestników kanału.
    int maxUsers;                            ///< Maksymalna liczba użytkowników (2 lub 32).
};
namespace std
{
    template <>
    struct hash<Channel>
    {
        std::size_t operator()(const Channel &channel) const
        {
            return std::hash<int>{}(channel.getChannelId());
        }
    };
}