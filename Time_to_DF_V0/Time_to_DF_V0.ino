//#include <ESP8266WiFi.h>
#define FASTLED_ESP32_I2S 
#include <WiFi.h> //
#include <WiFiUdp.h>
#include <FastLED.h>
#include <time.h>

// ==== Wi-Fi ====
const char* ssid     = "phys_net";
const char* password = "MSU54325";
//const char* ssid     = "pop_it";
//const char* password = "00000000";

// ==== Временная зона ====
const long gmtOffset_sec = 3 * 3600;  // GMT+3
const int daylightOffset_sec = 0;

// ==== Светодиоды ====
#define LED_PIN        2 // D2
#define NUM_LEDS       99
#define BRIGHTNESS     255
#define LED_TYPE       WS2812B
#define COLOR_ORDER    GRB
#define SIGN_LED_PIN   4 // D1
#define SIGN_NUM_LEDS  24

#define DF_SECOND 59
#define DF_MINUTE 59
#define DF_HOUR   23
#define DF_DAY    22

CRGB leds[NUM_LEDS];
CRGB sign_leds[SIGN_NUM_LEDS];
CRGB color = CRGB(255, 255, 255);
CRGB color_sig = CRGB(0, 255, 0);
CRGB dark  = CRGB(0, 0, 0);

const byte LEN = 2;

const byte mask[7] = {
  0b10000000, 0b01000000, 0b00100000,
  0b00010000, 0b00001000, 0b00000100, 0b00000010,
};

const byte digits[10] = {
  0b11111100, 0b01100000, 0b11011010, 0b11110010, 0b01100110,
  0b10110110, 0b10111110, 0b11100000, 0b11111110, 0b11110110
};

// Временные переменные
time_t bootTime = 0;        // Время старта (в секундах с 1970)
unsigned long bootMillis;   // Время запуска (millis)

void set_number(int segment, int digit) {
  int start = segment * LEN * 7 + 1;
  for (int i = 0; i < 7; i++) {
    int idx1 = start + i * LEN ;
    int idx2 = idx1 + 1 ;
    if ((digits[digit] & mask[i]) != 0) {
      leds[idx1] = color;
      leds[idx2] = color;
    } else {
      leds[idx1] = dark;
      leds[idx2] = dark;
    }
  }
}

void setup() {
  Serial.begin(9600);
  WiFi.begin(ssid, password);
  Serial.print("connecting to Wi-Fi");
  

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" conected!");

  configTime(gmtOffset_sec, daylightOffset_sec, "pool.ntp.org", "time.nist.gov");

  struct tm timeinfo;
  int retries = 30;
  while (!getLocalTime(&timeinfo) && retries-- > 0) {
    Serial.println("waiting NTP...");
    delay(500);
  }

  if (retries <= 0) {
    Serial.println("no time readed from NTP!");
    bootTime = 0;
  } else {
    time(&bootTime);  // Сохраняем timestamp
    bootMillis = millis();
    Serial.printf("time set: %02d:%02d:%02d, day: %02d\n",
      timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec, timeinfo.tm_mday);
  }

  WiFi.disconnect(true);  // Можно отключить Wi-Fi

  // Инициализация ленты
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.addLeds<LED_TYPE, SIGN_LED_PIN, COLOR_ORDER>(sign_leds, SIGN_NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  
  for (int i = 0; i < SIGN_NUM_LEDS; i++) {
    sign_leds[i] = color_sig;
  }
}

void loop() {
  // Текущее время = время старта + (миллисекунды/1000)
  time_t current = bootTime + (millis() - bootMillis) / 1000;
  struct tm timeinfo;
  localtime_r(&current, &timeinfo);

  int second = timeinfo.tm_sec;
  int minute = timeinfo.tm_min;
  int hour   = timeinfo.tm_hour;
  int day    = timeinfo.tm_mday;

  Serial.printf("%02d:%02d:%02d, day: %02d\n", hour, minute, second, day);

  //Serial.printf("XYN");

  // Обратный отсчёт
  set_number(3, (DF_SECOND - second) % 10);
  set_number(2, (DF_SECOND - second) / 10);
  set_number(1, (DF_MINUTE - minute) % 10);
  set_number(0, (DF_MINUTE - minute) / 10);
  set_number(6, (DF_HOUR - hour) % 10);
  set_number(5, (DF_HOUR - hour) / 10);
  set_number(4, (DF_DAY - day) % 10);

  FastLED.show();
  delay(1000);
}
