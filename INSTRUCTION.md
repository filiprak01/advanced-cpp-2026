# PWChat Backend - instrukcja uruchomienia

Wersja: `1.0.0 MVP`

Ten projekt uruchamiaj w Linux/WSL. Serwer MVP korzysta z POSIX sockets, dlatego natywne Windows sockets nie sa czescia aktualnego release.

W tej instrukcji slowo "rozwiazanie" oznacza katalog build wygenerowany przez CMake, np. `build/presets/server-only`.

## 1. Instalacja zaleznosci

```bash
cd /home/filip/cpp/advanced-cpp-2026
chmod +x precondition.sh
./precondition.sh
```

Skrypt instaluje kompilator, CMake, biblioteke JSON, OpenSSL, GoogleTest oraz Doxygen.

## 2. Tworzenie rozwiazania CMake dla serwera

Ta komenda tylko konfiguruje projekt i tworzy katalog build dla serwera:

```bash
cd /home/filip/cpp/advanced-cpp-2026
cmake --preset server-only
```

Budowanie serwera:

```bash
cmake --build --preset server-only
```

Plik wykonywalny serwera powstaje tutaj:

```bash
./build/presets/server-only/server/PWChat
```

## 3. Tworzenie rozwiazania CMake dla backendu klienta

Backend klienta jest warstwa transportowa uzywana przez UI.

Konfiguracja:

```bash
cd /home/filip/cpp/advanced-cpp-2026
cmake --preset client-only
```

Budowanie:

```bash
cmake --build --preset client-only
```

## 4. Tworzenie rozwiazania deweloperskiego serwer + klient

Ten wariant buduje biblioteki backendu serwera i klienta razem, ale bez testow:

```bash
cd /home/filip/cpp/advanced-cpp-2026
cmake --preset debug
cmake --build --preset debug
```

## 5. Uruchomienie serwera

```bash
cd /home/filip/cpp/advanced-cpp-2026
./build/presets/server-only/server/PWChat
```

Serwer uzywa konfiguracji:

```text
config/server.json
```

Aktualny port MVP:

```text
8091
```

Po poprawnym starcie powinienes zobaczyc informacja podobna do:

```text
Server listening on 0.0.0.0:8091
Waiting for connections...
```

## 6. Tworzenie rozwiazania CMake z testami

Konfiguracja testow:

```bash
cd /home/filip/cpp/advanced-cpp-2026
cmake --preset debug-tests
```

Budowanie testow:

```bash
cmake --build --preset debug-tests
```

Wypisanie wszystkich testow bez uruchamiania:

```bash
ctest --test-dir build/presets/debug-tests -N
```

## 7. Uruchamianie wszystkich testow backendu

```bash
cd /home/filip/cpp/advanced-cpp-2026
ctest --test-dir build/presets/debug-tests --output-on-failure --timeout 30
```

Oczekiwany stan po aktualnym MVP:

```text
272/272 tests passed
```

## 8. Uruchamianie testow unit wedlug modulow

Wszystkie testy unit bez integracyjnych:

```bash
ctest --test-dir build/presets/debug-tests -E 'Integration' --output-on-failure --timeout 30
```

Modele domenowe, czyli `User`, `Channel`, `Message`, `Session`, `Connection`:

```bash
ctest --test-dir build/presets/debug-tests -R '^(ChannelTest|ConnectionTest|MessageTest|SessionTest|UserTest)\.' --output-on-failure --timeout 30
```

Repozytoria, czyli zapis i odczyt danych:

```bash
ctest --test-dir build/presets/debug-tests -R '^(ChannelRepositoryTest|MessageRepositoryTest|SessionRepositoryTest|UserRepositoryTest)\.' --output-on-failure --timeout 30
```

Managerowie i zdarzenia domenowe:

```bash
ctest --test-dir build/presets/debug-tests -R '^(AuthFixture|RegFixture|SessionFixture|ShortTimeoutFixture|ConnectionManagerTest|ChannelFixture|MessageFixture|EventFactoryTest|ChannelBroadcastFixture|MessageBroadcastFixture)\.' --output-on-failure --timeout 30
```

Backend klienta i protokol komunikacji:

```bash
ctest --test-dir build/presets/debug-tests -R '^(ClientBackendTest|ProtocolTest|ResponseDispatcherTest)\.' --output-on-failure --timeout 30
```

Pojedyncza grupa testow, przyklad dla sesji:

```bash
ctest --test-dir build/presets/debug-tests -R '^SessionFixture\.' --output-on-failure --timeout 30
```

## 9. Uruchamianie testow integracyjnych

Wszystkie testy integracyjne:

```bash
ctest --test-dir build/presets/debug-tests -R 'Integration' --output-on-failure --timeout 30
```

Tylko integracja serwera:

```bash
ctest --test-dir build/presets/debug-tests -R '^ServerIntegrationTest\.' --output-on-failure --timeout 30
```

Tylko integracja klient-serwer:

```bash
ctest --test-dir build/presets/debug-tests -R '^ClientServerIntegrationTest\.' --output-on-failure --timeout 30
```

Jezeli test integracyjny nie startuje, sprawdz czy port testowy nie jest zajety:

```bash
ss -ltnp | grep 19001
```

## 10. Dokumentacja Doxygen

```bash
cd /home/filip/cpp/advanced-cpp-2026
./scripts/gen-docs.sh
```

Dokumentacja HTML powstaje tutaj:

```text
docs/html/index.html
```

## 11. Typowa kolejnosc uruchomienia calej aplikacji

1. Uruchom backend serwera w pierwszym terminalu.
2. Przejdz do repozytorium `advanced-cpp-2026-ui`.
3. Zbuduj i uruchom klienta UI.
4. W UI zarejestruj uzytkownika albo zaloguj sie.
5. Po zalogowaniu przejdz do kanalow i testuj wiadomosci.

## 12. Najczestsze problemy

- Jesli port jest zajety, sprawdz procesy: `ss -ltnp | grep 8091`.
- Jesli zmienisz port w backendzie, zmien tez port w UI w `advanced-cpp-2026-ui/config/client.json`.
- Jesli testy integracyjne nie startuja, upewnij sie, ze port testowy `19001` nie jest zajety.
- Jesli po zmianach CMake zachowuje sie dziwnie, usun tylko katalog `build/` i skonfiguruj ponownie.
