#include "timer.h"

void pwm_init(timer_parameters *timer_param)
{
	// Налаштування таймера
	ledc_timer_config_t timer_config = {
		.speed_mode = LEDC_LOW_SPEED_MODE,				 // Режим швидкостий/повільний таймер
		.duty_resolution = timer_param->duty_resolution, // Роздільна здатність ШІМ
		.timer_num = timer_param->timer_num,			 // Номер таймера LEDC
		.freq_hz = timer_param->freq_hz,				 // Частота сигналу ШІМ (Гц)
		.clk_cfg = LEDC_AUTO_CLK,						 // Автоматичний вибір джерела CLK
		.deconfigure = false							 // Не виконувати деініціалізацію таймера
	};

	ledc_timer_config(&timer_config);

	// Налаштування каналу
	ledc_channel_config_t channel_config = {
		.gpio_num = timer_param->gpio_num,			  // Номер GPIO для виходу ШІМ
		.speed_mode = LEDC_LOW_SPEED_MODE,			  // Режим швидкості LEDC
		.channel = timer_param->channel,			  // Номер каналу ШІМ
		.intr_type = LEDC_INTR_DISABLE,				  // Вимкнення переривань LEDC
		.timer_sel = timer_param->timer_num,		  // Таймер, прив'язаний до каналу
		.duty = 0,									  // Початкове значення заповнення ШІМ
		.hpoint = 0,								  // Початкова точка формування імп
		.sleep_mode = LEDC_SLEEP_MODE_NO_ALIVE_NO_PD, // Режим роботи у сні
		.flags = 0,									  // Додаткові параметри
		.deconfigure = false};						  // Не деініціалізувати канал

	ledc_channel_config(&channel_config);
}

void pwm_set_duty(int duty, ledc_channel_t channel)
{
	ledc_set_duty(LEDC_LOW_SPEED_MODE, channel, duty);
	ledc_update_duty(LEDC_LOW_SPEED_MODE, channel);
}
