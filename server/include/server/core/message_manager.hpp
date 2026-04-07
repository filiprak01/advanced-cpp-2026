#pragma once
#include <server/repos/message_repo.hpp>
#include <server/repos/user_repo.hpp>
#include <server/repos/channel_repo.hpp>
#include <unordered_map>
#include <optional>

/**
 * @brief Menedżer wiadomości serwera.
 *
 * Odpowiada za wysyłanie, edytowanie, usuwanie wiadomości
 * oraz pobieranie historii wiadomości dla kanału.
 */
class MessageManager
{
public:
    /**
     * @brief Tworzy menedżer wiadomości.
     * @param messageRepo  Repozytorium wiadomości.
     * @param userRepo     Repozytorium użytkowników.
     * @param channelRepo  Repozytorium kanałów.
     * @param nextId       Początkowa wartość licznika identyfikatorów.
     * @param messageMap   Mapa początkowa: id wiadomości → id kanału.
     */
    MessageManager(MessageRepository &messageRepo, UserRepository &userRepo, ChannelRepository &channelRepo, int nextId, const std::unordered_map<int, int> &messageMap) : messageRepository(messageRepo), userRepository(userRepo), channelRepository(channelRepo), nextMessageId(nextId), messageMap(messageMap) {}

    /**
     * @brief Wysyła wiadomość do kanału.
     * @param content    Treść wiadomości.
     * @param senderId   Identyfikator (nazwa) nadawcy.
     * @param channelId  Identyfikator docelowego kanału.
     * @param timestamp  Czas wysyłania wiadomości.
     * @return @c true jeśli wysyłanie się powiódło.
     */
    bool sendMessage(const std::string &content, std::string senderId, int channelId, const std::chrono::steady_clock::time_point &timestamp);

    /**
     * @brief Usuwa wiadomość.
     * @param messageId Identyfikator wiadomości do usunięcia.
     * @return @c true jeśli usunięcie się powiódło.
     */
    bool deleteMessage(int messageId);

    /**
     * @brief Edytuje treść wiadomości.
     * @param messageId  Identyfikator wiadomości.
     * @param newContent Nowa treść wiadomości.
     * @return @c true jeśli edycja się powiódła.
     */
    bool editMessage(int messageId, const std::string &newContent);

    /**
     * @brief Pobiera wszystkie wiadomości z kanału.
     * @param channelId Identyfikator kanału.
     * @return Lista wiadomości lub @c std::nullopt jeśli kanał nie istnieje.
     */
    std::optional<std::vector<Message>> getChannelMessages(int channelId);

private:
    MessageRepository &messageRepository;    ///< Repozytorium wiadomości.
    UserRepository &userRepository;          ///< Repozytorium użytkowników.
    ChannelRepository &channelRepository;    ///< Repozytorium kanałów.
    int nextMessageId;                       ///< Licznik do generowania identyfikatorów wiadomości.
    std::unordered_map<int, int> messageMap; ///< Mapa: id wiadomości → id kanału.
};