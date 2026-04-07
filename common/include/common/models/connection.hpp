#pragma once
#include <string>
#include <common/models/session.hpp>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

/**
 * @brief Reprezentuje aktywne połączenie klienta z serwerem.
 *
 * Łączy deskryptor gniazda sieciowego z przypisaną sesją użytkownika.
 */
class Connection
{
public:
    /**
     * @brief Tworzy połączenie z podanym gniazdem i sesją.
     * @param socket  Deskryptor gniazda sieciowego klienta.
     * @param session Sesja przypisana do tego połączenia.
     */
    Connection(std::string &socket, Session &session) : socket(socket), session(session) {}
    /// @brief Zwraca deskryptor gniazda sieciowego.
    const std::string &getSocket() const
    {
        return socket;
    }

    /// @brief Zwraca sesję przypisaną do połączenia.
    const Session &getSession() const
    {
        return session;
    }

    /// @brief Porównuje dwa połączenia po deskryptorze gniazda.
    bool operator==(const Connection &other) const;

    /**
     * @brief Serializuje połączenie do obiektu JSON.
     * @return Obiekt JSON reprezentujący połączenie.
     */
    json toJson() const;

    /**
     * @brief Deserializuje połączenie z obiektu JSON.
     * @param j Obiekt JSON z danymi połączenia.
     * @return Odtworzony obiekt połączenia.
     */
    static Connection fromJson(const json &j);

private:
    std::string socket; ///< Deskryptor gniazda sieciowego.
    Session session;    ///< Sesja powiązana z połączeniem.
};
namespace std
{
    template <>
    struct hash<Connection>
    {
        std::size_t operator()(const Connection &connection) const
        {
            return std::hash<std::string>{}(connection.getSocket());
        }
    };
}
