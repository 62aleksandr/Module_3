#include "memory.h"

#include "esp_log.h"

#include "esp_flash.h"

static const char *TAG = "MEMORY";

esp_err_t memory_print_info(void)
{
	uint32_t flash_size = 0;

	esp_err_t err = esp_flash_get_size(NULL, &flash_size);
	if (err != ESP_OK)
	{
		return err;
	}

	ESP_LOGI(TAG,
			 "Flash size : %u KB",
			 flash_size / 1024);

	size_t free_ram = heap_caps_get_free_size(MALLOC_CAP_INTERNAL);
	size_t min_ram = heap_caps_get_minimum_free_size(MALLOC_CAP_INTERNAL);
	size_t largest_ram = heap_caps_get_largest_free_block(MALLOC_CAP_INTERNAL);

	ESP_LOGI(TAG,
			 "Internal RAM");

	ESP_LOGI(TAG,
			 "  Free        : %u KB",
			 free_ram / 1024);

	ESP_LOGI(TAG,
			 "  Minimum     : %u KB",
			 min_ram / 1024);

	ESP_LOGI(TAG,
			 "  Largest     : %u KB",
			 largest_ram / 1024);

	return ESP_OK;
}