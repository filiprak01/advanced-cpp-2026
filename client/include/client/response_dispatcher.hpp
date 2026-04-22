#pragma once
#include <functional>
#include <string>
#include <unordered_map>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

/**
 * @brief Routes incoming server responses to registered handler functions.
 *
 * Register one handler per response @c type string.  Call dispatch() with
 * every JSON object received from the server; the matching handler (if any)
 * is invoked with the full message, including its @c status and @c payload.
 *
 * Expected response shape:
 * @code
 * { "type": "...", "status": "ok"|"error", "payload": { ... } }
 * @endcode
 */
class ResponseDispatcher
{
public:
    using Handler = std::function<void(const json &message)>;

    /**
     * @brief Registers a handler for the given response type.
     *
     * If a handler is already registered for @p type it is replaced.
     *
     * @param type    Response type string (e.g. "login_response").
     * @param handler Callable invoked with the full JSON message.
     */
    void registerHandler(const std::string &type, Handler handler);

    /**
     * @brief Dispatches a parsed JSON message to the matching handler.
     *
     * If the message has no @c type field, or no handler is registered for
     * that type, the optional fallback handler is called instead (if set).
     *
     * @param message Fully parsed JSON response from the server.
     */
    void dispatch(const json &message) const;

    /**
     * @brief Sets a catch-all handler for unrecognised response types.
     * @param handler Callable receiving any unmatched message.
     */
    void setFallbackHandler(Handler handler);

private:
    std::unordered_map<std::string, Handler> handlers;
    Handler fallback;
};
