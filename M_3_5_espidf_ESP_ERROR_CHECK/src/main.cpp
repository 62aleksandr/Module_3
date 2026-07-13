#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "gpio.h"

static const char *TAG = "ESP_LOG";

// constexpr gpio_num_t LED_PIN_16 = GPIO_NUM_16;

const gpio_num_t BLINK_GPIO = GPIO_NUM_18;

extern "C" void app_main(void)
{
	ESP_LOGI(TAG, "INIT");
	//------------- PIN Init ---------------
	led_context led_ctx = {};
	led_ctx.pin_bit = BLINK_GPIO;
	led_ctx.mode = GPIO_MODE_OUTPUT;
	led_ctx.pull_up_en = GPIO_PULLUP_DISABLE;
	led_ctx.pull_down_en = GPIO_PULLDOWN_DISABLE;
	led_ctx.intr_type = GPIO_INTR_DISABLE;
	led_ctx.initialized = false;

	// Автоматична перевірка
	ESP_ERROR_CHECK(gpio_init(&led_ctx));
	ESP_ERROR_CHECK(gpio_set(&led_ctx, 1));

	ESP_LOGI(TAG, "LED ініциалізована");
}
