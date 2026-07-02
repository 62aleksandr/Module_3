#include <Arduino.h>

const uint8_t BUTTON_PIN = 0;

constexpr uint8_t LED_PIN = 8;

enum ButtonState
{
  RELEASED, // кнопка відпущена
  PRESSED   // кнопка натиснута
};

enum ButtonEvent
{
  NONE_EVENT,             // подія відсутня
  PRESSED_EVENT,          // момент натискання
  RELEASED_EVENT,         // момент відпускання
  CLICK_EVENT,            // коротке натискання
  LONG_PRESS_EVENT,       // довге утримання
  LONG_PRESS_REPEAT_EVENT // повтор довгого утримання
};

struct Button
{
  uint8_t pin;                    // номер GPIO піна кнопки
  bool lastPinState;              // попередній  стан кнопки
  ButtonState pinState;           // поточний стан кнопки
  ButtonEvent event;              // подія кнопки
  unsigned long lastDebounceTime; // час останньої зміни стану
  unsigned long pressStartTime;   // час початку натискання кнопки
  unsigned long lastRepeatTime;   // час останнього повтору довгого натискання
  bool longPressActive;           // прапорець довгого натискання
};

// Час антидребезгу (мс)
constexpr uint16_t debounceDelay = 50;   // час антидребезгу кнопки (мс)
constexpr uint16_t longPressTime = 1500; // час довгого натискання (мс)
constexpr uint16_t repeatInterval = 500; // інтервал повторення події при утриманні кнопки (мс)

Button buttons[1] =
    {
        {BUTTON_PIN, HIGH, RELEASED, NONE_EVENT, 0, 0, 0, false}};

// Лічильник натискань кнопки
uint32_t pressCount = 0;
uint32_t releaseCount = 0;

ButtonEvent debounceButton(Button &btn);

void setup()
{
  Serial.begin(115200);
  delay(1000);

  // Кнопка підключена PULLUP
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  pinMode(LED_PIN, OUTPUT);
}

void loop()
{
  ButtonEvent event = debounceButton(buttons[0]);

  if (event == PRESSED_EVENT)
    Serial.println("PRESS_EVENT");

  if (event == LONG_PRESS_EVENT)
    Serial.println("LONG_PRESS_EVENT");

  if (event == LONG_PRESS_REPEAT_EVENT)
    Serial.println("LONG_PRESS_REPEAT_EVENT");

  if (event == CLICK_EVENT)
    Serial.println("CLICK_EVENT");

  if (event == RELEASED_EVENT)
    Serial.println("RELEASED_EVENT");

  if (buttons[0].pinState == PRESSED)
  {
    digitalWrite(LED_PIN, HIGH); // Вимикаємо світло
  }
  else
  {
    digitalWrite(LED_PIN, LOW); // Вмикаємо світло
  }
}

ButtonEvent debounceButton(Button &btn)
{
  unsigned long now = millis();
  bool currentPinState = digitalRead(btn.pin);

  // debounce trigger
  if (currentPinState != btn.lastPinState)
  {
    btn.lastDebounceTime = now;
  }

  btn.lastPinState = currentPinState;

  // stable state check
  if ((now - btn.lastDebounceTime) >= debounceDelay)
  {
    ButtonState newPinState = (currentPinState == LOW) ? PRESSED : RELEASED;

    // state changed
    if (newPinState != btn.pinState)
    {
      btn.pinState = newPinState;

      if (newPinState == PRESSED)
      {
        btn.pressStartTime = now;
        btn.longPressActive = false;
        btn.lastRepeatTime = 0;

        btn.event = PRESSED_EVENT;
        return btn.event;
      }
      else
      {
        btn.event = (!btn.longPressActive) ? CLICK_EVENT : RELEASED_EVENT;
        return btn.event;
      }
    }
  }

  // LONG PRESS
  if (btn.pinState == PRESSED)
  {
    unsigned long heldTime = now - btn.pressStartTime;

    // Перший long press
    if (!btn.longPressActive && heldTime >= longPressTime)
    {
      btn.longPressActive = true;
      btn.lastRepeatTime = now;

      btn.event = LONG_PRESS_EVENT;
      return btn.event;
    }

    // повторний long press
    if (btn.longPressActive &&
        (now - btn.lastRepeatTime >= repeatInterval))
    {
      btn.lastRepeatTime = now;

      btn.event = LONG_PRESS_REPEAT_EVENT;
      return btn.event;
    }
  }

  btn.event = NONE_EVENT;
  return btn.event;
}
