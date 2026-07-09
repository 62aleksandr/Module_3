#pragma once

#include "driver/ledc.h"

typedef struct
{
	int gpio_num;
	ledc_timer_t timer_num;
	ledc_channel_t channel;
	uint32_t freq_hz;
	ledc_timer_bit_t duty_resolution;
} timer_parameters;

void pwm_init(timer_parameters *timer_param);
void pwm_set_duty(int duty, ledc_channel_t channel);