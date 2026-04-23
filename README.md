# PWChat Backend

Wersja: `1.0.0 MVP`

Backend projektu PWChat zawiera serwer TCP, wspólne modele domenowe, repozytoria danych oraz bibliotekę klienta używaną przez aplikację Qt z repozytorium `advanced-cpp-2026-ui`.

## Zakres MVP

- Rejestracja i logowanie użytkowników.
- Sesje oparte o mapowanie `fd -> username`.
- Kanały otwarte i prywatne w modelu domenowym.
- Tworzenie i usuwanie kanałów.
- Dodawanie użytkowników do kanałów.
- Wysyłanie, edycja i usuwanie wiadomości po stronie backendu.
- Broadcast wiadomości do użytkowników należących do kanału.
- Synchronizacja danych po zalogowaniu.
- Zapis i odczyt repozytoriów z plików JSON.
- Konfiguracja serwera przez `config/server.json`.
- Dokumentacja Doxygen i ręczne testy.

## Struktura

- `common/` - modele, repozytoria wspólne, konfiguracja i serializacja JSON.
- `server/` - serwer TCP, eventy, managerowie, sesje i autoryzacja.
- `client/` - backend klienta: socket, protokół JSON i dispatcher odpowiedzi.
- `tests/` - testy jednostkowe i integracyjne uruchamiane ręcznie.
- `config/` - konfiguracja klienta i serwera.
- `data/` - pliki JSON używane przez persistence MVP.
- `scripts/` - skrypty pomocnicze, m.in. generowanie dokumentacji.

## Wymagania

- Linux albo WSL.
- CMake `3.22+`.
- Kompilator C++17, np. `g++`.
- `nlohmann-json3-dev`.
- `libssl-dev`.
- `libgtest-dev` tylko do testów.
- `doxygen` i `graphviz` tylko do dokumentacji.

Przykład instalacji na Ubuntu/WSL:

```bash
sudo apt update
sudo apt install g++ cmake nlohmann-json3-dev libssl-dev libgtest-dev doxygen graphviz
```

## Budowanie

Serwer:

```bash
cmake --preset server-only
cmake --build --preset server-only
```

Backend klienta:

```bash
cmake --preset client-only
cmake --build --preset client-only
```

Pełny build developerski:

```bash
cmake --preset debug
cmake --build --preset debug
```

## Uruchomienie serwera

```bash
./build/presets/server-only/server/PWChat
```

Domyślna konfiguracja serwera jest w `config/server.json`. Aktualny port MVP to `8091`.

## Testy

Testy nie są wymagane przy każdym buildzie. Uruchamiaj je ręcznie wtedy, gdy chcesz sprawdzić implementację:

```bash
cmake --preset debug-tests
cmake --build --preset debug-tests
ctest --test-dir build/presets/debug-tests --output-on-failure
```

Szybki skrypt pomocniczy:

```bash
./scripts/test-fast.sh
```

## Dokumentacja

```bash
./scripts/gen-docs.sh
```

Główne pliki opisowe:

- `OVERVIEW.md` - opis zaimplementowanego szkieletu.
- `DEADLINE_GUIDE.md` - plan pracy i zakres MVP.
- `EXTENSION.md` - pomysły na rozszerzenia po MVP.
- `../ALL_SUMMARY.md` - zbiorcze przygotowanie do obrony dla obu repozytoriów.

## Ograniczenia MVP

- Backend jest przygotowany pod Linux/WSL i używa POSIX sockets.
- Natywne Windows sockets są rozszerzeniem po MVP.
- Persistence jest plikowe, bez osobnej bazy danych.
- Brakuje pełnej warstwy uprawnień administratora.
- Część funkcji backendu nie ma jeszcze pełnej obsługi w UI.

## Najważniejsze punkty do obrony

- Serwer używa `poll`, żeby obsługiwać wiele połączeń TCP.
- Wiadomości są ramkowane długością i przesyłane jako JSON.
- `EventFactory` mapuje typ requestu na konkretną klasę eventu.
- Managerowie przechowują reguły domenowe, a eventy są adapterami protokołu.
- `DomainResult` ujednolica sukcesy i błędy.
- Sesja jest powiązana z deskryptorem połączenia, a nie wyłącznie z nazwą użytkownika.
