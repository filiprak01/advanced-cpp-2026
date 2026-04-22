#pragma once

#include <gtest/gtest.h>
#include <server/core/domain_result.hpp>

#define EXPECT_RESULT_SUCCESS(result_expr, expected_code)      \
    do                                                         \
    {                                                          \
        auto _result = (result_expr);                          \
        EXPECT_TRUE(_result.isSuccess())                       \
            << "Expected success, got: " << _result.getMessage(); \
        if (_result.isSuccess())                               \
        {                                                      \
            EXPECT_EQ(_result.getSuccessCode(), (expected_code)); \
        }                                                      \
    } while (false)

#define ASSERT_RESULT_SUCCESS(result_expr, expected_code)      \
    do                                                         \
    {                                                          \
        auto _result = (result_expr);                          \
        ASSERT_TRUE(_result.isSuccess())                       \
            << "Expected success, got: " << _result.getMessage(); \
        EXPECT_EQ(_result.getSuccessCode(), (expected_code));  \
    } while (false)

#define EXPECT_RESULT_DOMAIN_ERROR(result_expr, expected_code) \
    do                                                         \
    {                                                          \
        auto _result = (result_expr);                          \
        EXPECT_TRUE(_result.isDomainError())                   \
            << "Expected domain error, got: " << _result.getMessage(); \
        if (_result.isDomainError())                           \
        {                                                      \
            EXPECT_EQ(_result.getErrorCode(), (expected_code)); \
        }                                                      \
    } while (false)

#define EXPECT_RESULT_FORMAT_ERROR(result_expr, expected_code) \
    do                                                         \
    {                                                          \
        auto _result = (result_expr);                          \
        EXPECT_TRUE(_result.isFormatError())                   \
            << "Expected format error, got: " << _result.getMessage(); \
        if (_result.isFormatError())                           \
        {                                                      \
            EXPECT_EQ(_result.getErrorCode(), (expected_code)); \
        }                                                      \
    } while (false)
