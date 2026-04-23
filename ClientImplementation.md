# Client Implementation Plan — PWChat MVP

Focus: **send messages, join/create/edit channels, basic auth.** No nice-to-haves until this works end-to-end.

---

## Architecture overview

```
┌─────────────────────────────────────────┐
│  UI layer  (ncurses / Qt / web / TUI)   │
│  - channel list panel                   │
│  - message panel                        │
│  - input bar                            │
└───────────────┬─────────────────────────┘
                │ calls
┌───────────────▼─────────────────────────┐
│  ClientBackend  (C++ class)             │
│  - holds socket fd                      │
│  - send(json)  /  recv loop             │
│  - dispatches responses to handlers     │
└───────────────┬─────────────────────────┘
                │ TCP/socket
          PWChat server
```

The backend and UI live in the same process. The recv loop runs in a background thread; responses are posted to a queue which the UI thread drains on each render tick.

---

## Reuse from `common/`

| Common class          | Used for                                      |
|-----------------------|-----------------------------------------------|
| `Channel`             | local cache, `toJson` / `fromJson`            |
| `Message`             | local cache, `toJson` / `fromJson`            |
| `User`                | display name, `toJson` / `fromJson`           |
| `Session`             | store current session locally                 |
| `ClientConfig`        | load host/port/settings from JSON config file |
| `Serializer`          | serialize `ClientConfig` to/from file         |
| `channel_repo`        | optional local channel cache                  |
| `message_repo`        | optional local message cache                  |

---

## Client-side component list

### 1. `Connection` (wrap the socket)
- `connect(host, port)` → opens TCP socket
- `send(const json &)` → serialise + write
- `disconnect()`
- Can reuse `common/models/connection.hpp` as a model; wrap it in a `SocketConnection` that owns the fd.

### 2. `ClientBackend`
Owns a `Connection`, a response queue, and the local state caches.

```
class ClientBackend {
  Connection conn;
  std::queue<json> incomingQueue;   // written by recv thread, read by UI
  std::mutex queueMutex;

  // local state
  std::unordered_map<int, Channel>  channels;
  std::unordered_map<int, Message>  messages;
  std::string currentUser;

  void recvLoop();   // background thread
  void send(const json &);

  // high-level API called by UI
  bool login(string user, string pass);
  bool registerUser(string user, string pass);
  void sendMessage(int channelId, string content);
  void editMessage(int messageId, string newContent);
  void removeMessage(int messageId, int channelId);
  void createChannel(string name, vector<string> members, bool isPrivate);
  void editChannel(int channelId, string newName);
  void removeChannel(int channelId);
  void addUserToChannel(int channelId, string userName);
  void synchronize();
};
```

### 3. `ResponseDispatcher`
Maps `response.type` → handler function. Keeps `ClientBackend` clean.

```cpp
using Handler = std::function<void(const json &payload)>;
std::unordered_map<std::string, Handler> handlers;
```

Suggested handlers (one per response type from `suggestions.md`):

| Response type         | Action                                              |
|-----------------------|-----------------------------------------------------|
| `login_response`      | store `currentUser`, trigger sync, update UI        |
| `register_response`   | show success / error                                |
| `new_message`         | add to local message cache, repaint message panel   |
| `message_edited`      | update cache entry, repaint                         |
| `message_removed`     | remove from cache, repaint                          |
| `channel_created`     | insert into channel cache, repaint list             |
| `channel_edited`      | update name in cache, repaint list                  |
| `channel_removed`     | remove from cache, repaint list                     |
| `user_joined_channel` | update channel member list                          |
| `sync_response`       | bulk-load channels + messages, full repaint         |
| `error`               | show status bar message                             |

### 4. `ClientConfig` usage
Load from `config/client.json` using the existing `ClientConfig::readFromFile`. Relevant fields:
- `serverIp` / `serverPort`
- `autoReconnect` + `reconnectIntervalMs`
- `maxMessageLength`
- `sendOnEnter`
- `messageCacheSize`

Example config file (`config/client.json`):
```json
{
  "serverIp": 2130706433,
  "serverPort": 8090,
  "autoReconnect": true,
  "reconnectIntervalMs": 3000,
  "maxMessageLength": 1000,
  "sendOnEnter": true,
  "messageCacheSize": 200,
  "showConnectionStatus": true
}
```

> **Note:** `serverIp` is stored as a 32-bit integer in the existing model. Consider changing it to `std::string` to hold a dotted-decimal address — that is a one-line change in `ClientConfig`.

### 5. UI layer (choose one, ordered by effort)

#### Option A — ncurses / FTXUI (recommended for MVP)
- `FTXUI` is a header-only TUI library that integrates cleanly with a C++ backend.
- Three panels: channel list (left), messages (centre), input bar (bottom).
- Render tick: drain `incomingQueue`, call `screen.RequestAnimationFrame()`.
- No web server, no extra dependencies beyond the library.

#### Option B — Dear ImGui + SDL2
- Immediate-mode GUI, easy to prototype.
- Higher binary size; requires SDL2.

#### Option C — REST bridge + web frontend
- Add a small HTTP server (cpp-httplib, single header) to `ClientBackend`.
- Frontend in plain HTML/JS (fetch + WebSocket or polling).
- Most portable; most work.

---

## MVP step-by-step

### Step 1 — Socket wrapper
- [ ] Create `client/include/client/socket_connection.hpp` + `src/socket_connection.cpp`
- [ ] `connect()`, `send(string)`, `recv(string &)`, `disconnect()`
- [ ] Unit test: loopback echo server

### Step 2 — JSON protocol helpers
- [ ] `client/include/client/protocol.hpp` — free functions `buildLoginRequest(user, pass)`, `buildMessageRequest(...)`, etc., one per type from `suggestions.md`
- [ ] Unit test: verify JSON shape of each builder

### Step 3 — ClientBackend (no UI)
- [ ] Implement `ClientBackend` as described above
- [ ] Start background recv thread in `connect()`
- [ ] `ResponseDispatcher` with handlers that update local caches
- [ ] Integration test: connect to a running server, login, send a message, receive echo

### Step 4 — CLI smoke test
- [ ] `client/src/main.cpp` reads config, connects, logs in, sends hardcoded message, prints response — confirms end-to-end works

### Step 5 — TUI (FTXUI)
- [ ] Add FTXUI via CMake FetchContent (single dependency)
- [ ] Channel list component: reads `ClientBackend::channels`
- [ ] Message list component: reads `ClientBackend::messages` filtered by selected channel
- [ ] Input component: on Enter calls `ClientBackend::sendMessage()`
- [ ] Render loop polls `incomingQueue` every 16 ms

### Step 6 — Channel management UI
- [ ] Modal / inline form: create channel (name + member list)
- [ ] Inline rename: edit channel name
- [ ] Confirm dialog: remove channel
- [ ] Add user: type username → `addUserToChannel`

### Step 7 — Message editing
- [ ] Double-click / keybind on own message → edit inline
- [ ] Delete keybind → confirm → `removeMessage`

### Step 8 — Reconnect & sync
- [ ] On disconnect: wait `reconnectIntervalMs`, retry up to N times
- [ ] On reconnect: send `synchronize` to restore state

---

## Suggested patterns

### Thread safety
- One mutex guards `incomingQueue`. UI thread pops, recv thread pushes.
- Local caches (`channels`, `messages`) are **only written** by the UI thread (after popping from queue), so no second lock is needed.

### Error display
- `status: "error"` responses push a `StatusMessage{text, timestamp}` to a small ring buffer; the status bar renders the most recent one.

### Message cache eviction
- Keep only the last `messageCacheSize` messages per channel (from `ClientConfig`). Evict oldest on insert.

### Auto-reconnect pattern
```cpp
while (autoReconnect && !shouldStop) {
    try {
        conn.connect(host, port);
        sendSynchronize();
        recvLoop();           // blocks until disconnect
    } catch (...) {
        std::this_thread::sleep_for(reconnectInterval);
    }
}
```

---

## CMake additions needed

```cmake
# client/CMakeLists.txt
add_executable(PWChatClient src/main.cpp src/socket_connection.cpp src/client_backend.cpp)
target_link_libraries(PWChatClient PRIVATE commonLib)

# Optional TUI
include(FetchContent)
FetchContent_Declare(ftxui GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI GIT_TAG v5.0.0)
FetchContent_MakeAvailable(ftxui)
target_link_libraries(PWChatClient PRIVATE ftxui::screen ftxui::dom ftxui::component)
```
