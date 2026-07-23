#pragma once

#include "esp_err.h"

esp_err_t telegram_init(void);
esp_err_t telegram_send(const char *message);
esp_err_t telegram_sendf(const char *format, ...);
