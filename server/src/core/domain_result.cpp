#include <server/core/domain_result.hpp>

DomainResult::DomainResult(Type type, success::Code code)
    : type(type), code(code) {}

DomainResult::DomainResult(Type type, errors::Code code)
    : type(type), code(code) {}

DomainResult DomainResult::success(success::Code code)
{
    return DomainResult(Type::Success, code);
}

DomainResult DomainResult::formatError(errors::Code code)
{
    return DomainResult(Type::FormatError, code);
}

DomainResult DomainResult::domainError(errors::Code code)
{
    return DomainResult(Type::DomainError, code);
}

bool DomainResult::isSuccess() const
{
    return type == Type::Success;
}

bool DomainResult::isError() const
{
    return !isSuccess();
}

bool DomainResult::isFormatError() const
{
    return type == Type::FormatError;
}

bool DomainResult::isDomainError() const
{
    return type == Type::DomainError;
}

DomainResult::Type DomainResult::getType() const
{
    return type;
}

success::Code DomainResult::getSuccessCode() const
{
    return std::get<success::Code>(code);
}

errors::Code DomainResult::getErrorCode() const
{
    return std::get<errors::Code>(code);
}

std::string_view DomainResult::getMessage() const
{
    if (isSuccess())
    {
        return success::toString(getSuccessCode());
    }

    return errors::toString(getErrorCode());
}

std::string_view DomainResult::getResponseType() const
{
    switch (type)
    {
    case Type::Success:
        return "success";
    case Type::FormatError:
        return "format_error";
    case Type::DomainError:
        return "domain_error";
    }

    return "error";
}
