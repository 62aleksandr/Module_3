#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "gpio.h"

static const char *TAG = "ESP_LOG";

// Hardware pin definitions
const gpio_num_t GPIO_15 = GPIO_NUM_15;
const gpio_num_t GPIO_16 = GPIO_NUM_16;
const gpio_num_t GPIO_17 = GPIO_NUM_17;

// Global counter variable
uint8_t digit_count = 2;

extern "C" void app_main(void)
{
	ESP_LOGI(TAG, "INIT");

	pin_context pin_ctxs[] = {

		{.pin_bit = GPIO_15,
		 .mode = GPIO_MODE_OUTPUT,
		 .pull_up_en = GPIO_PULLUP_DISABLE,
		 .pull_down_en = GPIO_PULLDOWN_DISABLE,
		 .intr_type = GPIO_INTR_DISABLE,
		 .initialized = false},
		{.pin_bit = GPIO_16,
		 .mode = GPIO_MODE_OUTPUT,
		 .pull_up_en = GPIO_PULLUP_DISABLE,
		 .pull_down_en = GPIO_PULLDOWN_DISABLE,
		 .intr_type = GPIO_INTR_DISABLE,
		 .initialized = false},
		{.pin_bit = GPIO_17,
		 .mode = GPIO_MODE_OUTPUT,
		 .pull_up_en = GPIO_PULLUP_DISABLE,
		 .pull_down_en = GPIO_PULLDOWN_DISABLE,
		 .intr_type = GPIO_INTR_DISABLE,
		 .initialized = false}};

	// Автоматично вираховуємо кількість елементів у масиві
	constexpr size_t num_leds = sizeof(pin_ctxs) / sizeof(pin_ctxs[0]);

	// Обчислюємо 2 в степені num_leds
	constexpr uint32_t total_pin_states = (1U << num_leds);

	// 2. Ініціалізуємо піни
	for (size_t i = 0; i < num_leds; i++)
	{
		ESP_ERROR_CHECK(gpio_init(&pin_ctxs[i]));
		ESP_ERROR_CHECK(gpio_set(&pin_ctxs[i], 0));
	}

	// Масив станів цифр від 0 до 7
	bool digits[total_pin_states][num_leds] = {
		{0, 0, 0}, // 0
		{1, 0, 0}, // 1
		{0, 1, 0}, // 2
		{1, 1, 0}, // 3
		{0, 0, 1}, // 4
		{1, 0, 1}, // 5
		{0, 1, 1}, // 6
		{1, 1, 1}  // 7
	};

	// Цикл по кількості пінів
	for (size_t i = 0; i < num_leds; i++)
	{
		// Беремо значення (true/false) з таблиці станів для поточної цифри
		bool pin_state = digits[digit_count][i];

		// Встановлюємо стан для конкретного піна
		ESP_ERROR_CHECK(gpio_set(&pin_ctxs[i], pin_state));
	}
}
