# Backend Implementation Guide — PWChat MVP

This guide provides a complete, step-by-step list of items to implement for the server and client backend to achieve a functional MVP. It addresses all gaps identified in MISSING.md, focusing on transport, protocol, state management, and testing.

## Overview

The MVP backend enables:
- End-to-end client-server communication with consistent framing and JSON protocol.
- User authentication, channel management, and messaging.
- State synchronization and error handling.
- Basic testing coverage.

**JSON Serialization Update:** Models (Channel, Message, User, Session) now inherit from `JSONSerializable` with `toJson()` and `void fromJson(const json&)`. The `Serializer` class has been removed; configs have their own `toJson()`/`fromJson()` methods. Use these for all serialization.

**Critical Path:** Start with Item 1 (transport framing) as it blocks all integration.

## Completed: Framing and JSON Protocol

Stages 1-3 are implemented.

- Transport framing is fixed in `server/src/core/connection_manager.cpp`:
  - `sendMessage()` now prefixes length using `htonl()` and sends the serialized JSON payload.
  - `receiveMessage()` now reads a 4-byte big-endian length header and then reads exactly that many bytes.
- Server event responses are now JSON objects in:
  - `server/src/auth/authentication_event.cpp`
  - `server/src/auth/registration_event.cpp`
  - `server/src/events/add_message_event.cpp`
  - `server/src/events/create_channel_event.cpp`
  - `server/src/events/edit_message_event.cpp`
  - `server/src/events/remove_message_event.cpp`
  - `server/src/events/edit_channel_event.cpp`
  - `server/src/events/remove_channel_event.cpp`
  - `server/src/events/add_user_to_channel_event.cpp`
  - `server/src/events/synchronize_data_event.cpp`
- Responses now use `type`, `status`, and `payload` and include structured errors.

## Stage Status Summary

| Stage | Status | File targets | Notes |
|---|---|---|---|
| Framing and JSON protocol | Done | `server/src/core/connection_manager.cpp`, `server/src/auth/authentication_event.cpp`, `server/src/auth/registration_event.cpp`, `server/src/events/add_message_event.cpp`, `server/src/events/create_channel_event.cpp`, `server/src/events/edit_message_event.cpp`, `server/src/events/remove_message_event.cpp`, `server/src/events/edit_channel_event.cpp`, `server/src/events/remove_channel_event.cpp`, `server/src/events/add_user_to_channel_event.cpp`, `server/src/events/synchronize_data_event.cpp` | Length-prefixed JSON and structured server responses are implemented |
| Session/auth flow and authorization | Pending | `server/src/core/session_manager.cpp`, `server/src/core/channel_manager.cpp`, `server/src/core/message_manager.cpp`, `server/src/events/*.cpp` | Add session expiry, auth checks, and permission enforcement |
| Channel/message broadcast | Pending | `server/src/core/message_manager.cpp`, `server/src/core/connection_manager.cpp`, `server/src/core/channel_manager.cpp` | Broadcast channel events to all members |
| Persistence | Pending | `server/src/core/server.cpp`, `server/src/events/save_data_event.cpp`, `server/src/repos/*.hpp`, `server/src/repos/*.cpp` | Add JSON persistence for repo data |
| Error handling / validation | Pending | `server/src/core/factory/event_factory.cpp`, `server/src/events/*.cpp` | Validate payloads and return structured errors |
| Client response handling | Pending | `client/src/client_backend.cpp` | Add handlers for all response types |
| Client sync semantics | Pending | `client/src/client_backend.cpp` | Parse `sync_response` payload arrays |
| Client resilience | Pending | `client/src/client_backend.cpp` | Add reconnect/disconnect handling |
| Client status channel | Pending | `client/src/client_backend.cpp` | Add status/error queue |
| Client message cache policy | Pending | `client/src/client_backend.cpp` | Implement channel message deque cache |
| Tests and integration | Pending | `tests/client/client_backend_test.cpp`, `tests/integration/client_server_integration_test.cpp`, `tests/integration/server_integration_test.cpp`, `tests/client/protocol_test.cpp` | Derived test scenarios in `TEST_ANALYSIS.md` |

## 1. Strengthen Session and Authorization Flow (Server)

**Why:** Session management is incomplete, risking unauthorized access.

**Steps:**
- In `server/src/auth/authentication_event.cpp` and `server/src/auth/registration_event.cpp`, ensure successful auth updates the connection mapping and session state.
- In each event handler under `server/src/events/*.cpp`, add checks: `if (!context.sessionManager.hasSession(userName)) return unauthorized; context.sessionManager.updateUserSession(userName);`.
- In `server/src/core/session_manager.cpp`, add session expiration cleanup that removes sessions older than `sessionTimeout`.
- In `server/src/core/channel_manager.cpp` and `server/src/core/message_manager.cpp`, enforce authorization: channel ops require channel membership; message edits/deletes require sender ownership.

**Related tests:** see `TEST_ANALYSIS.md` sections `4.2 Authentication and session flow` and `4.4 Event response protocol`.

**Explanation:** Ensures security and prevents stale sessions from allowing access.

## 2. Add Channel/Message Broadcast and User-Specific Routing (Server)

**Why:** Currently, responses only go to the requester; channel events need to reach all members.

**Steps:**
- In `message_manager.cpp`, after add/edit/remove, get channel members from `channelRepo` and broadcast to each except sender using `connectionManager.sendMessage(userName, json)`.
- For channel events, broadcast to all members.
- Ensure `connectionManager` maps `userName` to active connections.

**Explanation:** Enables real-time updates for all channel participants.

**Related tests:** see `TEST_ANALYSIS.md` sections `4.4 Event response protocol` and `4.5 Integration scenarios`.

## 3. Implement Persistence / Data Storage (Server, Optional for MVP)

**Why:** Data is lost on restart; MVP may need basic persistence.

**Steps:**
- In `server/src/core/server.cpp` constructor, load repositories from JSON files (e.g., `repo/users.json`) using repo `fromJson()` support.
- In `server/src/events/save_data_event.cpp`, implement serialization of repos to JSON and file storage.
- In `server/src/repos/*.cpp` and `server/src/repos/*.hpp`, add `toJson()` and `fromJson()` for collections of models, using `Channel::toJson()`, `Message::toJson()`, `User::toJson()`, etc.

**Explanation:** Allows server to persist state across restarts, though in-memory is sufficient for basic MVP.

## 4. Improve Error Handling and Validation (Server)

**Why:** Current errors are generic; need structured validation.

**Steps:**
- In event factories (`event_factory.cpp`), validate payload fields (e.g., `if (!payload.contains("username")) return error`).
- In handlers, check business rules (e.g., channel exists, user permissions).
- Send structured errors: `{"type": "error", "status": "error", "payload": {"reason": "missing_field"}}`.

**Explanation:** Provides better debugging and client-side error handling.

**Related tests:** see `TEST_ANALYSIS.md` sections `4.3 Event response protocol` and `4.5 Integration scenarios`.

## 5. Extend Client Backend Response Handling

**Why:** `ClientBackend` only handles a few response types.

**Steps:**
- In `client/src/client_backend.cpp`, extend `setupDefaultHandlers()`:
  - Add handlers for `login_failed`, `register_failed` (log or set error state).
  - `new_message`, `message_edited`, `message_removed`: update `messages` state in `client/src/client_backend.cpp`.
  - `channel_created`, `channel_edited`, `channel_removed`: update `channels` state in `client/src/client_backend.cpp`.
  - `sync_response`: clear and repopulate `channels` and `messages` from `payload` arrays.
  - `error`: handle generic errors and push them to a status queue.

**Explanation:** Allows client to react to all server events and maintain accurate local state.

## 6. Implement Full Synchronize Semantics (Client)

**Why:** Sync request exists, but client doesn't process the response fully.

**Steps:**
- In `client/src/client_backend.cpp`, `synchronize()` sends the request.
- In the `sync_response` handler, parse `payload["channels"]` and `payload["messages"]` to rebuild local caches.

**Explanation:** Enables clients to catch up on state after reconnect or initial login.

## 7. Add Connection Resilience (Client)

**Why:** No handling for disconnects or reconnections.

**Steps:**
- In `client/src/client_backend.cpp` `connect()`, wrap in a retry loop with `std::this_thread::sleep_for(std::chrono::milliseconds(config.getReconnectIntervalMs()))`.
- In `client/src/client_backend.cpp` `recvLoop()`, on `recv()` failure detect disconnect, set disconnected state, and trigger reconnect.
- Add `isReconnecting()` and `isConnected()` methods in `client/src/client_backend.cpp`.

**Explanation:** Makes client robust against network issues.

## 8. Add Error and Status Channel (Client)

**Why:** No way to report errors separately from data events.

**Steps:**
- Add `std::queue<std::string> statusQueue` to `ClientBackend`.
- Add `pushStatus(const std::string &msg)` and `std::vector<std::string> drainStatus()` to `client/include/client/client_backend.hpp`.
- Register an `error` handler in `client/src/client_backend.cpp` that pushes `payload.reason` into the status queue.
- Add tests in `tests/client/client_backend_test.cpp` for `error` response handling.

**Explanation:** Allows UI (in another repo) to display error messages and keep data handling separate.

## 9. Improve Message Cache Policy (Client)

**Why:** Current cache is a flat map; needs per-channel ordering and eviction.

**Steps:**
- In `client/src/client_backend.cpp`, change `messages` from `std::unordered_map<int, Message>` to `std::unordered_map<int, std::deque<Message>>`.
- On add, push new messages to the channel deque and evict from front when size exceeds `config.getMessageCacheSize()`.

**Explanation:** Better memory management and ordered display.

## 10. Add Client Backend Tests

**Why:** No tests for `ClientBackend` behavior.

**Steps:**
- In `tests/client/client_backend_test.cpp`:
  - Test `connect()` (mock socket).
  - Test `drainQueue()` (empty/full queues).
  - Test request methods (e.g., `login()` produces correct JSON).
  - Test response handling by injecting JSON directly into `responseDispatcher()`.
  - Test `login_response`, `new_message`, `message_edited`, `message_removed`, `sync_response`, and `error` handling.
  - Add `drainStatus()` validation for queued error messages.

**Explanation:** Ensures backend reliability and that response handlers keep local state consistent.

## 11. Add Integration Tests

**Why:** No end-to-end tests for client-server interaction.

**Steps:**
- In `tests/integration/client_server_integration_test.cpp`:
  - Start server, connect client, test: register, login, sync, send message, receive broadcasts.
  - Test framing consistency.
  - Test error scenarios.

**Explanation:** Validates full flows.

## 12. Update Existing Tests

**Why:** Tests need to match new protocol.

**Steps:**
- Update `tests/integration/server_integration_test.cpp` for JSON responses and length-prefixed framing.
- Add broadcast tests (multiple clients).
- Ensure `tests/client/protocol_test.cpp` aligns with server expectations.

**Explanation:** Keeps test suite current.

## Implementation Order

1. Start with 1-2 (session/auth flow and broadcast routing) now that transport and JSON framing are implemented.
2. Then 3-4 (persistence and validation).
3. Then 5-9 (client backend improvements).
4. Finally 10-12 (testing).

This completes the backend MVP. Each step includes file paths for direct implementation.