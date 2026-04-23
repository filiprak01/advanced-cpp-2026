#pragma once
#include <server/core/event.hpp>
#include <server/core/domain_result.hpp>
#include <server/core/manager_context.hpp>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class InvalidEvent : public Event
{
public:
    explicit InvalidEvent(DomainResult result) : result(result) {}
    void perform(ManagerContext &context, int clientFd) override;

private:
    DomainResult result;
};
