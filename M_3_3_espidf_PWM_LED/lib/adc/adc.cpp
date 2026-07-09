#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "adc.h"

//-------------ADC1 Config + Calibration---------------
void adc_init(adc_oneshot_unit_handle_t *adc_handle, adc_init_conf *adc_init)
{
	//-------------ADC1 Config ---------------
	adc_oneshot_unit_init_cfg_t init_config = {
		.unit_id = adc_init->unit_id,		// Використання ADC 1 ADC_UNIT_1
		.clk_src = ADC_RTC_CLK_SRC_DEFAULT, // стандартне джерело CLK
		.ulp_mode = ADC_ULP_MODE_DISABLE,	// вимкнення режиму ULP
	};

	ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config, adc_handle)); // Створення дескриптора ADC 1

	adc_oneshot_chan_cfg_t config = {
		.atten = ADC_ATTEN_DB_12,		// Діапазон до ~3.1В
		.bitwidth = adc_init->bitwidth, // Зазвичай 12 біт ADC_BITWIDTH_DEFAULT
	};

	for (uint8_t i = 0; i < adc_init->channel_count; i++)
	{
		ESP_ERROR_CHECK(
			adc_oneshot_config_channel(*adc_handle, adc_init->channels[i], &config));
	}

	//-------------ADC1 Calibration ---------------

	for (uint8_t i = 0; i < adc_init->channel_count; i++)
	{
		adc_cali_curve_fitting_config_t cali_config = {
			.unit_id = adc_init->unit_id,
			.chan = adc_init->channels[i],
			.atten = ADC_ATTEN_DB_12,
			.bitwidth = adc_init->bitwidth,
		};

		ESP_ERROR_CHECK(
			adc_cali_create_scheme_curve_fitting(&cali_config, &adc_init->cali_handles[i]));
	}

	// Створення дескриптора калібрування

	// adc_cali_delete_scheme_curve_fitting(cali_handle)); // Видалення попереднього дескриптора калібрування
}
