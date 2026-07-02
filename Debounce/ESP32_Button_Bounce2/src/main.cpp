#include <Arduino.h>
#include <Bounce2.h>

// Вкажіть будь-який вільний пін на вашій ESP32-S3
constexpr uint8_t BUTTON_PIN = 0;

constexpr uint8_t LED_PIN = 8;

// Налаштування часу (у мілісекундах)
constexpr uint16_t DEBOUNCE_INTERVAL = 30; // Час фільтрації дребезгу контакту
constexpr uint16_t LONG_PRESS_TIME = 1000; // Час до першого спрацьовування утримання (1 секунда)
constexpr uint16_t REPEAT_INTERVAL = 300;  // Інтервал між повторами під час утримання (0.2 секунди)

// Створюємо об'єкт кнопки з бібліотеки Bounce2
Bounce button = Bounce();

// Змінні для відстеження часу утримання кнопки
unsigned long pressStartTime = 0; // Час фізичного натискання кнопки
unsigned long lastRepeatTime = 0; // Час останнього повтору події
bool isLongPressActive = false;   // Прапорець, що довге натискання вже активне

void setup()
{
  Serial.begin(115200);
  delay(1000); // Даємо час для ініціалізації Serial порту
  Serial.println("Систему запущено. Очікування натискання кнопки...");

  // Налаштовуємо кнопку: пін, режим INPUT_PULLUP (натиснуто = 0, відпущено = 1) та час дребезгу
  button.attach(BUTTON_PIN, INPUT_PULLUP);
  button.interval(DEBOUNCE_INTERVAL);

  pinMode(LED_PIN, OUTPUT);
}

void loop()
{
  // Обов'язково оновлюємо стан кнопки в кожному циклі loop
  button.update();

  unsigned long now = millis(); // Фіксуємо поточний час один раз для точних розрахунків

  // 1. ПОДІЯ НАТИСКАННЯ (Перехід з 1 в 0)
  if (button.fell())
  {
    pressStartTime = now;
    isLongPressActive = false;
    Serial.println("Подія: Кнопку НАТИСНУТО (Фізичний контакт замкнуто, стан 0)");
  }

  // 2. ЛОГІКА ДОВГОГО НАТИСКАННЯ ТА АВТОПОВТОРУ (Поки кнопка утримується в стані LOW)
  if (button.read() == LOW)
  {
    unsigned long heldTime = now - pressStartTime;

    // Перевірка на перше спрацьовування тривалого натискання
    if (!isLongPressActive && heldTime >= LONG_PRESS_TIME)
    {
      isLongPressActive = true;
      lastRepeatTime = now; // Запам'ятовуємо час для старту інтервалів повтору
      Serial.println("Подія: ТРИВАЛЕ НАТИСКАННЯ (Перше спрацьовування)");
    }

    // Логіка автоповтору, якщо тривале натискання вже активне
    if (isLongPressActive && (now - lastRepeatTime >= REPEAT_INTERVAL))
    {
      lastRepeatTime = now; // Скидаємо таймер для наступного повтору
      Serial.println("Подія: АВТОПОВТОР (Кнопка утримується далі)");
    }
  }

  // 3. ПОДІЯ ВІДПУСКАННЯ (Перехід з 0 в 1)
  if (button.rose())
  {
    Serial.print("Подія: Кнопку ВІДПУЩЕНО (Стан 1). ");

    // Перевіряємо, чи це був швидкий клік, чи кнопку відпустили після утримання
    if (!isLongPressActive)
    {
      Serial.println("Результат: ШВИДКИЙ КЛІК");
    }
    else
    {
      Serial.println("Результат: Кінець тривалого утримання");
    }
  }

  // Перевіряємо поточний стан кнопки: LOW означає, що вона натиснута
  if (button.read() == LOW)
  {
    digitalWrite(LED_PIN, HIGH); // Вимикаємо світло (або вмикаємо, залежно від вашої схеми)
  }
  else
  {
    digitalWrite(LED_PIN, LOW); // Вмикаємо світло
  }
}