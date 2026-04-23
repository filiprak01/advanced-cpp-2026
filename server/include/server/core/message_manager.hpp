#pragma once
#include <server/repos/message_repo.hpp>
#include <server/repos/user_repo.hpp>
#include <server/repos/channel_repo.hpp>
#include <server/core/connection_manager.hpp>
#include <server/core/domain_result.hpp>
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
    MessageManager(ConnectionManager &connectionManager, MessageRepository &messageRepo, UserRepository &userRepo, ChannelRepository &channelRepo, int nextId, const std::unordered_map<int, int> &messageMap) : connectionManager(connectionManager), messageRepository(messageRepo), userRepository(userRepo), channelRepository(channelRepo), nextMessageId(nextId), messageMap(messageMap) {}

    /**
     * @brief Wysyła wiadomość do kanału.
     * @param content    Treść wiadomości.
     * @param senderId   Identyfikator (nazwa) nadawcy.
     * @param channelId  Identyfikator docelowego kanału.
     * @param timestamp  Czas wysyłania wiadomości.
     * @return @c true jeśli wysyłanie się powiódło.
     */
    DomainResult sendMessage(const std::string &content, int fd, int channelId, const std::chrono::steady_clock::time_point &timestamp, Message *createdMessage = nullptr);

    /**
     * @brief Usuwa wiadomość.
     * @param messageId Identyfikator wiadomości do usunięcia.
     * @return @c true jeśli usunięcie się powiódło.
     */
    DomainResult deleteMessage(std::string requestorName, int messageId);

    /**
     * @brief Edytuje treść wiadomości.
     * @param messageId  Identyfikator wiadomości.
     * @param newContent Nowa treść wiadomości.
     * @return @c true jeśli edycja się powiódła.
     */
    DomainResult editMessage(std::string requestorName, int messageId, const std::string &newContent);

    /**
     * @brief Pobiera wszystkie wiadomości z kanału.
     * @param channelId Identyfikator kanału.
     * @return Lista wiadomości lub @c std::nullopt jeśli kanał nie istnieje.
     */
    std::optional<std::vector<Message>> getChannelMessages(int channelId);

    std::optional<int> getChannelIdFromMessage(int messageId) const;

private:
    ConnectionManager &connectionManager;
    MessageRepository &messageRepository;    ///< Repozytorium wiadomości.
    UserRepository &userRepository;          ///< Repozytorium użytkowników.
    ChannelRepository &channelRepository;    ///< Repozytorium kanałów.
    int nextMessageId;                       ///< Licznik do generowania identyfikatorów wiadomości.
    std::unordered_map<int, int> messageMap; ///< Mapa: id wiadomości → id kanału.
};
