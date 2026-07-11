#include <stdio.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h" // Драйвер для роботи з апаратним ШІМ (LEDC)

// Математичні константи та налаштування сигналу
#define PI 3.14159265358979323846
#define POINTS_COUNT 100 // Кількість точок на один повний період сигналу

// Налаштування затримки для плоттера
// Збільшіть це значення (наприклад, до 50), якщо графік біжить занадто швидко.
#define DELAY_MS 30 // Затримка в мілісекундах між виведенням точок у printf

// Призначення фізичних пінів для виходу ШІМ на ESP32-S3
#define SINE_PWM_PIN 4	   // GPIO для виходу синусоїди
#define TRIANGLE_PWM_PIN 5 // GPIO для виходу трикутного сигналу

/**
 * @brief Функція розрахунку трикутного сигналу
 * @param step Поточний крок циклу
 * @param total_steps Загальна кількість точок на період
 * @return Значення від 0.0 до 1.0
 */
float calculate_triangle(int step, int total_steps)
{
	float position = (float)step / total_steps; // Позиція на графіку від 0.0 до 1.0
	if (position < 0.5f)
	{
		return position * 2.0f; // Лінійний підйом від 0 до 1
	}
	else
	{
		return 2.0f - (position * 2.0f); // Лінійний спуск від 1 до 0
	}
}

/**
 * @brief Ініціалізація апаратного ШІМ (LEDC)
 */
void init_pwm(void)
{
	// 1. Налаштування таймера ШІМ (10 біт, частота несучої 5 кГц)
	ledc_timer_config_t ledc_timer = {
		.speed_mode = LEDC_LOW_SPEED_MODE,
		.timer_num = LEDC_TIMER_0,
		.duty_resolution = LEDC_TIMER_10_BIT, // Роздільна здатність 0 - 1023
		.freq_hz = 5000,					  // Частота ШІМ 5 кГц
		.clk_cfg = LEDC_AUTO_CLK};
	ledc_timer_config(&ledc_timer);

	// 2. Налаштування каналу для Синусоїди (Канал 0, GPIO 4)
	ledc_channel_config_t ledc_sine = {
		.speed_mode = LEDC_LOW_SPEED_MODE,
		.channel = LEDC_CHANNEL_0,
		.timer_sel = LEDC_TIMER_0,
		.intr_type = LEDC_INTR_DISABLE,
		.gpio_num = SINE_PWM_PIN,
		.duty = 0,
		.hpoint = 0};
	ledc_channel_config(&ledc_sine);

	// 3. Налаштування каналу для Трикутника (Канал 1, GPIO 5)
	ledc_channel_config_t ledc_triangle = {
		.speed_mode = LEDC_LOW_SPEED_MODE,
		.channel = LEDC_CHANNEL_1,
		.timer_sel = LEDC_TIMER_0,
		.intr_type = LEDC_INTR_DISABLE,
		.gpio_num = TRIANGLE_PWM_PIN,
		.duty = 0,
		.hpoint = 0};
	ledc_channel_config(&ledc_triangle);
}

/**
 * @brief Головна функція програми (Точка входу ESP-IDF)
 */
void app_main(void)
{
	// Вмикаємо та налаштовуємо ШІМ периферію
	init_pwm();

	int step = 0; // Змінна для відстеження поточного кроку всередині періоду

	// Головний нескінченний цикл
	while (1)
	{
		// --- 1. РОЗРАХУНОК СИНУСОЇДИ (Діапазон 0 - 1023) ---
		float angle = (2.0f * PI * step) / POINTS_COUNT;
		// sinf дає [-1; 1] -> (+1) дає [0; 2] -> (* 511.5) масштабує в [0; 1023]
		uint32_t sine_pwm = (uint32_t)((sinf(angle) + 1.0f) * 511.5f);

		// --- 2. РОЗРАХУНОК ТРИКУТНИКА (Діапазон 0 - 1023) ---
		// Функція повертає [0.0; 1.0] -> просто помножуємо на максимальне значення 1023
		uint32_t triangle_pwm = (uint32_t)(calculate_triangle(step, POINTS_COUNT) * 1023.0f);

		// --- 3. ОНОВЛЕННЯ ДАНИХ НА ФІЗИЧНИХ ПІНАХ ---
		// Передаємо розраховане заповнення (duty) в канали ШІМ
		ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, sine_pwm);
		ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);

		ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, triangle_pwm);
		ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1);

		// --- 4. ВИВЕДЕННЯ В SERIAL PLOTTER ---
		// Формат запису "Ім'я_Графіка:Значення" через кому є стандартним для плоттерів
		printf("S:%lu,T:%lu\n", sine_pwm, triangle_pwm);

		// Перехід до наступної точки сигналу
		step++;
		if (step >= POINTS_COUNT)
		{
			step = 0; // Скидаємо крок на нуль, якщо період завершився
		}

		// --- 5. ЗАДЕРЖКА ДЛЯ СТАБІЛІЗАЦІЇ ГРАФІКУ ---
		// Тимчасово блокує задачу, щоб плоттер встигав коректно малювати лінії
		vTaskDelay(pdMS_TO_TICKS(DELAY_MS));
	}
}