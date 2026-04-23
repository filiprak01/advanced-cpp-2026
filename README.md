# PWChat

## Quick Links

- Implemented scaffold overview: [OVERVIEW.md](OVERVIEW.md)
- Deadline-first guide: [DEADLINE_GUIDE.md](DEADLINE_GUIDE.md)
- Optional stretch goals: [EXTENSION.md](EXTENSION.md)
- Detailed backend plan: [BACKEND_IMPLEMENTATION_GUIDE.md](BACKEND_IMPLEMENTATION_GUIDE.md)
- Detailed client-backend plan: [ClientImplementation.md](ClientImplementation.md)

## Test Policy

Tests are optional during normal implementation.

- Default builds do not compile tests.
- Enable tests only when you want them:
  `cmake --preset debug-tests && cmake --build --preset debug-tests`
- Fast local verification:
  `bash scripts/test-fast.sh`

## Documentation

- Generate Doxygen docs:
  `./scripts/gen-docs.sh`
- Main documentation page:
  [OVERVIEW.md](OVERVIEW.md)

## Struktura katalogów

```
.
├── CMakeLists.txt              # Główny plik konfiguracyjny CMake
├── Doxyfile                    # Konfiguracja generowania dokumentacji (Doxygen)
├── format.sh                   # Skrypt do formatowania kodu (clang-format)
├── tidy.sh                     # Skrypt do analizy statycznej kodu (clang-tidy)
├── README.md                   # Ten plik
│
├── common/                     # Biblioteka wspólna (modele i narzędzia)
│   ├── CMakeLists.txt          # Konfiguracja CMake dla commonLib
│   ├── include/
│   │   └── common/
│   │       ├── models/         # Klasy modeli danych
│   │       │   ├── channel.hpp
│   │       │   ├── connection.hpp
│   │       │   ├── message.hpp
│   │       │   ├── session.hpp
│   │       │   └── user.hpp
│   │       └── utilities/      # Klasy narzędziowe
│   │           ├── config.hpp
│   │           └── serializer.hpp
│   └── src/                    # Implementacje metod z plików nagłówkowych
│       ├── channel.cpp
│       ├── config.cpp
│       ├── connection.cpp
│       ├── message.cpp
│       ├── serializer.cpp
│       ├── session.cpp
│       └── user.cpp
│
├── server/                     # Biblioteka serwera
│   ├── CMakeLists.txt          # Konfiguracja CMake dla serverLib
│   ├── include/
│   │   └── server/
│   │       ├── auth/           # Autoryzacja i rejestracja
│   │       │   ├── auth_manager.hpp
│   │       │   ├── password_hasher.hpp
│   │       │   └── registration_manager.hpp
│   │       ├── core/           # Logika biznesowa serwera
│   │       │   ├── channel_manager.hpp
│   │       │   ├── connection_manager.hpp
│   │       │   ├── message_manager.hpp
│   │       │   └── session_manager.hpp
│   │       └── repos/          # Repozytoria danych (in-memory)
│   │           ├── channel_repo.hpp
│   │           ├── message_repo.hpp
│   │           ├── session_repo.hpp
│   │           └── user_repo.hpp
│   └── src/                    # Implementacje metod z plików nagłówkowych
│       ├── auth_manager.cpp
│       ├── channel_manager.cpp
│       ├── channel_repo.cpp
│       ├── connection_manager.cpp
│       ├── message_manager.cpp
│       ├── message_repo.cpp
│       ├── password_hasher.cpp
│       ├── registration_manager.cpp
│       ├── session_manager.cpp
│       ├── session_repo.cpp
│       └── user_repo.cpp
│
├── client/                     # Klient (do zaimplementowania)
│   ├── include/
│   │   └── client/
│   └── src/
│
├── src/                        # Punkt wejścia aplikacji
│   └── main.cpp
│
├── tests/                      # Testy jednostkowe (GTest)
│   ├── CMakeLists.txt
│   ├── models/                 # Testy modeli danych
│   │   ├── user_test.cpp
│   │   ├── message_test.cpp
│   │   ├── channel_test.cpp
│   │   ├── session_test.cpp
│   │   └── connection_test.cpp
│   ├── repos/                  # Testy repozytoriów
│   │   ├── user_repo_test.cpp
│   │   ├── channel_repo_test.cpp
│   │   ├── message_repo_test.cpp
│   │   └── session_repo_test.cpp
│   └── managers/               # Testy managerów
│       ├── auth_manager_test.cpp
│       ├── registration_manager_test.cpp
│       ├── channel_manager_test.cpp
│       ├── message_manager_test.cpp
│       ├── session_manager_test.cpp
│       └── connection_manager_test.cpp
│
├── build/                      # Katalog z plikami kompilacji (generowany)
│
└── vcpkg/                      # Katalog na menedżer pakietów vcpkg (opcjonalny)
```

## Wymagania systemowe

- **Kompilator**: g++ (GCC) z obsługą C++17
- **CMake**: wersja 3.22 lub nowsza
- **Biblioteki**:
  - `nlohmann-json3-dev` — serializacja JSON
  - `libssl-dev` — hashowanie haseł (OpenSSL)
  - `libgtest-dev` — testy jednostkowe (Google Test)

### Instalacja zależności (Ubuntu/Debian)

```bash
sudo apt update
sudo apt install g++ cmake nlohmann-json3-dev libssl-dev libgtest-dev
```

## Kompilacja projektu

1. Utwórz katalog `build` i wygeneruj pliki kompilacji:

```bash
mkdir -p build
cd build
cmake ..
```

2. Skompiluj projekt:

```bash
make -j$(nproc)
```

Skompilowany plik wykonywalny `PWChat` znajdziesz w katalogu `build/`.

## Uruchomienie programu

```bash
cd build
./PWChat
```

## Uruchomienie testów

```bash
cd build
ctest --output-on-failure
```

lub bezpośrednio:

```bash
cd build
./tests/runTests
```

## Narzędzia pomocnicze

- **Formatowanie kodu**: `./format.sh` — uruchamia `clang-format` na plikach źródłowych.
- **Analiza statyczna**: `./tidy.sh` — uruchamia `clang-tidy` na plikach źródłowych.
- **Dokumentacja**: `doxygen Doxyfile` — generuje dokumentację HTML na podstawie komentarzy w kodzie.
