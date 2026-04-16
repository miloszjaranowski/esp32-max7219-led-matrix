#include <MD_Parola.h>
#include <MD_Parola_lib.h>
#include <MD_MAX72xx.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include "time.h"
#include <stdlib.h>
#include <HTTPClient.h>

#if __has_include("secrets.h")
#include "secrets.h"
#endif

#ifndef WIFI_SSID
#define WIFI_SSID "your_ssid"
#endif

#ifndef WIFI_PASSWORD
#define WIFI_PASSWORD "your_password"
#endif

#ifndef WIFI_IS_HIDDEN
#define WIFI_IS_HIDDEN true
#endif

#ifndef SERVER_MESSAGE_1_URL
#define SERVER_MESSAGE_1_URL "your_server_endpoint_1"
#endif

#ifndef SERVER_MESSAGE_2_URL
#define SERVER_MESSAGE_2_URL "your_server_endpoint_2"
#endif

#ifndef SERVER_MESSAGE_3_URL
#define SERVER_MESSAGE_3_URL "your_server_endpoint_3"
#endif

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 16
#define DATA_PIN 8
#define CLK_PIN 9
#define CS_PIN 10

const char *ssid = WIFI_SSID;
const char *password = WIFI_PASSWORD;
const bool isHiddenNetwork = WIFI_IS_HIDDEN;
const unsigned long reconnectRetryIntervalMs = 3000;
const unsigned long clockUpdateIntervalMs = 1000;

unsigned long lastBeginAtMs = 0;
const unsigned long connectAttemptWindowMs = 16000;

bool canStartNewConnectAttempt()
{
  if (lastBeginAtMs == 0)
    return true;

  unsigned long elapsed = millis() - lastBeginAtMs;
  if (elapsed < connectAttemptWindowMs)
  {
    Serial.println("Previous WiFi attempt still in progress/cooldown.");
    return false;
  }

  return true;
}

bool connectOnly5GHz()
{
  if (!canStartNewConnectAttempt())
    return false;

  if (isHiddenNetwork)
  {
    Serial.println("Hidden SSID mode: connecting directly...");
    WiFi.disconnect();
    delay(300);
    lastBeginAtMs = millis();
    WiFi.begin(ssid, password);

    unsigned long start = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - start < 15000)
    {
      delay(500);
      Serial.print(".");
    }

    if (WiFi.status() == WL_CONNECTED && WiFi.channel() > 14)
      return true;

    if (WiFi.status() == WL_CONNECTED)
    {
      Serial.println("Connected, but not on 5 GHz. Disconnecting...");
    }
    else
    {
      Serial.println("Failed to connect to hidden SSID.");
    }

    WiFi.disconnect();
    return false;
  }

  int found = WiFi.scanNetworks(false, true);
  if (found <= 0)
  {
    Serial.println("No WiFi networks found.");
    return false;
  }

  int bestIdx = -1;
  int bestRssi = -1000;

  for (int i = 0; i < found; i++)
  {
    if (WiFi.SSID(i) != ssid)
      continue;
    int ch = WiFi.channel(i);
    if (ch <= 14)
      continue;

    int rssi = WiFi.RSSI(i);
    if (rssi > bestRssi)
    {
      bestRssi = rssi;
      bestIdx = i;
    }
  }

  if (bestIdx < 0)
  {
    Serial.println("SSID found, but no 5 GHz BSSID available.");
    WiFi.scanDelete();
    return false;
  }

  int preferredChannel = WiFi.channel(bestIdx);
  const uint8_t *preferredBssid = WiFi.BSSID(bestIdx);

  Serial.print("Connecting to 5 GHz AP on channel ");
  Serial.println(preferredChannel);

  WiFi.disconnect();
  delay(300);
  lastBeginAtMs = millis();
  WiFi.begin(ssid, password, preferredChannel, preferredBssid, true);

  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 15000)
  {
    delay(500);
    Serial.print(".");
  }

  WiFi.scanDelete();
  return (WiFi.status() == WL_CONNECTED && WiFi.channel() > 14);
}

const char *serverMessage1Add = SERVER_MESSAGE_1_URL;
const char *serverMessage2Add = SERVER_MESSAGE_2_URL;
const char *serverMessage3Add = SERVER_MESSAGE_3_URL;

String message1, message2, message3;
uint8_t message1Time = 3, message2Time = 3, message3Time = 3;

const char *ntpServer = "pl.pool.ntp.org";
const int gmtOffset_sec = 3600;
const int daylightOffset_sec = 3600;

char message1ToDisplay[70], message2ToDisplay[70], message3ToDisplay[70];
char currTime[10], currDate[30], numDay[2], strToDisplay[15];
int numDayInt, currentHour = 0;
const char *strDay;

bool showColon = true;
unsigned long colonBlinkMillis = 0;
const unsigned long colonBlinkInterval = 1000;

const char *weekdayNames[] = {
    "Poniedzialek", "Wtorek", "Sroda", "Czwartek", "Piatek", "Sobota", "Niedziela"};

const uint8_t classSchedule[][4] = {
    {8, 0, 9, 30},
    {9, 45, 11, 15},
    {11, 45, 13, 15},
    {13, 30, 15, 0},
    {15, 10, 16, 40},
    {16, 50, 18, 20},
    {18, 30, 20, 0}};
const uint8_t classScheduleCount = sizeof(classSchedule) / sizeof(classSchedule[0]);

bool isDuringClasses()
{
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo))
    return false;
  int h = timeinfo.tm_hour;
  int m = timeinfo.tm_min;
  for (uint8_t i = 0; i < classScheduleCount; i++)
  {
    int sh = classSchedule[i][0], sm = classSchedule[i][1];
    int eh = classSchedule[i][2], em = classSchedule[i][3];
    if ((h > sh || (h == sh && m >= sm)) && (h < eh || (h == eh && m <= em)))
    {
      return true;
    }
  }
  return false;
}

void printLocalTime()
{
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo))
  {
    Serial.println("Failed to obtain time");
    return;
  }
  strftime(currTime, sizeof(currTime), "%H:%M", &timeinfo);
  strftime(currDate, sizeof(currDate), "%d-%m-%Y", &timeinfo);
  strftime(numDay, sizeof(numDay), "%u", &timeinfo);
  if (numDay[0] >= '0' && numDay[0] <= '9')
  {
    numDayInt = numDay[0] - '0';
  }
  currentHour = timeinfo.tm_hour;
  strDay = weekdayNames[numDayInt - 1];
  strcpy(strToDisplay, strDay);
  Serial.print("Godzina: ");
  Serial.println(currentHour);
}

const uint8_t F_PMAN2 = 6;
const uint8_t W_PMAN2 = 18;
static const uint8_t PROGMEM pacman2[F_PMAN2 * W_PMAN2] = {
    0x00,
    0x81,
    0xc3,
    0xe7,
    0xff,
    0x7e,
    0x7e,
    0x3c,
    0x00,
    0x00,
    0x00,
    0xfe,
    0x7b,
    0xf3,
    0x7f,
    0xfb,
    0x73,
    0xfe,
    0x00,
    0x42,
    0xe7,
    0xe7,
    0xff,
    0xff,
    0x7e,
    0x3c,
    0x00,
    0x00,
    0x00,
    0xfe,
    0x7b,
    0xf3,
    0x7f,
    0xfb,
    0x73,
    0xfe,
    0x24,
    0x66,
    0xe7,
    0xff,
    0xff,
    0xff,
    0x7e,
    0x3c,
    0x00,
    0x00,
    0x00,
    0xfe,
    0x7b,
    0xf3,
    0x7f,
    0xfb,
    0x73,
    0xfe,
    0x3c,
    0x7e,
    0xff,
    0xff,
    0xff,
    0xff,
    0x7e,
    0x3c,
    0x00,
    0x00,
    0x00,
    0xfe,
    0x73,
    0xfb,
    0x7f,
    0xf3,
    0x7b,
    0xfe,
    0x24,
    0x66,
    0xe7,
    0xff,
    0xff,
    0xff,
    0x7e,
    0x3c,
    0x00,
    0x00,
    0x00,
    0xfe,
    0x73,
    0xfb,
    0x7f,
    0xf3,
    0x7b,
    0xfe,
    0x00,
    0x42,
    0xe7,
    0xe7,
    0xff,
    0xff,
    0x7e,
    0x3c,
    0x00,
    0x00,
    0x00,
    0xfe,
    0x73,
    0xfb,
    0x7f,
    0xf3,
    0x7b,
    0xfe,
};

struct animations
{
  textEffect_t anim_in;
  textEffect_t anim_out;
  const char *textOut;
  uint16_t speed;
  uint16_t pause;
  textPosition_t just;
};

animations animList[] = {
    {PA_SPRITE, PA_SPRITE, "PUT MICRO zaprasza!", 4, 2, PA_CENTER},
    {PA_SCROLL_LEFT, PA_SCROLL_LEFT, message1ToDisplay, message1Time, 0, PA_CENTER},
    {PA_SCROLL_LEFT, PA_SCROLL_LEFT, message2ToDisplay, message2Time, 0, PA_CENTER},
    {PA_SCROLL_LEFT, PA_SCROLL_LEFT, message3ToDisplay, message3Time, 0, PA_CENTER},
    {PA_RANDOM, PA_RANDOM, "Dzis jest", 1, 5, PA_CENTER},
    {PA_SCROLL_DOWN_RIGHT, PA_SCROLL_DOWN_RIGHT, currDate, 4, 5, PA_CENTER},
    {PA_SCROLL_DOWN_LEFT, PA_SCROLL_DOWN_LEFT, strToDisplay, 4, 5, PA_CENTER},
    {PA_RANDOM, PA_RANDOM, "Godzina", 1, 5, PA_CENTER},
    {PA_SCROLL_DOWN, PA_SCROLL_DOWN, currTime, 5, 5, PA_CENTER},
};

MD_Parola Display = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

unsigned long previousMillis = 0;
const long interval = 60000;
unsigned long lastReconnectTryMs = 0;
unsigned long lastClockUpdateMs = 0;

String httpGETRequest(const char *serverName);

void copyToBuffer(const String &src, char *dst, size_t dstSize)
{
  if (dstSize == 0)
    return;
  snprintf(dst, dstSize, "%s", src.c_str());
}

void refreshMessagesFromServer()
{
  if (WiFi.status() != WL_CONNECTED)
    return;

  message1 = httpGETRequest(serverMessage1Add);
  copyToBuffer(message1, message1ToDisplay, sizeof(message1ToDisplay));

  message2 = httpGETRequest(serverMessage2Add);
  copyToBuffer(message2, message2ToDisplay, sizeof(message2ToDisplay));

  message3 = httpGETRequest(serverMessage3Add);
  copyToBuffer(message3, message3ToDisplay, sizeof(message3ToDisplay));
}

void setup()
{
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(false);
  WiFi.persistent(false);

#ifdef WIFI_BAND_MODE_5G_ONLY
  esp_wifi_set_band_mode(WIFI_BAND_MODE_5G_ONLY);
#endif

  Serial.printf("Connecting to %s ", ssid);
  while (!connectOnly5GHz())
  {
    Serial.println("Retrying 5 GHz connection in 3 seconds...");
    delay(3000);
  }
  Serial.println(" CONNECTED");
  Serial.println(WiFi.localIP());

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();

  refreshMessagesFromServer();

  Display.begin();
  Display.setSpriteData(pacman2, W_PMAN2, F_PMAN2, pacman2, W_PMAN2, F_PMAN2);
  Display.setIntensity(4);
  Display.displayClear();

  for (uint8_t i = 0; i < ARRAY_SIZE(animList); i++)
  {
    animList[i].speed *= Display.getSpeed();
    animList[i].pause *= 500;
  }
}

void loop()
{
  static uint8_t i = 0;
  static bool showTime = true;
  static unsigned long switchTimer = 0;
  unsigned long currentMillis = millis();

  if (currentMillis - lastClockUpdateMs >= clockUpdateIntervalMs)
  {
    printLocalTime();
    lastClockUpdateMs = currentMillis;
  }

  if (WiFi.status() != WL_CONNECTED || WiFi.channel() <= 14)
  {
    if (currentMillis - lastReconnectTryMs >= reconnectRetryIntervalMs)
    {
      Serial.println("WiFi not on 5 GHz, reconnecting...");
      connectOnly5GHz();
      lastReconnectTryMs = currentMillis;
    }
  }

  if ((currentMillis - previousMillis >= interval) && !(currentHour >= 20 || currentHour < 7))
  {
    refreshMessagesFromServer();
    previousMillis = currentMillis;
  }

  if (currentHour >= 20 || currentHour < 7)
  {
    Display.setIntensity(1);
    if (millis() - colonBlinkMillis >= colonBlinkInterval)
    {
      colonBlinkMillis = millis();
      showColon = !showColon;
    }
    char displayTime[6];
    strncpy(displayTime, currTime, sizeof(displayTime));
    if (!showColon)
      displayTime[2] = ' ';
    if (Display.displayAnimate())
    {
      Display.displayText(displayTime, PA_CENTER, 60, 0, PA_NO_EFFECT, PA_NO_EFFECT);
    }
  }
  else if (isDuringClasses())
  {
    if (millis() - switchTimer >= 4000)
    {
      showTime = !showTime;
      switchTimer = millis();
    }
    if (Display.displayAnimate())
    {
      if (showTime)
      {
        Display.displayText(currTime, PA_CENTER, 60, 10000, PA_SCROLL_DOWN, PA_SCROLL_UP);
      }
      else
      {
        Display.displayText("PUT MICRO zaprasza!", PA_CENTER, 60, 1000, PA_SPRITE, PA_SPRITE);
      }
    }
  }
  else
  {
    Display.setIntensity(4);
    if (Display.displayAnimate())
    {
      if (i == ARRAY_SIZE(animList))
        i = 0;
      Display.displayText(animList[i].textOut, animList[i].just, animList[i].speed,
                          animList[i].pause, animList[i].anim_in, animList[i].anim_out);
      i++;
    }
  }
}

String httpGETRequest(const char *serverName)
{
  HTTPClient http;
  http.begin(serverName);
  int httpResponseCode = http.GET();
  String payload = "--";
  if (httpResponseCode > 0)
  {
    payload = http.getString();
  }
  http.end();
  return payload;
}
