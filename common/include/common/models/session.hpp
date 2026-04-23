#pragma once
#include <string>
#include <common/models/user.hpp>
#include <chrono>
#include <nlohmann/json.hpp>
#include <common/utilities/json_serializable.hpp>
using json = nlohmann::json;

/**
 * @brief Reprezentuje sesję zalogowanego użytkownika.
 *
 * Przechowuje powiązanie między identyfikatorem sesji a użytkownikiem
 * oraz czas ostatniej aktywności do wykrywania nieaktywnych sesji.
 */
class Session : public JSONSerializable
{
public:
    Session() = default;

    /**
     * @brief Tworzy sesję z podanymi parametrami.
     * @param sessionId   Unikalny identyfikator sesji.
     * @param userName    Nazwa użytkownika przypisanego do sesji.
     * @param lastActive  Czas ostatniej aktywności sesji.
     */
    Session(const int &sessionId, const std::string &userName, const std::chrono::time_point<std::chrono::steady_clock> &lastActive) : sessionId(sessionId), userName(userName), lastActive(lastActive) {}

    /**
     * @brief Zwraca nową sesję z zaktualizowanym czasem ostatniej aktywności.
     * @param newDeadline Nowy czas ostatniej aktywności.
     * @return Kopia sesji z nowym czasem aktywności.
     */
    Session withNewLastActive(const std::chrono::time_point<std::chrono::steady_clock> &newDeadline) const;

    /// @brief Zwraca unikalny identyfikator sesji.
    const int &getSessionId() const
    {
        return sessionId;
    }

    /// @brief Zwraca nazwę użytkownika powiązanego z sesją.
    const std::string &getUserName() const
    {
        return userName;
    }

    /// @brief Zwraca czas ostatniej aktywności sesji.
    const std::chrono::time_point<std::chrono::steady_clock> &getLastActive() const
    {
        return lastActive;
    }

    /// @brief Porównuje dwie sesje po identyfikatorze sesji.
    bool operator==(const Session &other) const;

    /**
     * @brief Serializuje sesję do obiektu JSON.
     * @return Obiekt JSON reprezentujący sesję.
     */
    json toJson() const override;

    /**
     * @brief Deserializuje sesję z obiektu JSON.
     * @param j Obiekt JSON z danymi sesji.
     */
    void fromJson(const json &j) override;

private:
    int sessionId;                                                 ///< Identyfikator sesji.
    std::string userName;                                          ///< Nazwa użytkownika.
    std::chrono::time_point<std::chrono::steady_clock> lastActive; ///< Czas ostatniej aktywności.
};
