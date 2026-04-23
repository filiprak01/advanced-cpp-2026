# Event Type Suggestions for EventFactory

All messages are JSON objects sent over the socket. Every message has a top-level `type` string and a `payload` object.

---

## Client → Server requests

### Already implemented

#### `login`
```json
{
  "type": "login",
  "payload": {
    "username": "alice",
    "password": "secret123"
  }
}
```

#### `register`
```json
{
  "type": "register",
  "payload": {
    "username": "alice",
    "password": "secret123"
  }
}
```

#### `message`  *(legacy, keep for back-compat)*
```json
{
  "type": "message",
  "payload": {
    "senderName": "alice",
    "channelId": 1,
    "content": "Hello, world!"
  }
}
```

---

### Suggested new types

#### `add_message`
Preferred replacement for `message`. Timestamp is set by the server, not the client.
```json
{
  "type": "add_message",
  "payload": {
    "senderName": "alice",
    "channelId": 1,
    "content": "Hello, world!"
  }
}
```

#### `edit_message`
```json
{
  "type": "edit_message",
  "payload": {
    "messageId": 42,
    "newContent": "Hello, updated world!"
  }
}
```

#### `remove_message`
```json
{
  "type": "remove_message",
  "payload": {
    "messageId": 42,
    "channelId": 1
  }
}
```

#### `create_channel`
```json
{
  "type": "create_channel",
  "payload": {
    "name": "general",
    "userNames": ["alice", "bob"],
    "isPrivate": false
  }
}
```

#### `create_private_channel`
Convenience alias — sets `isPrivate: true`, requires exactly 2 userNames.
```json
{
  "type": "create_private_channel",
  "payload": {
    "userNames": ["alice", "bob"]
  }
}
```

#### `edit_channel`
```json
{
  "type": "edit_channel",
  "payload": {
    "channelId": 3,
    "newName": "general-chat"
  }
}
```

#### `remove_channel`
```json
{
  "type": "remove_channel",
  "payload": {
    "channelId": 3
  }
}
```

#### `add_user_to_channel`
```json
{
  "type": "add_user_to_channel",
  "payload": {
    "channelId": 3,
    "userName": "charlie"
  }
}
```

#### `synchronize`
Sent by client after login to get full state (channels + recent messages).
```json
{
  "type": "synchronize",
  "payload": {
    "clientFd": 7,
    "userName": "alice"
  }
}
```

#### `save_data`  *(admin / internal)*
```json
{
  "type": "save_data",
  "payload": {
    "filePath": "/var/pwchat/data.json"
  }
}
```

---

## Server → Client responses

Every response has a `status` (`"ok"` or `"error"`) and an optional `payload`.

### `login`
```json
{ "type": "login_response", "status": "ok",    "payload": { "userName": "alice" } }
{ "type": "login_response", "status": "error",  "payload": { "message": "Invalid credentials" } }
```

### `register`
```json
{ "type": "register_response", "status": "ok",   "payload": { "userName": "alice" } }
{ "type": "register_response", "status": "error", "payload": { "message": "Username already taken" } }
```

### `add_message` / `message`
Broadcast to all members of the channel, including sender (for confirmation).
```json
{
  "type": "new_message",
  "payload": {
    "messageId": 99,
    "channelId": 1,
    "senderName": "alice",
    "content": "Hello, world!",
    "timestampMs": 1712500000000
  }
}
```
Error (sent only to requester):
```json
{ "type": "add_message_response", "status": "error", "payload": { "message": "Channel not found" } }
```

### `edit_message`
Broadcast to channel members:
```json
{ "type": "message_edited", "payload": { "messageId": 42, "newContent": "Hello, updated world!" } }
```

### `remove_message`
Broadcast to channel members:
```json
{ "type": "message_removed", "payload": { "messageId": 42, "channelId": 1 } }
```

### `create_channel`
Sent to all initial members:
```json
{
  "type": "channel_created",
  "payload": {
    "channelId": 5,
    "name": "general",
    "isPrivate": false,
    "userNames": ["alice", "bob"],
    "messageIds": []
  }
}
```

### `edit_channel`
Broadcast to channel members:
```json
{ "type": "channel_edited", "payload": { "channelId": 3, "newName": "general-chat" } }
```

### `remove_channel`
Broadcast to channel members:
```json
{ "type": "channel_removed", "payload": { "channelId": 3 } }
```

### `add_user_to_channel`
Broadcast to existing channel members + new user:
```json
{ "type": "user_joined_channel", "payload": { "channelId": 3, "userName": "charlie" } }
```

### `synchronize`
Sent only to requesting client:
```json
{
  "type": "sync_response",
  "payload": {
    "channels": [
      {
        "channelId": 1,
        "name": "general",
        "isPrivate": false,
        "userNames": ["alice", "bob"],
        "messageIds": [1, 2, 3]
      }
    ],
    "messages": [
      {
        "messageId": 1,
        "channelId": 1,
        "senderName": "alice",
        "content": "Hello",
        "timestampMs": 1712500000000
      }
    ]
  }
}
```

### Generic error (unknown type, malformed JSON, etc.)
```json
{ "type": "error", "payload": { "message": "Unknown event type: foo" } }
```

---

## Suggested `EventFactory` type string mapping

| JSON `type`            | C++ Event class           | Notes                              |
|------------------------|---------------------------|------------------------------------|
| `login`                | `AuthenticationEvent`     | already implemented                |
| `register`             | `RegistrationEvent`       | already implemented                |
| `message`              | `MessageEvent`            | already implemented, keep for compat |
| `add_message`          | `AddMessageEvent`         | preferred replacement for `message`|
| `edit_message`         | `EditMessageEvent`        |                                    |
| `remove_message`       | `RemoveMessageEvent`      |                                    |
| `create_channel`       | `CreateChannelEvent`      | `isPrivate` from payload           |
| `create_private_channel` | `CreateChannelEvent`    | `isPrivate` forced true            |
| `edit_channel`         | `EditChannelEvent`        |                                    |
| `remove_channel`       | `RemoveChannelEvent`      |                                    |
| `add_user_to_channel`  | `AddUserToChannelEvent`   |                                    |
| `synchronize`          | `SynchronizeDataEvent`    |                                    |
| `save_data`            | `SaveDataEvent`           |                                    |
