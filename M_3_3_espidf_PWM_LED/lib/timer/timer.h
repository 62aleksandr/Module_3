#pragma once

#include "driver/ledc.h"

typedef struct
{
	int gpio_num;
	ledc_timer_t timer_num;
	ledc_channel_t channel;
	uint32_t freq_hz;
	ledc_timer_bit_t duty_resolution;
} pwm_context;

void pwm_init(pwm_context *timer_param);
void pwm_set_duty(pwm_context *timer_param, int duty);