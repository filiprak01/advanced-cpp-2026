#pragma once
#include <common/const/event_messages.hpp>
#include <string_view>
#include <variant>

class DomainResult
{
public:
    enum class Type
    {
        Success,
        FormatError,
        DomainError
    };

    static DomainResult success(success::Code code);
    static DomainResult formatError(errors::Code code);
    static DomainResult domainError(errors::Code code);

    bool isSuccess() const;
    bool isError() const;
    bool isFormatError() const;
    bool isDomainError() const;

    Type getType() const;
    success::Code getSuccessCode() const;
    errors::Code getErrorCode() const;
    std::string_view getMessage() const;
    std::string_view getResponseType() const;

private:
    DomainResult(Type type, success::Code code);
    DomainResult(Type type, errors::Code code);

    Type type;
    std::variant<success::Code, errors::Code> code;
};
