# Lista ulepszeń (MVP → produkcja)

## Krytyczne błędy do naprawienia

1. **Testy (`tests/`) wymagają implementacji** — struktura testów jest przygotowana, ale żaden test nie jest jeszcze zaimplementowany. Testy dla modeli, repozytoriów i managerów czekają na uzupełnienie.

2. **`Serializer` wymaga aktualizacji** — serializacja `timestamp` (steady_clock::time_point) nie jest zaimplementowana (oznaczona jako TODO).

## Ulepszenia architektury

3. **Implementacja klienta** — katalog `client/` jest pusty. Potrzebna jest implementacja klienta łączącego się z serwerem.

## Ulepszenia jakości kodu

4. **Obsługa błędów** — repozytoria zwracają domyślne obiekty (np. `User()`) zamiast `std::optional` lub rzucać wyjątki, gdy element nie istnieje.

5. **Brak logowania** — `ServerConfig` ma pola `logLevel` i `enableConsoleLogging`, ale nie ma systemu logowania.

6. **System zdarzeń (Event)** — klasa `Event` w `src/` jest niezintegrowana z resztą projektu.

7. **Konfiguracja z pliku** — `ServerConfig::readFromFile()` i `ClientConfig::readFromFile()` są zaimplementowane, ale brak domyślnego pliku konfiguracyjnego.

## Ulepszenia bezpieczeństwa

8. **Zarządzanie sesjami** — brak automatycznego czyszczenia nieaktywnych sesji (metoda `cleanInactiveSessions()` istnieje, ale nie jest nigdzie cyklicznie wywoływana).

## Ulepszenia infrastruktury

9. **Docker** — dodanie `Dockerfile` i `docker-compose.yml` do łatwego uruchamiania serwera.

10. **CI/CD** — konfiguracja GitHub Actions do automatycznego budowania i testowania.
