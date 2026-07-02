#include <Arduino.h>
#include "FsmButton.h"

constexpr u_int8_t LED_PIN8 = 8;

// Створюємо об’єкт кнопки GPIO 0
FsmButton bootButton(0);

void setup()
{
  Serial.begin(115200);

  // Ініціалізуємо FSM кнопку
  bootButton.begin();

  pinMode(LED_PIN8, OUTPUT);
}

void loop()
{
  // Постійно оновлюємо автомат у фоні
  bootButton.update();

  // Проверяем события кнопки
  if (bootButton.wasPressed())
  {
    Serial.println("Подія: Одиночне коротке натискання!");
    digitalWrite(LED_PIN8, HIGH);
  }

  if (bootButton.wasLongPressed())
  {
    Serial.println("Подія: Кнопка утримується більше 1 секунди!");
  }

  if (bootButton.wasReleased())
  {
    Serial.println("Подія: Кнопка успішно відпущена.");
    digitalWrite(LED_PIN8, LOW);
  }
}
