#pragma once
#include <nlohmann/json.hpp>
using json = nlohmann::json;

/**
 * @brief Interface for classes that can be serialized to/from JSON.
 */
class JSONSerializable
{
public:
    virtual ~JSONSerializable() = default;

    /// @brief Serializes the object to JSON.
    virtual json toJson() const = 0;

    /// @brief Deserializes the object from JSON.
    virtual void fromJson(const json &j) = 0;
};