#include <client/response_dispatcher.hpp>

void ResponseDispatcher::registerHandler(const std::string &type, Handler handler)
{
    handlers[type] = std::move(handler);
}

void ResponseDispatcher::setFallbackHandler(Handler handler)
{
    fallback = std::move(handler);
}

void ResponseDispatcher::dispatch(const json &message) const
{
    if (!message.contains("type") || !message["type"].is_string())
    {
        if (fallback)
            fallback(message);
        return;
    }

    const std::string type = message["type"].get<std::string>();
    auto it = handlers.find(type);
    if (it != handlers.end())
        it->second(message);
    else if (fallback)
        fallback(message);
}
