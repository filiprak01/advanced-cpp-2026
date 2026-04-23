# MISSING.md — MVP gaps and required program pieces

## 1. Server backend MVP (what is missing)

1. Protocol alignment between server and client
   - Server receives newline-terminated JSON.
   - Client sends length-prefixed JSON frames.
   - These must be unified into one transport framing style.

2. Proper response envelope structure
   - Server returns raw strings like `login_ok`, `register_ok`, `unauthorized`.
   - Client expects JSON objects with `type`, `status`, and `payload`.
   - Add consistent JSON response objects for every server response.

3. Complete request handling and response payloads
   - `login` and `register` should return user info or error details.
   - `create_channel`, `add_message`, `edit_message`, etc. should return structured payloads.
   - `synchronize` should return full `channels` and `messages` state.

4. Session and authorization flow
   - The server accepts operations only after login, but session lifecycle is incomplete.
   - The session manager needs stronger validation and expiration support.

5. Channel/message broadcast / user-specific routing
   - Server currently sends responses only to the requesting client or user name mapping.
   - For channel messages, other participants should receive `new_message` events.

6. Persistence / data storage
   - There is no durable data storage or save/load path for users, channels, messages.
   - The `save_data` event exists in factory, but persistence is not fully wired.

7. Error handling and validation
   - Validations for malformed requests, missing payload fields, invalid channel IDs, and invalid user names.
   - Current event code often returns generic `unauthorized` or `*_failed` strings.

8. Testing coverage for server network layer
   - The existing integration tests cover basic protocol behavior, but not full route paths.
   - Missing tests for channel membership, message broadcast, sync payloads, and wrong packet framing.

## 2. Client backend MVP (what is missing)

1. Protocol framing mismatch
   - Client uses 4-byte length prefix.
   - Server uses newline-terminated strings.
   - Fix the transport framing before any end-to-end client-server MVP works.

2. Response parsing and state update gaps
   - `ClientBackend` updates only a small set of response types.
   - It does not handle `login_failed`, `register_failed`, `message_added`, `channel_create_failed`, etc.

3. Full `synchronize` semantics
   - `synchronize()` sends the request, but the server and client do not agree on `sync_response` payload shape.
   - Client needs to populate channels and messages from server-provided state.

4. Connection resilience
   - No reconnect / auto-reconnect logic.
   - No server disconnect detection with clear UI notification.

5. Error and status channel
   - The backend has no error/status queue separate from normal responses.
   - UI needs a stable way to display `error` and `info` messages.

6. Client tests for backend integration
   - `protocol_test.cpp` and `response_dispatcher_test.cpp` are present.
   - Missing tests for `ClientBackend` methods, queue behavior, handler registration, and connect/disconnect.

7. Message cache policy
   - The backend stores a flat `messages` map and evicts by map size.
   - Need per-channel eviction and order preservation.

## 3. Current missing files / structure for MVP

- `tests/client/client_backend_test.cpp`
- `tests/integration/client_server_integration_test.cpp`

These are the minimum structural placeholders needed for the MVP extension path.
