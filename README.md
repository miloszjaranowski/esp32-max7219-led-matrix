# esp32-max7219-led-matrix

![Demo](demo.gif)

Tablica LED sterowana przez ESP32-C5, wyswietlajaca czas, date i komunikaty z sieci. / ESP32-C5-powered LED matrix displaying time, date and network messages.

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

Instalowane przez Arduino IDE Library Manager:
- MD_Parola
- MD_MAX72xx

Wbudowane w rdzen ESP32 (nie wymagaja instalacji):
- WiFi, esp_wifi, HTTPClient, time

### Konfiguracja

Sekrety (WiFi i endpointy) ustawiaj w lokalnym pliku `secrets.h` (ignorowanym przez Git):

1. Skopiuj `secrets.example.h` jako `secrets.h`.
2. Uzupelnij wartosci:
  - `WIFI_SSID`, `WIFI_PASSWORD`, `WIFI_IS_HIDDEN`
  - `SERVER_MESSAGE_1_URL`, `SERVER_MESSAGE_2_URL`, `SERVER_MESSAGE_3_URL`

Pozostale ustawienia nadal konfigurujesz w `esp32_led_matrix.ino`:
- `ntpServer`, `gmtOffset_sec`, `daylightOffset_sec`
- `classSchedule` (harmonogram zajec)

### Szybki start

1. Otworz projekt w Arduino IDE.
2. Zainstaluj biblioteki: MD_Parola, MD_MAX72xx.
3. Wybierz plytke: **ESP32C5 Dev Module** i odpowiedni port COM.
4. Utworz `secrets.h` na podstawie `secrets.example.h` i wpisz lokalne dane.
5. Wgraj kod i uruchom Serial Monitor (115200).

### Ograniczenia

- Logika polaczenia preferuje i pilnuje pasma 5 GHz.
- Jesli AP dziala tylko na 2.4 GHz, urzadzenie moze nie utrzymac polaczenia.
- Projekt zaklada stale polaczenie z internetem (NTP + HTTP).

### Rozwiazywanie problemow

- Brak WiFi: sprawdz `WIFI_SSID`, `WIFI_PASSWORD` i `WIFI_IS_HIDDEN` w `secrets.h`.
- Brak czasu: sprawdz dostep do internetu i ustawienia `ntpServer`/strefy czasowej.
- Brak komunikatow: sprawdz endpointy HTTP i odpowiedz serwera (czy zwraca tekst).
- Bledne wyswietlanie: potwierdz typ modulow (`FC16_HW`) i mapowanie pinow.

### Struktura repo

- `esp32_led_matrix.ino` - glowny kod programu
- `secrets.example.h` - przykladowy plik konfiguracji (skopiuj jako `secrets.h` i uzupelnij)

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

Install via Arduino IDE Library Manager:
- MD_Parola
- MD_MAX72xx

Built into the ESP32 core (no install needed):
- WiFi, esp_wifi, HTTPClient, time

### Configuration

Configure secrets (WiFi and endpoints) in a local `secrets.h` file (ignored by Git):

1. Copy `secrets.example.h` to `secrets.h`.
2. Fill in:
  - `WIFI_SSID`, `WIFI_PASSWORD`, `WIFI_IS_HIDDEN`
  - `SERVER_MESSAGE_1_URL`, `SERVER_MESSAGE_2_URL`, `SERVER_MESSAGE_3_URL`

The remaining settings stay in `esp32_led_matrix.ino`:
- `ntpServer`, `gmtOffset_sec`, `daylightOffset_sec`
- `classSchedule` (class timetable)

### Quick start

1. Open the project in Arduino IDE.
2. Install libraries: MD_Parola, MD_MAX72xx.
3. Select board: **ESP32C5 Dev Module** and the correct COM port.
4. Create `secrets.h` from `secrets.example.h` and enter local values.
5. Upload and open Serial Monitor (115200).

### Known limitations

- Connection logic prefers and enforces 5 GHz operation.
- If your AP is only 2.4 GHz, the device may fail to keep a connection.
- The project assumes internet availability (NTP + HTTP).

### Troubleshooting

- No WiFi: verify `WIFI_SSID`, `WIFI_PASSWORD`, and `WIFI_IS_HIDDEN` in `secrets.h`.
- No time sync: verify internet access and `ntpServer`/timezone settings.
- No messages: verify HTTP endpoints and server response payload.
- Display issues: confirm module type (`FC16_HW`) and pin mapping.

### Repository structure

- `esp32_led_matrix.ino` - main firmware file
- `secrets.example.h` - sample configuration (copy as `secrets.h` and fill in your values)
