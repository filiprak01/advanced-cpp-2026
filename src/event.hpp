#pragma once 
#include <string>

class Event {
    public:
        virtual ~Event() = default;
        Event() = default;
        Event(const std::string& timestamp);
        const std::string& getTimestamp() const;
        const std::string& getEventId() const;
        void process(const std::string& timestamp);
    private:
        std::string timestamp_;
        std::string event_id;
};