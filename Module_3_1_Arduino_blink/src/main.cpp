#include <Arduino.h>

#define LED_PIN 16 // GPIO16 (LED)

#define BLINK_DELAY_MS 1000

void setup()
{
  pinMode(LED_PIN, OUTPUT);
}

void loop()
{
  digitalWrite(LED_PIN, HIGH);
  delay(BLINK_DELAY_MS);

  digitalWrite(LED_PIN, LOW);
  delay(BLINK_DELAY_MS);
}