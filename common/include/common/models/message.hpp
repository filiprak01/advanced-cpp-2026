#pragma once
#include <string>
#include <nlohmann/json.hpp>
#include <functional>
#include <common/utilities/json_serializable.hpp>
using json = nlohmann::json;

/**
 * @brief Reprezentuje wiadomość przesłaną w systemie czatu.
 *
 * Przechowuje treść wiadomości, nazwę nadawcy, unikalny identyfikator
 * oraz znacznik czasu momentu wysłania.
 */
class Message : public JSONSerializable
{
public:
    Message() = default;

    /**
     * @brief Tworzy wiadomość z podanymi danymi.
     * @param id          Unikalny identyfikator wiadomości.
     * @param content     Treść wiadomości.
     * @param senderName  Nazwa nadawcy.
     * @param timestamp   Czas wysłania wiadomości.
     */
    Message(
        const int &id,
        const std::string &content,
        const std::string &senderName,
        const std::chrono::steady_clock::time_point &timestamp) : id(id), content(content), senderName(senderName), timestamp(timestamp) {}

    /// @brief Zwraca unikalny identyfikator wiadomości.
    const int &getId() const
    {
        return id;
    }

    /// @brief Zwraca treść wiadomości.
    const std::string &getContent() const
    {
        return content;
    }

    /// @brief Zwraca nazwę nadawcy wiadomości.
    const std::string &getSenderName() const
    {
        return senderName;
    }

    /// @brief Zwraca znacznik czasu wysłania wiadomości.
    const std::chrono::steady_clock::time_point &getTimestamp() const
    {
        return timestamp;
    }

    /// @brief Porównuje dwie wiadomości po identyfikatorze.
    bool operator==(const Message &other) const;

    /**
     * @brief Zwraca nową wiadomość z zaktualizowaną treścią.
     * @param newContent Nowa treść wiadomości.
     * @return Kopia wiadomości z nową treścią.
     */
    Message withContent(const std::string &newContent) const;

    /**
     * @brief Serializuje wiadomość do obiektu JSON.
     * @return Obiekt JSON reprezentujący wiadomość.
     */
    json toJson() const override;

    /**
     * @brief Deserializuje wiadomość z obiektu JSON.
     * @param j Obiekt JSON z danymi wiadomości.
     */
    void fromJson(const json &j) override;

private:
    int id;                                          ///< Unikalny identyfikator wiadomości.
    std::string content;                             ///< Treść wiadomości.
    std::string senderName;                          ///< Nazwa nadawcy.
    std::chrono::steady_clock::time_point timestamp; ///< Znacznik czasu wysłania.
};
namespace std
{
    template <>
    struct hash<Message>
    {
        std::size_t operator()(const Message &msg) const
        {
            return std::hash<int>{}(msg.getId());
        }
    };
}