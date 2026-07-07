#include "adc_calibration.h"

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