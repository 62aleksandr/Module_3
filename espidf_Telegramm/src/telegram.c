#include "telegram.h"

#include <stdio.h>
#include <string.h>

#include "esp_log.h"
#include "esp_http_client.h"
#include "esp_crt_bundle.h"

#include "wifi.h"
#include <stdarg.h>

#include "wifi.h"

static const char *TAG = "TELEGRAM";

#ifndef TELEGRAM_BOT_TOKEN
#error "TELEGRAM_BOT_TOKEN is not defined"
#endif

#ifndef TELEGRAM_CHAT_ID
#error "TELEGRAM_CHAT_ID is not defined"
#endif

static char telegram_url[256];

esp_err_t telegram_init(void)
{
	snprintf(telegram_url,
			 sizeof(telegram_url),
			 "https://api.telegram.org/bot%s/sendMessage",
			 TELEGRAM_BOT_TOKEN);

	ESP_LOGI(TAG, "Telegram initialized");

	return ESP_OK;
}

esp_err_t telegram_send(const char *message)
{
	if (message == NULL)
	{
		return ESP_ERR_INVALID_ARG;
	}

	if (!wifi_is_connected())
	{
		ESP_LOGE(TAG, "WiFi not connected");
		return ESP_ERR_INVALID_STATE;
	}

	char post_data[128];

	snprintf(post_data,
			 sizeof(post_data),
			 "{\"chat_id\":\"%s\",\"text\":\"%s\"}",
			 TELEGRAM_CHAT_ID,
			 message);

	esp_http_client_config_t config =
		{
			.url = telegram_url,
			.crt_bundle_attach = esp_crt_bundle_attach,
			.timeout_ms = 5000,
		};

	esp_http_client_handle_t client =
		esp_http_client_init(&config);

	if (client == NULL)
	{
		ESP_LOGE(TAG, "HTTP client init failed");
		return ESP_FAIL;
	}

	esp_http_client_set_method(
		client,
		HTTP_METHOD_POST);

	esp_http_client_set_header(
		client,
		"Content-Type",
		"application/json");

	esp_http_client_set_post_field(
		client,
		post_data,
		strlen(post_data));

	esp_err_t err =
		esp_http_client_perform(client);

	if (err == ESP_OK)
	{
		ESP_LOGI(TAG, "Message sent");
	}
	else
	{
		ESP_LOGE(TAG, "Send failed: %s",
				 esp_err_to_name(err));
	}

	if (err == ESP_OK)
	{
		// Перевіряємо реальний код відповіді від Telegram (200 = успішно)
		int status_code = esp_http_client_get_status_code(client);

		if (status_code == 200)
		{
			ESP_LOGI(TAG, "Message sent successfully to Telegram");
		}
		else
		{
			ESP_LOGE(TAG, "Telegram rejected request! HTTP Status: %d", status_code);
			err = ESP_FAIL; // Змінюємо статус на помилку, бо сервер відхилив запит
		}
	}
	else
	{
		ESP_LOGE(TAG, "Network connection failed: %s",
				 esp_err_to_name(err));
	}

	esp_http_client_cleanup(client);

	return err;
}

// esp_err_t telegram_sendf(const char *format, ...)
// {
// 	if (format == NULL)
// 	{
// 		return ESP_ERR_INVALID_ARG;
// 	}

// 	char message[256];

// 	va_list args;

// 	va_start(args, format);

// 	vsnprintf(message,
// 			  sizeof(message),
// 			  format,
// 			  args);

// 	va_end(args);

// 	ESP_LOGI(TAG, "Message = %s", message);

// 	return telegram_send(message);
// }

esp_err_t telegram_sendf(const char *format, ...)
{
	if (format == NULL)
	{
		return ESP_ERR_INVALID_ARG;
	}

	// СТАЛО: static звільняє стек задачі main!
	// Пам'ять виділяється в загальній RAM, а не на стеку.
	char message[64];

	// Обов'язково очищаємо статичний буфер перед кожним записом
	memset(message, 0, sizeof(message));

	va_list args;
	va_start(args, format);

	int written = vsnprintf(message, sizeof(message), format, args);

	va_end(args);

	if (written < 0 || written >= sizeof(message))
	{
		ESP_LOGW(TAG, "Formatted message was too long or failed!");
	}

	ESP_LOGI(TAG, "Sending formatted text: %s", message);

	return telegram_send(message);
}