# esp32-max7219-led-matrix

## PL

Projekt tablicy LED opartej o ESP32 i lancuch 16 modulow MAX7219 (FC16), wyswietlajacej czas, date, dzien tygodnia oraz komunikaty pobierane z HTTP.

### Funkcje

- Polaczenie z WiFi i kontrola pracy na 5 GHz.
- Synchronizacja czasu przez NTP.
- Pobieranie 3 wiadomosci z endpointow HTTP.
- Odswiezanie czasu co 1 sekunde.
- Odswiezanie wiadomosci co 60 sekund (w godzinach dziennych).
- Tryby wyswietlania:
  - nocny (20:00-07:00): przyciemniony zegar z migajacym dwukropkiem,
  - podczas zajec: naprzemiennie godzina i komunikat,
  - poza zajeciami: animacje + wiadomosci + data + dzien tygodnia.

### Sprzet i piny

- ESP32
- 16 x MAX7219 (FC16)
- Domyslne piny:
  - DATA_PIN: 8
  - CLK_PIN: 9
  - CS_PIN: 10

### Wymagane biblioteki

- MD_Parola
- MD_MAX72xx
- WiFi (ESP32)
- esp_wifi
- HTTPClient
- time

### Konfiguracja

Sekrety (WiFi i endpointy) ustawiaj w lokalnym pliku `secrets.h` (ignorowanym przez Git):

1. Skopiuj `secrets.example.h` jako `secrets.h`.
2. Uzupelnij wartosci:
  - `WIFI_SSID`, `WIFI_PASSWORD`, `WIFI_IS_HIDDEN`
  - `SERVER_MESSAGE_1_URL`, `SERVER_MESSAGE_2_URL`, `SERVER_MESSAGE_3_URL`

Pozostale ustawienia nadal konfigurujesz w `miecz_sw.ino`:
- `ntpServer`, `gmtOffset_sec`, `daylightOffset_sec`
- `classSchedule` (harmonogram zajec)

### Szybki start

1. Otworz projekt w Arduino IDE.
2. Zainstaluj biblioteki.
3. Wybierz plytke ESP32 i port COM.
4. Utworz `secrets.h` na podstawie `secrets.example.h` i wpisz lokalne dane.
5. Wgraj kod i uruchom Serial Monitor (115200).

### Endpointy
- Do konfiguracji

Przy bledzie HTTP wyswietlany jest placeholder `--`.

### Struktura repo

- `miecz_sw.ino` - glowny kod programu
- `secrets.example.h` - przykladowa konfiguracja sekretow
- `.gitignore` - ignoruje lokalny plik `secrets.h`

## EN

An ESP32-based LED display project using a 16-module MAX7219 (FC16) chain to show time, date, weekday, and HTTP-fetched messages.

### Features

- WiFi connection with 5 GHz enforcement/check.
- NTP time synchronization.
- Fetches 3 messages from HTTP endpoints.
- Clock refresh every 1 second.
- Message refresh every 60 seconds (daytime hours).
- Display modes:
  - night mode (20:00-07:00): dimmed clock with blinking colon,
  - class-time mode: alternates between time and message,
  - normal mode: animations + messages + date + weekday.

### Hardware and pins

- ESP32
- 16 x MAX7219 (FC16)
- Default pins:
  - DATA_PIN: 8
  - CLK_PIN: 9
  - CS_PIN: 10

### Required libraries

- MD_Parola
- MD_MAX72xx
- WiFi (ESP32)
- esp_wifi
- HTTPClient
- time

### Configuration

Configure secrets (WiFi and endpoints) in a local `secrets.h` file (ignored by Git):

1. Copy `secrets.example.h` to `secrets.h`.
2. Fill in:
  - `WIFI_SSID`, `WIFI_PASSWORD`, `WIFI_IS_HIDDEN`
  - `SERVER_MESSAGE_1_URL`, `SERVER_MESSAGE_2_URL`, `SERVER_MESSAGE_3_URL`

The remaining settings stay in `miecz_sw.ino`:
- `ntpServer`, `gmtOffset_sec`, `daylightOffset_sec`
- `classSchedule` (class timetable)

### Quick start

1. Open the project in Arduino IDE.
2. Install required libraries.
3. Select your ESP32 board and COM port.
4. Create `secrets.h` from `secrets.example.h` and enter local values.
5. Upload and open Serial Monitor (115200).

### Endpoints

- To be configured

If an HTTP request fails, the display falls back to `--`.

### Repository structure

- `miecz_sw.ino` - main firmware file
- `secrets.example.h` - sample secrets configuration
- `.gitignore` - ignores local `secrets.h`
