# Functional Test Scenarios — PWChat (nc)

Start the server first:
```bash
./build/server/PWChat
```

Each session below is a separate terminal using `nc localhost 8090`.

---

## Scenario 1: Register three users

**Terminal A — register alice**
```bash
nc localhost 8090
{"type":"register","payload":{"username":"alice","password":"secret123"}}
# expected: register_ok
```

**Terminal B — register bob**
```bash
nc localhost 8090
{"type":"register","payload":{"username":"bob","password":"secret456"}}
# expected: register_ok
```

**Terminal C — register carol**
```bash
nc localhost 8090
{"type":"register","payload":{"username":"carol","password":"secret789"}}
# expected: register_ok
```

---

## Scenario 2: Login all three users

**Terminal A — login alice**
```json
{"type":"login","payload":{"username":"alice","password":"secret123"}}
```
Expected: `login_ok`

**Terminal B — login bob**
```json
{"type":"login","payload":{"username":"bob","password":"secret456"}}
```
Expected: `login_ok`

**Terminal C — login carol**
```json
{"type":"login","payload":{"username":"carol","password":"secret789"}}
```
Expected: `login_ok`

---

## Scenario 3: Create a private conversation between alice and bob

**Terminal A (alice)**
```json
{"type":"create_private_channel","payload":{"userNames":["alice","bob"]}}
```
Expected: `channel_created`

---

## Scenario 4: Alice sends a private message to bob (channel id 1)

**Terminal A (alice)**
```json
{"type":"add_message","payload":{"senderName":"alice","channelId":1,"content":"Hey bob, how are you?"}}
```
Expected: `message_added`

**Terminal B (bob) replies**
```json
{"type":"add_message","payload":{"senderName":"bob","channelId":1,"content":"Hi alice, doing well!"}}
```
Expected: `message_added`

---

## Scenario 5: Create a public channel for all three users

**Terminal A (alice)**
```json
{"type":"create_channel","payload":{"name":"general","userNames":["alice","bob","carol"],"isPrivate":false}}
```
Expected: `channel_created`

---

## Scenario 6: Send messages to the public channel (channel id 2)

**Terminal A (alice)**
```json
{"type":"add_message","payload":{"senderName":"alice","channelId":2,"content":"Welcome to general!"}}
```
Expected: `message_added`

**Terminal B (bob)**
```json
{"type":"add_message","payload":{"senderName":"bob","channelId":2,"content":"Thanks for the invite!"}}
```
Expected: `message_added`

**Terminal C (carol)**
```json
{"type":"add_message","payload":{"senderName":"carol","channelId":2,"content":"Hello everyone!"}}
```
Expected: `message_added`

---

## Scenario 7: Edit a message (message id 1)

**Terminal A (alice)**
```json
{"type":"edit_message","payload":{"messageId":1,"newContent":"Hey bob, how are you doing today?"}}
```
Expected: `message_edited`

---

## Scenario 8: Remove a message (message id 2)

**Terminal B (bob)**
```json
{"type":"remove_message","payload":{"messageId":2,"channelId":1}}
```
Expected: `message_removed`

---

## Scenario 9: Add carol to the private channel (channel id 1)

**Terminal A (alice)**
```json
{"type":"add_user_to_channel","payload":{"channelId":1,"userName":"carol"}}
```
Expected: `channel_joined` (sent to carol's connection)

---

## Scenario 10: Rename the public channel (channel id 2)

**Terminal A (alice)**
```json
{"type":"edit_channel","payload":{"channelId":2,"newName":"main"}}
```
Expected: `channel_edited`

---

## Scenario 11: Synchronize data for alice

**Terminal A (alice)**
```json
{"type":"synchronize","payload":{"clientFd":0,"userName":"alice"}}
```
Expected: `synchronize_ok`

---

## Scenario 12: Save server data

**Terminal A (alice)**
```json
{"type":"save_data","payload":{"filePath":"/tmp/pwchat_data.json"}}
```
Expected: `data_saved`

---

## Scenario 13: Remove the public channel (channel id 2)

**Terminal A (alice)**
```json
{"type":"remove_channel","payload":{"channelId":2}}
```
Expected: `channel_removed`

---

## Error Cases

**Wrong password login**
```json
{"type":"login","payload":{"username":"alice","password":"wrongpass"}}
```
Expected: `login_failed`

**Duplicate registration**
```json
{"type":"register","payload":{"username":"alice","password":"otherpass"}}
```
Expected: `register_failed`

**Unknown event type**
```json
{"type":"unknown_event","payload":{}}
```
Expected: no response (server logs unknown event)
