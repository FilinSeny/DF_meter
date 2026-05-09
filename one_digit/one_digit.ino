#include <FastLED.h>

#define LED_PIN     2
#define NUM_LEDS    14
#define BRIGHTNESS  80
#define LED_TYPE    WS2815
#define COLOR_ORDER GRB

#define LEN 2

CRGB leds[NUM_LEDS];

const byte mask[7] =
{
  0b10000000,
  0b01000000,
  0b00100000,
  0b00010000,
  0b00001000,
  0b00000100,
  0b00000010,
};

const byte digits[10] =
{
  0b11111100, // 0
  0b01100000, // 1
  0b11011010, // 2
  0b11110010, // 3
  0b01100110, // 4
  0b10110110, // 5
  0b10111110, // 6
  0b11100000, // 7
  0b11111110, // 8
  0b11110110  // 9
};

void clearDisplay()
{
  for (int i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = CRGB::Black;
  }
}

void setNumber(int digit)
{
  clearDisplay();

  for (int i = 0; i < 7; i++)
  {
    int led1 = i * LEN;
    int led2 = i * LEN + 1;

    if ((digits[digit] & mask[i]) != 0)
    {
      leds[led1] = CRGB::White;
      leds[led2] = CRGB::White;
    }
  }

  FastLED.show();
}

void setup()
{
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);

  FastLED.setBrightness(BRIGHTNESS);

  clearDisplay();
  FastLED.show();
}

void loop()
{
  for (int digit = 0; digit <= 9; digit++)
  {
    setNumber(digit);
    delay(1000);
  }
}