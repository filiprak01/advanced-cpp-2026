#pragma once
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/buffer.h>
#include <string>
#include <vector>
class PasswordHasher
{
    PasswordHasher() = default;

private:
    std::string base64Encode(const std::vector<unsigned char> &input);
    std::vector<unsigned char> base64Decode(const std::string &input);

public:
    const std::string generateBase64Salt();
    const std::string generatePasswordHash(const std::string &password, const std::string &base64salt);
};
