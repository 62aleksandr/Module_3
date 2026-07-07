#pragma once
#include "esp_adc/adc_oneshot.h"
#include "esp_log.h"
#include "esp_err.h"

void adc_init(adc_oneshot_unit_handle_t *handle);