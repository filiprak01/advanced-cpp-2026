#pragma once
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/buffer.h>
#include <string>
#include <vector>

/**
 * @brief Narzędzie do generowania i weryfikacji skrótów haseł.
 *
 * Generuje kryptograficzną sól (salt) i skrót hasła przy użyciu OpenSSL.
 * Dzięki soleniu każde hasło ma unikalny skrót nawet przy tej samej wartości.
 */
class PasswordHasher
{
public:
    PasswordHasher() = default;

    /**
     * @brief Generuje losową sól zakodowaną w Base64.
     * @return Sól w postaci ciągu Base64.
     */
    const std::string generateBase64Salt();

    /**
     * @brief Generuje skrót hasła przy użyciu podanej soli.
     * @param password   Hasło w postaci jawnej.
     * @param base64salt Sól zakodowana w Base64.
     * @return Skrót hasła zakodowany w Base64.
     */
    const std::string generatePasswordHash(const std::string &password, const std::string &base64salt);

private:
    /**
     * @brief Koduje dane binarne do Base64.
     * @param input Dane do zakodowania.
     * @return Ciąg Base64.
     */
    std::string base64Encode(const std::vector<unsigned char> &input);

    /**
     * @brief Dekoduje ciąg Base64 do danych binarnych.
     * @param input Ciąg Base64 do zdekodowania.
     * @return Zdekodowane dane binarne.
     */
    std::vector<unsigned char> base64Decode(const std::string &input);
};
