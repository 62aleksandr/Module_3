#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"

// Налаштування піна та ШИМ-периферії для ESP32-S3
#define BUZZER_PIN (16) // GPIO 16
#define LEDC_TIMER LEDC_TIMER_0
#define LEDC_MODE LEDC_LOW_SPEED_MODE // Низкоскоростной режим для ESP32-S3
#define LEDC_CHANNEL LEDC_CHANNEL_0
#define LEDC_DUTY_RES LEDC_TIMER_8_BIT // 8-бітовий дозвіл (0-255)

// Розрахунок шпаруватості (50% від 2^8 - 1 = 128) для отримання звуку (меандра)
#define LEDC_DUTY_50_PERCENT (128)

// Частоти нот (у Герцах)
#define NOTE_A4 440
#define NOTE_A5 880
#define NOTE_F4 349
#define NOTE_C5 523
#define NOTE_E5 659
#define NOTE_G5 784
#define NOTE_GS5 831
#define NOTE_F5 698

// Масив нот "Imperial March"
const int melody[] = {
	NOTE_A4, NOTE_A4, NOTE_A4, NOTE_F4, NOTE_C5, NOTE_A4, NOTE_F4, NOTE_C5, NOTE_A4,
	NOTE_E5, NOTE_E5, NOTE_E5, NOTE_F5, NOTE_C5, NOTE_GS5, NOTE_F4, NOTE_C5, NOTE_A4};

// Тривалість кожної ноти
const int durations[] = {
	4, 4, 4, 8, 16, 4, 8, 16, 2,
	4, 4, 4, 8, 16, 4, 8, 16, 2};

// Ініціалізація LEDC (ШИМ)
void init_pwm_buzzer(void)
{
	// 1. Конфігурація Таймера
	ledc_timer_config_t ledc_timer = {
		.speed_mode = LEDC_MODE,
		.timer_num = LEDC_TIMER,
		.duty_resolution = LEDC_DUTY_RES,
		.freq_hz = 2000, // Початкова частота за замовчуванням
		.clk_cfg = LEDC_AUTO_CLK};
	ledc_timer_config(&ledc_timer);

	// 2. Конфігурація Канала
	ledc_channel_config_t ledc_channel = {
		.speed_mode = LEDC_MODE,
		.channel = LEDC_CHANNEL,
		.timer_sel = LEDC_TIMER,
		.intr_type = LEDC_INTR_DISABLE,
		.gpio_num = BUZZER_PIN,
		.duty = 0, // Спочатку тиша (скважність 0)
		.hpoint = 0};
	ledc_channel_config(&ledc_channel);
}

void app_main(void)
{
	// Ініціалізуємо ШИМ
	init_pwm_buzzer();

	int size = sizeof(melody) / sizeof(int);
	int speed = 1200; // Базовый темп марша

	while (1)
	{
		for (int thisNote = 0; thisNote < size; thisNote++)
		{
			// Вичислюємо тривалість поточної ноти в мілісекундах
			int noteDuration = speed / durations[thisNote];

			// 1. Змінюємо частоту ШИМ-таймера под поточну ноту
			ledc_set_freq(LEDC_MODE, LEDC_TIMER, melody[thisNote]);

			// 2. Встановлюємо скважність на 50%, щоб запустити звук
			ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, LEDC_DUTY_50_PERCENT);
			ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);

			// Ждемо, поки нота звучить (80% времени)
			vTaskDelay(pdMS_TO_TICKS(noteDuration * 0.80));

			// 3. Вимикаємо звук (скважність 0), створюючи паузу між нотами
			ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, 0);
			ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);

			// Коротка тиша між нотами (20% часу, що залишилися)
			vTaskDelay(pdMS_TO_TICKS(noteDuration * 0.20));
		}

		// Пауза 4 секунди перед повтором мелодии
		vTaskDelay(pdMS_TO_TICKS(4000));
	}
}