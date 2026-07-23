#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "wifi.h"
#include "telegram.h"

#include "esp_log.h"

static const char *TAG = "MAIN";

void app_main(void)
{
	ESP_LOGI(TAG, "Start");
	memory_print_info();

	int t = 45;

	wifi_init();

	if (wifi_wait_connected(10000))
	{
		ESP_LOGI(TAG, "WiFi OK");

		telegram_init();

		// telegram_send("TEMP ERR");
		telegram_sendf("T = %d C", t);
	}
	else
	{
		ESP_LOGE(TAG,
				 "WiFi connection timeout");
	}

	// КРИТИЧНО: Не даємо функції завершитися
	while (1)
	{
		vTaskDelay(pdMS_TO_TICKS(1000)); // Пауза в 1 секунду, щоб не навантажувати процесор
	}
}