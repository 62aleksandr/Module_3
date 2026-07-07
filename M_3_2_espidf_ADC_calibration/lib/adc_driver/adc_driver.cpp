#include "adc_driver.h"

void adc_init(adc_oneshot_unit_handle_t *adc1_handle)
{
	adc_oneshot_unit_init_cfg_t init_config1 = {
		.unit_id = ADC_UNIT_1,				// Використання ADC 1
		.clk_src = ADC_RTC_CLK_SRC_DEFAULT, // стандартне джерело CLK
		.ulp_mode = ADC_ULP_MODE_DISABLE,	// вимкнення режиму ULP
	};

	ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, adc1_handle)); // Створення дескриптора ADC 1

	//-------------ADC1 Config---------------//
	adc_oneshot_chan_cfg_t config = {
		.atten = ADC_ATTEN_DB_12,		  // Діапазон до ~3.1В
		.bitwidth = ADC_BITWIDTH_DEFAULT, // Зазвичай 12 біт
	};

	ESP_ERROR_CHECK(adc_oneshot_config_channel(*adc1_handle, ADC_CHANNEL_3, &config)); // Налаштування каналу 3 ADC 1
}