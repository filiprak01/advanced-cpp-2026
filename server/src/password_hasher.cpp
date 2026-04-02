#include <server/auth/password_hasher.hpp>

std::vector<unsigned char> PasswordHasher::base64Decode(const std::string &input)
{
    std::vector<unsigned char> output;
    BIO *b64 = BIO_new(BIO_f_base64());
    BIO *sink = BIO_new_mem_buf(input.data(), input.size());
    BIO_push(b64, sink);
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    std::vector<unsigned char> decoded(input.size());
    int decodedLength = BIO_read(b64, decoded.data(), static_cast<int>(decoded.size()));
    if (decodedLength > 0)
    {
        decoded.resize(decodedLength);
    }
    else
    {
        decoded.clear();
    }
    BIO_free_all(b64);
    return decoded;
}
std::string PasswordHasher::base64Encode(const std::vector<unsigned char> &input)
{
    BIO *b64 = BIO_new(BIO_f_base64());
    BIO *sink = BIO_new(BIO_s_mem());
    BIO_push(b64, sink);
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    BIO_write(b64, input.data(), static_cast<int>(input.size()));
    BIO_flush(b64);
    BUF_MEM *bufferPtr;
    BIO_get_mem_ptr(b64, &bufferPtr);
    std::string encoded(bufferPtr->data, bufferPtr->length);
    BIO_free_all(b64);
    return encoded;
}

const std::string PasswordHasher::generateBase64Salt()
{
    std::vector<unsigned char> salt(16);
    if (RAND_bytes(salt.data(), static_cast<int>(salt.size())) != 1)
    {
        return "";
    }
    return base64Encode(salt);
}
const std::string PasswordHasher::generatePasswordHash(const std::string &password, const std::string &base64salt)
{
    std::vector<unsigned char> salt = base64Decode(base64salt);
    std::vector<unsigned char> hash(32);
    int iterations = 100000;

    int hashed = PKCS5_PBKDF2_HMAC(password.c_str(), static_cast<int>(password.size()), reinterpret_cast<unsigned char *>(salt.data()), static_cast<int>(salt.size()), iterations, EVP_sha256(), 32, hash.data());

    if (hashed != 32)
    {
        return "";
    }
    return base64Encode(hash);
}