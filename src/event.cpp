#include "event.hpp"

Event::Event(const std::string& timestamp) : timestamp_(timestamp) {}
void Event::process(const std::string& timestamp)
{
    timestamp_ = timestamp;
}
const std::string& Event::getTimestamp() const
{
    return timestamp_;
}
const std::string& Event::getEventId() const
{
    return event_id;
}