#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "adc.h"

//-------------ADC1 Config---------------
void adc_init(adc_oneshot_unit_handle_t *adc1_handle)
{
	adc_oneshot_unit_init_cfg_t init_config1 = {
		.unit_id = ADC_UNIT_1,				// Використання ADC 1
		.clk_src = ADC_RTC_CLK_SRC_DEFAULT, // стандартне джерело CLK
		.ulp_mode = ADC_ULP_MODE_DISABLE,	// вимкнення режиму ULP
	};

	ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, adc1_handle)); // Створення дескриптора ADC 1

	adc_oneshot_chan_cfg_t config = {
		.atten = ADC_ATTEN_DB_12,		  // Діапазон до ~3.1В
		.bitwidth = ADC_BITWIDTH_DEFAULT, // Зазвичай 12 біт
	};

	ESP_ERROR_CHECK(adc_oneshot_config_channel(*adc1_handle, ADC_CHANNEL_3, &config)); // Налаштування каналу 3 ADC 1
}

//-------------ADC1 Calibration ---------------
void adc_calibration_init(adc_cali_handle_t *cali_handle)
{
	adc_cali_curve_fitting_config_t cali_config = {
		.unit_id = ADC_UNIT_1,
		.chan = ADC_CHANNEL_3,
		.atten = ADC_ATTEN_DB_12,
		.bitwidth = ADC_BITWIDTH_DEFAULT,
	}; // Налаштування калібрування для каналу 3 ADC 1

	ESP_ERROR_CHECK(adc_cali_create_scheme_curve_fitting(&cali_config, cali_handle)); // Створення дескриптора калібрування

	// adc_cali_delete_scheme_curve_fitting(cali_handle)); // Видалення попереднього дескриптора калібрування, якщо він існує
}
