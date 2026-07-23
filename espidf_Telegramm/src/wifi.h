#pragma once

#include "esp_err.h"

#include <stdbool.h>
#include <stdint.h>

esp_err_t wifi_init(void);

bool wifi_is_connected(void);

bool wifi_wait_connected(uint32_t timeout_ms);
