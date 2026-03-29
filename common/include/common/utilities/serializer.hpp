#pragma once
#include <string>
#include <common/models/message.hpp>
#include <common/utilities/config.hpp>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

class Serializer
{
public:
    // Serializers
    json serialize(const Message &message);
    json serialize(const ServerConfig &config);
    json serialize(const ClientConfig &config);
    // Deserializers
    Message deserializeMsg(const json &message);
    ServerConfig deserializeServerConfig(const json &config);
    ClientConfig deserializeClientConfig(const json &config);
};
