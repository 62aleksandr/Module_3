#pragma once

#include "esp_adc/adc_oneshot.h"
#include "esp_log.h"
#include "esp_err.h"

typedef struct
{
	adc_unit_t unit_id;
	adc_bitwidth_t bitwidth;
	adc_channel_t *channels;
	uint8_t channel_count;
	adc_cali_handle_t *cali_handles;

} adc_context;

void adc_init(adc_oneshot_unit_handle_t *adc_handle, adc_context *adc_init);
