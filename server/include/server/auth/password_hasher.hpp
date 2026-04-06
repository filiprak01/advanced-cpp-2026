#pragma once
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/buffer.h>
#include <string>
#include <vector>
class PasswordHasher
{
public:
    PasswordHasher() = default;

    const std::string generateBase64Salt();
    const std::string generatePasswordHash(const std::string &password, const std::string &base64salt);

private:
    std::string base64Encode(const std::vector<unsigned char> &input);
    std::vector<unsigned char> base64Decode(const std::string &input);
};
