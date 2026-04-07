#pragma once
#include <string>
#include <functional>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

/**
 * @brief Reprezentuje użytkownika systemu.
 *
 * Przechowuje nazwę użytkownika oraz dane uwierzytelniające
 * (skrót hasła i sól w formacie Base64).
 */
class User
{
public:
    User() = default;

    /**
     * @brief Tworzy użytkownika z podanymi danymi.
     * @param username  Nazwa użytkownika.
     * @param passwordHash Skrót hasła użytkownika.
     * @param base64salt   Sól hasła zakodowana w Base64.
     */
    User(const std::string &username, const std::string &passwordHash, const std::string &base64salt) : username(username), passwordHash(passwordHash), base64salt(base64salt) {}

    /// @brief Zwraca nazwę użytkownika.
    const std::string &getUsername() const
    {
        return username;
    }

    /// @brief Zwraca skrót hasła użytkownika.
    const std::string &getPasswordHash() const
    {
        return passwordHash;
    }

    /// @brief Zwraca sól hasła zakodowaną w Base64.
    const std::string &getBase64salt() const
    {
        return base64salt;
    }

    /// @brief Porównuje dwóch użytkowników po nazwie użytkownika.
    bool operator==(const User &other) const;

    /**
     * @brief Serializuje użytkownika do obiektu JSON.
     * @return Obiekt JSON reprezentujący użytkownika.
     */
    json toJson() const;

    /**
     * @brief Deserializuje użytkownika z obiektu JSON.
     * @param j Obiekt JSON z danymi użytkownika.
     * @return Odtworzony obiekt użytkownika.
     */
    static User fromJson(const json &j);

private:
    std::string username;     ///< Nazwa użytkownika.
    std::string passwordHash; ///< Skrót hasła.
    std::string base64salt;   ///< Sól hasła w Base64.
};

namespace std
{
    template <>
    struct hash<User>
    {
        std::size_t operator()(const User &user) const
        {
            return std::hash<std::string>{}(user.getUsername());
        }
    };
}