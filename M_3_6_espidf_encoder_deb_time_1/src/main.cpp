#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "encoder.h"
#include "debounce.h"

const gpio_num_t ENC_A_16 = GPIO_NUM_16;
const gpio_num_t ENC_B_17 = GPIO_NUM_17;
const gpio_num_t BUTTON_15 = GPIO_NUM_15;
const gpio_num_t LED_4 = GPIO_NUM_4;
constexpr uint8_t BUTTONS_COUNT = 1;

constexpr uint16_t ENCODER_DEBOUNCE_NS = 1000;
constexpr int32_t ENCODER_MAX_COUNT = 32767;
constexpr int32_t ENCODER_MIN_COUNT = -32768;

// Час антидребезгу (мс)
constexpr uint16_t DEBOUNCE_DELAY = 50;	   // час антидребезгу кнопки (мс)
constexpr uint16_t LONG_PRESS_TIME = 1500; // час довгого натискання (мс)
constexpr uint16_t REPEAT_INTERVAL = 500;  // інтервал повторення події (мс)

static const char *TAG = "Encoder";

// ------------------------------------------------------
static void buttons_process(deb *btns, uint8_t btns_count)
{
	for (uint8_t i = 0; i < btns_count; i++)
	{
		ButtonEvent event = deb_get_btn_event(&btns[i]);
		ButtonState status = deb_get_btn_status(&btns[i]);

		if (event == LONG_PRESS_EVENT || event == CLICK_EVENT)

		{
			gpio_set_level(LED_4, 1);
		}
		else if (status == RELEASED)
		{
			gpio_set_level(LED_4, 0);
		}
	}
}

extern "C" void app_main()
{
	//-------------ENC CONTEXT Init  ---------------
	enc_context enc_ctx = {};
	enc_ctx.low_limit = ENCODER_MIN_COUNT;
	enc_ctx.high_limit = ENCODER_MAX_COUNT;
	enc_ctx.max_glitch_ns = ENCODER_DEBOUNCE_NS;
	enc_ctx.gpio_num_a = GPIO_NUM_16;
	enc_ctx.gpio_num_b = GPIO_NUM_17;
	enc_ctx.initialized = false;

	ESP_ERROR_CHECK(enc_init(&enc_ctx));

	//-------------Ініціалізація GPIO LED-------------
	gpio_config_t led_conf = {};
	led_conf.pin_bit_mask = (1ULL << LED_4);
	led_conf.mode = GPIO_MODE_OUTPUT;
	led_conf.pull_up_en = GPIO_PULLUP_DISABLE;
	led_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
	led_conf.intr_type = GPIO_INTR_DISABLE;

	ESP_ERROR_CHECK(gpio_config(&led_conf));

	//-------------Ініціалізація GPIO BUTTON------------
	gpio_config_t btn_conf = {};
	btn_conf.pin_bit_mask = (1ULL << BUTTON_15);
	btn_conf.mode = GPIO_MODE_INPUT;
	btn_conf.pull_up_en = GPIO_PULLUP_ENABLE;
	btn_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
	btn_conf.intr_type = GPIO_INTR_DISABLE;

	ESP_ERROR_CHECK(gpio_config(&btn_conf));

	//-------------Ініціалізація DEBOUNCE------------

	deb btns[BUTTONS_COUNT] = {};
	// 1. Конфігураційні параметри (Заповнює користувач)
	btns[0].pin = BUTTON_15;
	btns[0].debounceDelay = DEBOUNCE_DELAY;
	btns[0].longPressTime = LONG_PRESS_TIME;
	btns[0].repeatInterval = REPEAT_INTERVAL;
	btns[0].activeLevel = 0; // Натиснута - 0; натиснута - 1

	deb_init(&btns[0]);

	int current_val = 0;

	while (1)
	{
		// Зчитуемо поточне значення PCNT
		ESP_ERROR_CHECK(enc_get_count(&enc_ctx, &current_val));

		// Зчитуємо стани входів A, B PCNT
		int a_state = gpio_get_level(ENC_A_16);
		int b_state = gpio_get_level(ENC_B_17);

		ESP_LOGI(TAG, "Position: %d | A: %d | B: %d", current_val, a_state, b_state);

		// Зчитуємо стан кнопки та керуємо LED
		ESP_ERROR_CHECK(deb_btns_update(btns, BUTTONS_COUNT));
		buttons_process(btns, BUTTONS_COUNT);

		vTaskDelay(pdMS_TO_TICKS(20));
	}
}