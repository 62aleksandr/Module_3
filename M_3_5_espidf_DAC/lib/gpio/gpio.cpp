#include "gpio.h"

//------------- Налаштування  GPIO LED -------
esp_err_t gpio_init(pin_context *ctx)
{
	// Перевірка коректності аргументів
	if (ctx == NULL)
	{
		return ESP_ERR_INVALID_ARG;
	}
	// Перевірка стану ініціалізації
	if (ctx->initialized)
	{
		return ESP_ERR_INVALID_STATE;
	}
	// Конфігурація GPIO
	gpio_config_t io_conf = {};

	io_conf.pin_bit_mask = (1ULL << ctx->pin_bit);
	io_conf.mode = ctx->mode;
	io_conf.pull_up_en = ctx->pull_up_en;
	io_conf.pull_down_en = ctx->pull_down_en;
	io_conf.intr_type = ctx->intr_type;

	esp_err_t err = gpio_config(&io_conf);
	if (err != ESP_OK)
	{
		return err;
	}

	ctx->initialized = true;

	return ESP_OK;
}

esp_err_t gpio_set(const pin_context *ctx, uint32_t level)
{
	// Перевірка коректності аргументів
	if (ctx == NULL || !ctx->initialized)
	{
		return ESP_ERR_INVALID_ARG;
	}

	// GPIO підтримує тільки LOW/HIGH
	if (level > 1)
	{
		return ESP_ERR_INVALID_ARG;
	}

	// Встановити level
	esp_err_t err = gpio_set_level(ctx->pin_bit, level);
	if (err != ESP_OK)
	{
		return err;
	}

	return ESP_OK;
}

//------------- Деініціалізації gpio ---------------
esp_err_t gpio_deinit(pin_context *ctx)
{
	// Перевірка коректності аргументів
	if (ctx == NULL)
	{
		return ESP_ERR_INVALID_ARG;
	}

	// Перевірка стану ініціалізації
	if (!ctx->initialized)
	{
		return ESP_OK;
	}

	// Скидання конфігурації
	esp_err_t err = gpio_reset_pin(ctx->pin_bit);
	if (err != ESP_OK)
	{
		return err;
	}

	ctx->initialized = false;
	return ESP_OK;
}
