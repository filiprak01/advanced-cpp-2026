#pragma once
#include <string>
#include "message.hpp"
#include <nlohmann/json.hpp>
using json = nlohmann::json;

class Serializer
{
public:
    // Serializers
    json serialize(const Message &message);
    // Deserializers
    Message deserialize(const json &message);
};
