#pragma once

#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_log.h"

typedef struct
{
	gpio_num_t pin_bit;
	gpio_mode_t mode; // Режим роботи GPIO - вихід
	gpio_pullup_t pull_up_en;
	gpio_pulldown_t pull_down_en;
	gpio_int_type_t intr_type; // Вимкнення переривань
	bool initialized;
} pin_context;

esp_err_t gpio_init(pin_context *ctx);
esp_err_t gpio_set(const pin_context *ctx, uint32_t level);
esp_err_t gpio_deinit(pin_context *ctx);