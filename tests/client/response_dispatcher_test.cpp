/**
 * @file response_dispatcher_test.cpp
 * @brief Testy jednostkowe ResponseDispatcher.
 *
 * Sprawdza rejestrację handlerów, ich wywoływanie, fallback oraz
 * zachowanie przy brakujących polach w wiadomości.
 */
#include <gtest/gtest.h>
#include <client/response_dispatcher.hpp>

// ---------------------------------------------------------------------------
// Rejestracja i dispatch
// ---------------------------------------------------------------------------

/// @test Zarejestrowany handler jest wywoływany dla pasującego typu.
TEST(ResponseDispatcherTest, RegisteredHandlerIsCalledForMatchingType)
{
    ResponseDispatcher dispatcher;
    bool called = false;

    dispatcher.registerHandler("login_response", [&](const json &)
                               { called = true; });

    dispatcher.dispatch({{"type", "login_response"}, {"status", "ok"}});
    EXPECT_TRUE(called);
}

/// @test Handler nie jest wywoływany dla innego typu.
TEST(ResponseDispatcherTest, HandlerNotCalledForDifferentType)
{
    ResponseDispatcher dispatcher;
    bool called = false;

    dispatcher.registerHandler("login_response", [&](const json &)
                               { called = true; });

    dispatcher.dispatch({{"type", "register_response"}, {"status", "ok"}});
    EXPECT_FALSE(called);
}

/// @test Handler otrzymuje pełny obiekt JSON.
TEST(ResponseDispatcherTest, HandlerReceivesFullMessage)
{
    ResponseDispatcher dispatcher;
    json received;

    dispatcher.registerHandler("test_event", [&](const json &msg)
                               { received = msg; });

    json expected = {{"type", "test_event"}, {"status", "ok"}, {"payload", {{"data", 42}}}};
    dispatcher.dispatch(expected);

    EXPECT_EQ(received, expected);
}

/// @test Wielokrotna rejestracja zastępuje poprzedni handler.
TEST(ResponseDispatcherTest, ReregisteringHandlerReplacesOld)
{
    ResponseDispatcher dispatcher;
    int callCount = 0;

    dispatcher.registerHandler("event", [&](const json &)
                               { callCount = 1; });
    dispatcher.registerHandler("event", [&](const json &)
                               { callCount = 2; });

    dispatcher.dispatch({{"type", "event"}});
    EXPECT_EQ(callCount, 2);
}

/// @test Wiele różnych handlerów działa niezależnie.
TEST(ResponseDispatcherTest, MultipleHandlersWorkIndependently)
{
    ResponseDispatcher dispatcher;
    bool aFired = false, bFired = false;

    dispatcher.registerHandler("type_a", [&](const json &)
                               { aFired = true; });
    dispatcher.registerHandler("type_b", [&](const json &)
                               { bFired = true; });

    dispatcher.dispatch({{"type", "type_a"}});
    EXPECT_TRUE(aFired);
    EXPECT_FALSE(bFired);

    dispatcher.dispatch({{"type", "type_b"}});
    EXPECT_TRUE(bFired);
}

// ---------------------------------------------------------------------------
// Fallback
// ---------------------------------------------------------------------------

/// @test Fallback jest wywoływany gdy nie ma pasującego handlera.
TEST(ResponseDispatcherTest, FallbackCalledForUnknownType)
{
    ResponseDispatcher dispatcher;
    bool fallbackCalled = false;

    dispatcher.setFallbackHandler([&](const json &)
                                  { fallbackCalled = true; });

    dispatcher.dispatch({{"type", "some_unknown_type"}});
    EXPECT_TRUE(fallbackCalled);
}

/// @test Fallback jest wywoływany gdy wiadomość nie ma pola "type".
TEST(ResponseDispatcherTest, FallbackCalledWhenTypeFieldMissing)
{
    ResponseDispatcher dispatcher;
    bool fallbackCalled = false;

    dispatcher.setFallbackHandler([&](const json &)
                                  { fallbackCalled = true; });

    dispatcher.dispatch({{"status", "ok"}, {"data", 123}});
    EXPECT_TRUE(fallbackCalled);
}

/// @test Fallback nie jest wywoływany gdy pasuje zarejestrowany handler.
TEST(ResponseDispatcherTest, FallbackNotCalledWhenHandlerMatches)
{
    ResponseDispatcher dispatcher;
    bool fallbackCalled = false;

    dispatcher.registerHandler("known_event", [](const json &) {});
    dispatcher.setFallbackHandler([&](const json &)
                                  { fallbackCalled = true; });

    dispatcher.dispatch({{"type", "known_event"}});
    EXPECT_FALSE(fallbackCalled);
}

/// @test Bez fallbacku dispatch nieznanego typu nie rzuca wyjątku.
TEST(ResponseDispatcherTest, NoFallbackNoThrowForUnknownType)
{
    ResponseDispatcher dispatcher;
    EXPECT_NO_THROW(dispatcher.dispatch({{"type", "no_handler_for_this"}}));
}

/// @test Bez fallbacku dispatch wiadomości bez pola "type" nie rzuca wyjątku.
TEST(ResponseDispatcherTest, NoFallbackNoThrowForMissingType)
{
    ResponseDispatcher dispatcher;
    EXPECT_NO_THROW(dispatcher.dispatch({{"status", "ok"}}));
}

// ---------------------------------------------------------------------------
// Pole "type" niebędące stringiem
// ---------------------------------------------------------------------------

/// @test Fallback jest wywoływany gdy pole "type" nie jest stringiem.
TEST(ResponseDispatcherTest, FallbackCalledWhenTypeIsNotString)
{
    ResponseDispatcher dispatcher;
    bool fallbackCalled = false;

    dispatcher.setFallbackHandler([&](const json &)
                                  { fallbackCalled = true; });

    dispatcher.dispatch({{"type", 42}});
    EXPECT_TRUE(fallbackCalled);
}
