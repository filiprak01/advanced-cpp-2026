# Analiza testów jednostkowych — przypadki testowe

## 1. Modele (`tests/models/`)

### 1.1 `User` (`user_test.cpp`)

| # | Przypadek testowy | Opis |
|---|---|---|
| 1 | Domyślny konstruktor | `User()` — `getUsername()`, `getPasswordHash()`, `getBase64salt()` powinny zwracać puste stringi |
| 2 | Konstruktor z parametrami | Sprawdzenie poprawnego przypisania `username`, `passwordHash`, `base64salt` |
| 3 | Operator `==` — równe | Dwóch użytkowników z tym samym `username` powinno być równe |
| 4 | Operator `==` — różne | Dwóch użytkowników z różnymi `username` powinno być różne |
| 5 | Hash (std::hash) — spójność | Ten sam użytkownik powinien mieć ten sam hash |
| 6 | Hash — różny dla różnych | Różni użytkownicy powinni mieć różne hashe |

**Przypadki brzegowe:**
- Pusty `username` w konstruktorze
- Bardzo długi `username` (np. 10000 znaków)
- Znaki specjalne i UTF-8 w `username`

---

### 1.2 `Message` (`message_test.cpp`)

| # | Przypadek testowy | Opis |
|---|---|---|
| 1 | Domyślny konstruktor | `Message()` — sprawdzenie wartości domyślnych |
| 2 | Konstruktor z parametrami | Sprawdzenie `id`, `content`, `senderName`, `timestamp` |
| 3 | `withContent()` | Zmiana treści — kopia z nową treścią i zaktualizowanym timestamp |
| 4 | `withContent()` — nie modyfikuje oryginału | Oryginalna wiadomość nie powinna się zmienić |
| 5 | Operator `==` — porównanie po `id` | Wiadomości z tym samym `id` powinny być równe |
| 6 | Operator `==` — różne `id` | Wiadomości z różnymi `id` powinny nie być równe |
| 7 | Hash — spójność po `id` | `std::hash<Message>` zwraca ten sam wynik dla tego samego `id` |

**Przypadki brzegowe:**
- Pusty `content`
- Bardzo długi `content`
- `id` ujemny lub zero
- `senderName` pusty

---

### 1.3 `Channel` (`channel_test.cpp`)

| # | Przypadek testowy | Opis |
|---|---|---|
| 1 | Domyślny konstruktor | Wartości domyślne |
| 2 | Konstruktor — kanał publiczny | `isPrivate=false`, `maxUsers=32` |
| 3 | Konstruktor — kanał prywatny | `isPrivate=true`, `maxUsers=2` |
| 4 | `addMessageId()` | Dodanie ID wiadomości do kanału |
| 5 | `addMessageId()` — duplikat | Duplikat nie powinien być dodany |
| 6 | `addUserId()` | Dodanie ID użytkownika |
| 7 | `addUserId()` — duplikat | `unordered_set` ignoruje duplikat |
| 8 | `removeUserId()` | Usunięcie istniejącego użytkownika |
| 9 | `removeUserId()` — nieistniejący | Usunięcie nieistniejącego — brak efektu |
| 10 | `removeMessageId()` | Usunięcie istniejącej wiadomości |
| 11 | `removeMessageId()` — nieistniejąca | Brak efektu |
| 12 | `withName()` | Zmiana nazwy — zwraca kopię z nową nazwą |
| 13 | `toggleActive()` | Zmiana statusu aktywności |
| 14 | `toggleActive()` dwukrotnie | Podwójne przełączenie — powrót do oryginału |
| 15 | Operator `==` | Porównanie po `channelId` |
| 16 | Niemutowalność | Metody zwracają kopie, oryginał niezmieniony |

**Przypadki brzegowe:**
- Kanał z pustą listą użytkowników
- Kanał z pustą listą wiadomości
- Pusta nazwa kanału
- `channelId` ujemny lub zero

---

### 1.4 `Session` (`session_test.cpp`)

| # | Przypadek testowy | Opis |
|---|---|---|
| 1 | Domyślny konstruktor | Wartości domyślne |
| 2 | Konstruktor z parametrami | Sprawdzenie `sessionId`, `userId`, `lastActive` |
| 3 | `withNewLastActive()` | Nowa sesja z zaktualizowanym `lastActive` |
| 4 | `withNewLastActive()` — oryginał niezmieniony | Oryginał zachowuje stary `lastActive` |
| 5 | Operator `==` | Porównanie po `sessionId` |

**Przypadki brzegowe:**
- `sessionId` lub `userId` ujemny
- `lastActive` w przeszłości lub przyszłości

---

### 1.5 `Connection` (`connection_test.cpp`)

| # | Przypadek testowy | Opis |
|---|---|---|
| 1 | Konstruktor z parametrami | Sprawdzenie `socket` i `session` |
| 2 | Operator `==` | Porównanie po `socket` |
| 3 | Hash — spójność | `std::hash<Connection>` oparte na `socket` |

**Przypadki brzegowe:**
- Pusty `socket`

---

## 2. Repozytoria (`tests/repos/`)

### 2.1 `UserRepository` (`user_repo_test.cpp`)

| # | Przypadek testowy | Opis |
|---|---|---|
| 1 | `addUser()` — sukces | Dodanie nowego użytkownika zwraca `true` |
| 2 | `addUser()` — duplikat | Dodanie istniejącego użytkownika zwraca `false` |
| 3 | `removeUser()` — sukces | Usunięcie istniejącego zwraca `true` |
| 4 | `removeUser()` — nieistniejący | Usunięcie nieistniejącego zwraca `false` |
| 5 | `getUser()` — istniejący | Zwraca poprawnego użytkownika |
| 6 | `getUser()` — nieistniejący | Zwraca domyślny `User()` |
| 7 | `userExists()` — true/false | Sprawdzenie obu wariantów |
| 8 | `getAllUsers()` — puste repo | Zwraca pusty wektor |
| 9 | `getAllUsers()` — z użytkownikami | Zwraca wszystkich dodanych |
| 10 | Dodanie, usunięcie, ponowne dodanie | Cykl życia użytkownika |

**Przypadki brzegowe:**
- Operacje na pustym repozytorium
- Wielokrotne dodanie/usunięcie tego samego użytkownika
- Pusty `username`

---

### 2.2 `ChannelRepository` (`channel_repo_test.cpp`)

| # | Przypadek testowy | Opis |
|---|---|---|
| 1 | `addChannel()` — sukces | Dodanie kanału |
| 2 | `removeChannel()` — sukces | Usunięcie kanału |
| 3 | `getChannel()` — istniejący | Zwraca poprawny kanał |
| 4 | `channelExists()` — true/false | Sprawdzenie istnienia |
| 5 | `isChannelActive()` | Sprawdzenie aktywności |
| 6 | `isChannelPrivate()` | Sprawdzenie prywatności |
| 7 | `updateChannel()` | Aktualizacja danych kanału |

**Przypadki brzegowe:**
- `getChannel()` z nieistniejącym ID
- `isChannelActive()` / `isChannelPrivate()` z nieistniejącym ID (potencjalny undefined behavior)
- `removeChannel()` z nieistniejącym ID

---

### 2.3 `MessageRepository` (`message_repo_test.cpp`)

| # | Przypadek testowy | Opis |
|---|---|---|
| 1 | `addMessage()` — sukces | Dodanie wiadomości |
| 2 | `removeMessage()` — sukces | Usunięcie wiadomości |
| 3 | `getMessage()` — istniejąca | Zwraca poprawną wiadomość |
| 4 | `messageExists()` — true/false | Sprawdzenie istnienia |
| 5 | `updateMessage()` | Aktualizacja treści wiadomości |

**Przypadki brzegowe:**
- `getMessage()` z nieistniejącym ID
- `updateMessage()` z nieistniejącym ID (nadpisze lub stworzy nowy wpis)
- `removeMessage()` z nieistniejącym ID

---

### 2.4 `SessionRepository` (`session_repo_test.cpp`)

| # | Przypadek testowy | Opis |
|---|---|---|
| 1 | `addSession()` — sukces | Dodanie sesji |
| 2 | `removeSession()` — sukces | Usunięcie sesji |
| 3 | `getSession()` — istniejąca | Zwraca poprawną sesję |
| 4 | `sessionExists()` — true/false | Sprawdzenie istnienia |
| 5 | `updateSession()` | Aktualizacja sesji |

**Przypadki brzegowe:**
- `getSession()` z nieistniejącym ID
- `updateSession()` z nieistniejącym ID

---

## 3. Managery (`tests/managers/`)

### 3.1 `AuthManager` (`auth_manager_test.cpp`)

| # | Przypadek testowy | Opis |
|---|---|---|
| 1 | `authenticate()` — poprawne dane | Zwraca `true` dla prawidłowego hasła |
| 2 | `authenticate()` — złe hasło | Zwraca `false` |
| 3 | `authenticate()` — nieistniejący użytkownik | Zwraca `false` (pusty `username` z `getUser()`) |
| 4 | `authenticate()` — pusty username | Zwraca `false` |
| 5 | `authenticate()` — puste hasło | Zwraca `false` |

**Przypadki brzegowe:**
- Użytkownik z pustym hashem hasła
- Bardzo długie hasło

---

### 3.2 `RegistrationManager` (`registration_manager_test.cpp`)

| # | Przypadek testowy | Opis |
|---|---|---|
| 1 | `registerUser()` — sukces | Prawidłowe dane — zwraca `true` |
| 2 | `registerUser()` — hasło za krótkie | Zwraca `false` |
| 3 | `registerUser()` — hasło za długie | Zwraca `false` |
| 4 | `registerUser()` — username za krótki | Zwraca `false` |
| 5 | `registerUser()` — username za długi | Zwraca `false` |
| 6 | `registerUser()` — duplikat | Próba rejestracji istniejącego użytkownika — `false` |
| 7 | `registerUser()` — minimalna długość hasła | Hasło dokładnie `passwordMinLength` znaków |
| 8 | `registerUser()` — maksymalna długość hasła | Hasło dokładnie `passwordMaxLength` znaków |
| 9 | `registerUser()` — minimalna długość username | Username dokładnie `usernameMinLength` znaków |
| 10 | `registerUser()` — maksymalna długość username | Username dokładnie `usernameMaxLength` znaków |

**Przypadki brzegowe:**
- Puste hasło i pusta nazwa użytkownika
- Znaki specjalne w haśle i nazwie
- `passwordMinLength > passwordMaxLength` (odwrócone limity)

---

### 3.3 `ChannelManager` (`channel_manager_test.cpp`)

| # | Przypadek testowy | Opis |
|---|---|---|
| 1 | `createPublicChannel()` — sukces | Tworzenie kanału publicznego |
| 2 | `createPrivateConversation()` — sukces | Tworzenie prywatnej konwersacji (2 użytkowników) |
| 3 | `createPrivateConversation()` — zła liczba | Nie 2 użytkowników — `false` |
| 4 | `deleteChannel()` — sukces | Usunięcie istniejącego kanału |
| 5 | `deleteChannel()` — nieistniejący | Zwraca `false` |
| 6 | `removeUserFromChannel()` — sukces | Usunięcie użytkownika z kanału |
| 7 | `removeUserFromChannel()` — użytkownik nie w kanale | Zwraca `false` |
| 8 | `addUserToChannel()` — sukces | Dodanie użytkownika |
| 9 | `addUserToChannel()` — kanał pełny | Zwraca `false` (maxUsers) |
| 10 | `addUserToChannel()` — duplikat | Użytkownik już w kanale — `false` |
| 11 | `editChannelName()` — sukces | Zmiana nazwy kanału |
| 12 | `getUserActiveChannels()` | Zwraca tylko aktywne kanały |
| 13 | `getAllUserChannels()` | Zwraca wszystkie kanały użytkownika |
| 14 | `deactivateChannel()` — kanał prywatny | Dezaktywacja prywatnego kanału |
| 15 | `deactivateChannel()` — kanał publiczny | Dla publicznego zwraca `true` bez zmian |

**Przypadki brzegowe:**
- Kanał z 0 użytkownikami
- Usunięcie kanału a potem próba operacji na nim
- `getUserActiveChannels()` dla użytkownika bez kanałów

---

### 3.4 `MessageManager` (`message_manager_test.cpp`)

| # | Przypadek testowy | Opis |
|---|---|---|
| 1 | `sendMessage()` — sukces | Wysłanie wiadomości |
| 2 | `sendMessage()` — nieistniejący nadawca | Zwraca `false` |
| 3 | `sendMessage()` — nieistniejący kanał | Zwraca `false` |
| 4 | `deleteMessage()` — sukces | Usunięcie wiadomości |
| 5 | `deleteMessage()` — nieistniejąca | Zwraca `false` |
| 6 | `editMessage()` — sukces | Edycja treści wiadomości |
| 7 | `getChannelMessages()` — kanał z wiadomościami | Zwraca wektor wiadomości |
| 8 | `getChannelMessages()` — pusty kanał | Zwraca pusty wektor |
| 9 | `getChannelMessages()` — nieistniejący kanał | Zwraca `std::nullopt` |

**Przypadki brzegowe:**
- Wiadomość z pustą treścią
- Edycja nieistniejącej wiadomości

---

## 4. Derived backend implementation tests

### 4.1 Transport framing and protocol

| # | Przypadek testowy | Plik | Opis |
|---|---|---|---|
| 1 | Length-prefixed send header | `server/src/core/connection_manager.cpp` | `sendMessage()` wysyła 4-bajtową długość w kolejności big-endian przed JSON-em |
| 2 | Length-prefixed receive header | `server/src/core/connection_manager.cpp` | `receiveMessage()` odczytuje 4-bajtowy nagłówek i następnie właściwą liczbę bajtów |
| 3 | Invalid length handling | `server/src/core/connection_manager.cpp` | `receiveMessage()` zwraca pusty JSON przy błędzie `recv()` lub niepoprawnym rozmiarze |

### 4.2 Authentication and session flow

| # | Przypadek testowy | Plik | Opis |
|---|---|---|---|
| 1 | Login response JSON success | `server/src/auth/authentication_event.cpp` | po poprawnym logowaniu zwraca `type=login_response`, `status=ok`, payload z `userName` |
| 2 | Login response JSON failure | `server/src/auth/authentication_event.cpp` | przy złych danych zwraca `type=login_response`, `status=error`, payload z `reason` |
| 3 | Registration response JSON | `server/src/auth/registration_event.cpp` | odpowiedź rejestracji używa `type=register_response` i `status` |
| 4 | Unauthorized event response | `server/src/events/*.cpp` | brak sesji zwraca `type=error`, `status=error`, `payload.reason=unauthorized` |
| 5 | Session activity update | `server/src/events/*.cpp` | każde poprawne żądanie aktualizuje czas ostatniej aktywności sesji |

### 4.3 Event response protocol

| # | Przypadek testowy | Plik | Opis |
|---|---|---|---|
| 1 | Add message response | `server/src/events/add_message_event.cpp` | `new_message` z `payload` zawierającym senderName, channelId, content |
| 2 | Create channel response | `server/src/events/create_channel_event.cpp` | `channel_created` z `payload` opisującym kanał |
| 3 | Edit message response | `server/src/events/edit_message_event.cpp` | `message_edited` z `messageId` i `newContent` |
| 4 | Remove message response | `server/src/events/remove_message_event.cpp` | `message_removed` z `messageId` |
| 5 | Edit channel response | `server/src/events/edit_channel_event.cpp` | `channel_edited` z `channelId` i `newName` |
| 6 | Remove channel response | `server/src/events/remove_channel_event.cpp` | `channel_removed` z `channelId` |
| 7 | Add user to channel response | `server/src/events/add_user_to_channel_event.cpp` | `user_joined_channel` z `channelId` i `userName` |
| 8 | Sync response payload | `server/src/events/synchronize_data_event.cpp` | `sync_response` zawiera `channels` i `messages` jako tablice JSON |

### 4.4 Client response handling

| # | Przypadek testowy | Plik | Opis |
|---|---|---|---|
| 1 | Handle new_message | `client/src/client_backend.cpp` | `new_message` aktualizuje lokalny stan `messages` |
| 2 | Handle message_edited | `client/src/client_backend.cpp` | `message_edited` modyfikuje istniejącą wiadomość |
| 3 | Handle message_removed | `client/src/client_backend.cpp` | `message_removed` usuwa wiadomość z lokalnego stanu |
| 4 | Handle channel_created | `client/src/client_backend.cpp` | `channel_created` dodaje nowy kanał do lokalnego stanu |
| 5 | Handle sync_response | `client/src/client_backend.cpp` | `sync_response` odtwarza w pełni stan `channels` i `messages` |
| 6 | Handle generic error | `client/src/client_backend.cpp` | `error` zapisuje komunikat w status queue |

> These cases are now covered by unit tests in `tests/client/client_backend_test.cpp`.

### 4.5 Integration scenarios

| # | Przypadek testowy | Plik | Opis |
|---|---|---|---|
| 1 | Register/login flow | `tests/integration/client_server_integration_test.cpp` | end-to-end rejestracja użytkownika, logowanie i JSON response verification |
| 2 | Send message and receive ack | `tests/integration/client_server_integration_test.cpp` | klient wysyła `add_message`, serwer zwraca `new_message` |
| 3 | Create channel and sync | `tests/integration/client_server_integration_test.cpp` | klient tworzy kanał, następnie `synchronize` pobiera kanał i wiadomości |
| 4 | Unauthorized access handling | `tests/integration/client_server_integration_test.cpp` | brak sesji przy żądaniu zwraca `type=error` |
| 5 | Framing compatibility | `tests/integration/server_integration_test.cpp` | potwierdza length-prefixed JSON między klientem a serwerem |

**Notes:** Some tests may initially fail until implementation is completed; use this file as the canonical mapping between guide stages and derived test cases.

---

### 3.5 `SessionManager` (`session_manager_test.cpp`)

| # | Przypadek testowy | Opis |
|---|---|---|
| 1 | `createSession()` — sukces | Tworzenie sesji |
| 2 | `createSession()` — duplikat sessionId | Zwraca `false` (mało prawdopodobne) |
| 3 | `removeUserSession()` — sukces | Usunięcie sesji użytkownika |
| 4 | `removeUserSession()` — nieistniejąca | Zwraca `false` |
| 5 | `updateUserSession()` — sukces | Aktualizacja `lastActive` |
| 6 | `updateUserSession()` — nieistniejąca | Zwraca `false` |
| 7 | `cleanInactiveSessions()` — żadna wygasła | Nic nie usuwa |
| 8 | `cleanInactiveSessions()` — wygasłe sesje | Usuwa wygasłe sesje |

**Przypadki brzegowe:**
- `sessionTimeout` = 0 ms (natychmiast wygasa)
- Bardzo duży `sessionTimeout`
- Wywołanie `cleanInactiveSessions()` na pustej sesji

---

### 3.6 `ConnectionManager` (`connection_manager_test.cpp`)

| # | Przypadek testowy | Opis |
|---|---|---|
| 1 | `acceptConnection()` — sukces | Akceptacja połączenia (wymaga mockowania socketu) |
| 2 | `acceptConnection()` — błąd accept | Zwraca `false` |
| 3 | `closeConnection()` — sukces | Zamknięcie istniejącego połączenia |
| 4 | `closeConnection()` — nieistniejące | Zwraca `false` |
| 5 | `sendMessage()` — sukces | Wysłanie wiadomości |
| 6 | `sendMessage()` — nieistniejący fd | Zwraca `false` |
| 7 | `receiveMessage()` — sukces | Odczyt wiadomości |
| 8 | `receiveMessage()` — zbyt duża wiadomość | Zwraca pusty string (limit 65536) |
| 9 | `getConnectionMap()` — pusta mapa | Sprawdzenie pustej mapy |

**Przypadki brzegowe:**
- Wysyłka do zamkniętego socketu
- Odbiór z zamkniętego socketu
- Wiadomość o długości 0
- Wiadomość o długości dokładnie 65536

> **Uwaga**: Testy `ConnectionManager` wymagają albo mockowania syscalli (socketpair), albo integracyjnego podejścia z prawdziwymi socketami. Warto rozważyć abstrakcję interfejsu socketu dla łatwiejszego testowania.

---

## 4. Klient — backend i protokół

| # | Przypadek testowy | Opis |
|---|---|---|
| 1 | `ClientBackend::connect()` | Połączenie z serwerem i uruchomienie pętli odbioru |
| 2 | `ClientBackend::disconnect()` | Zamykanie gniazda i zakończenie wątku |
| 3 | `ClientBackend::drainQueue()` | Odrzut wiadomości z kolejki przy pustej i pełnej kolejce |
| 4 | `login()` / `registerUser()` | Wysyłanie poprawnych żądań JSON |
| 5 | `sendMessage()` / `editMessage()` / `removeMessage()` | Wysyłanie żądań wiadomości |
| 6 | `createChannel()` / `editChannel()` / `removeChannel()` | Wysyłanie żądań kanałów |
| 7 | `synchronize()` | Poprawne żądanie synchronizacji |
| 8 | Odbiór `login_response` | Aktualizacja `currentUser` |
| 9 | Odbiór `sync_response` | Uzupełnianie `channels` i `messages` |
| 10 | Błędne / nieznane odpowiedzi | Fallback handler i brak crashu |

**Przypadki brzegowe:**
- Serwer wysyła niepoprawny JSON
- Połączenie zerwane w trakcie `recv()`
- Odbiór odpowiedzi przed zalogowaniem
- Różnice między formatem wysyłanym przez klienta a oczekiwanym przez serwer

---

## 5. Integration tests

| # | Przypadek testowy | Opis |
|---|---|---|
| 1 | End-to-end register/login | Rejestracja, logowanie, potwierdzenie odpowiedzi |
| 2 | Synchronizacja stanu | `synchronize` zwraca pełny stan kanałów i wiadomości |
| 3 | Wysyłanie wiadomości | Klient wysyła wiadomość, serwer zwraca `new_message` |
| 4 | Tworzenie kanału | Kanał tworzony i poprawnie widoczny po stronie serwera |
| 5 | Operacja bez sesji | Żądania bez logowania są odrzucane |
| 6 | Protokół framing | Klient i serwer używają tej samej metody ramkowania danych |

**Istniejące ograniczenia:**
- Aktualne testy integracyjne serwera weryfikują protokół w jednej formie.
- W kodzie klienta i serwera istnieje niezsynchronizowany sposób kodowania pakietów.
- Należy dopracować testy, aby wspierały jednorodny transport i obie strony protokołu.
