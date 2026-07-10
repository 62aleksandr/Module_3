#include "timer.h"

void pwm_init(pwm_context *pwm_ctx)
{
	// Налаштування таймера
	ledc_timer_config_t timer_config = {
		.speed_mode = LEDC_LOW_SPEED_MODE,			 // Режим швидкостий/повільний таймер
		.duty_resolution = pwm_ctx->duty_resolution, // Роздільна здатність ШІМ
		.timer_num = pwm_ctx->timer_num,			 // Номер таймера LEDC
		.freq_hz = pwm_ctx->freq_hz,				 // Частота сигналу ШІМ (Гц)
		.clk_cfg = LEDC_AUTO_CLK,					 // Автоматичний вибір джерела CLK
		.deconfigure = false						 // Не виконувати деініціалізацію таймера
	};

	ledc_timer_config(&timer_config);

	// Налаштування каналу
	ledc_channel_config_t channel_config = {
		.gpio_num = pwm_ctx->gpio_num,				  // Номер GPIO для виходу ШІМ
		.speed_mode = LEDC_LOW_SPEED_MODE,			  // Режим швидкості LEDC
		.channel = pwm_ctx->channel,				  // Номер каналу ШІМ
		.intr_type = LEDC_INTR_DISABLE,				  // Вимкнення переривань LEDC
		.timer_sel = pwm_ctx->timer_num,			  // Таймер, прив'язаний до каналу
		.duty = 0,									  // Початкове значення заповнення ШІМ
		.hpoint = 0,								  // Початкова точка формування імп
		.sleep_mode = LEDC_SLEEP_MODE_NO_ALIVE_NO_PD, // Режим роботи у сні
		.flags = 0,									  // Додаткові параметри
		.deconfigure = false};						  // Не деініціалізувати канал

	ledc_channel_config(&channel_config);
}

// void pwm_set_duty(ledc_channel_t channel, int duty)
void pwm_set_duty(pwm_context *pwm_ctx, int duty)
{
	ledc_set_duty(LEDC_LOW_SPEED_MODE, pwm_ctx->channel, duty);
	ledc_update_duty(LEDC_LOW_SPEED_MODE, pwm_ctx->channel);
}
