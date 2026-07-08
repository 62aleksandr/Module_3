#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/ledc.h"
#include "adc.h"

constexpr int LED_PIN = 16;
constexpr ledc_timer_t LEDC_TIMER = LEDC_TIMER_0;
constexpr ledc_channel_t PWM_CHANNEL = LEDC_CHANNEL_7;
constexpr int LEDC_FREQUENCY = 5000;                            // 1 kHz
constexpr ledc_timer_bit_t LEDC_RESOLUTION = LEDC_TIMER_10_BIT; // 10-bit (0-1023)

extern "C" void app_main(void)
{
    //-------------ADC1 Init---------------
    adc_oneshot_unit_handle_t adc1_handle; // Дескриптор апаратного модуля ADC 1
    adc_init(&adc1_handle);

    //-------------ADC1 Calibration Init---------------
    adc_cali_handle_t cali_handle = NULL; // Дескриптор калібрування
    adc_calibration_init(&cali_handle);

    //-------------Timer LED Init---------------
    // Налаштування таймера
    ledc_timer_config_t timer_config = {
        .speed_mode = LEDC_LOW_SPEED_MODE,  // Режим швидкостий/повільний таймер
        .duty_resolution = LEDC_RESOLUTION, // Роздільна здатність ШІМ
        .timer_num = LEDC_TIMER,            // Номер таймера LEDC
        .freq_hz = LEDC_FREQUENCY,          // Частота сигналу ШІМ (Гц)
        .clk_cfg = LEDC_AUTO_CLK,           // Автоматичний вибір джерела CLK
        .deconfigure = false                // Не виконувати деініціалізацію таймера
    };

    ledc_timer_config(&timer_config);

    // Налаштування каналу
    ledc_channel_config_t channel_config = {
        .gpio_num = LED_PIN,                          // Номер GPIO для виходу ШІМ
        .speed_mode = LEDC_LOW_SPEED_MODE,            // Режим швидкості LEDC
        .channel = PWM_CHANNEL,                       // Номер каналу ШІМ
        .intr_type = LEDC_INTR_DISABLE,               // Вимкнення переривань LEDC
        .timer_sel = LEDC_TIMER,                      // Таймер, прив'язаний до каналу
        .duty = 0,                                    // Початкове значення заповнення ШІМ
        .hpoint = 0,                                  // Початкова точка формування імп
        .sleep_mode = LEDC_SLEEP_MODE_NO_ALIVE_NO_PD, // Режим роботи у сні
        .flags = 0,                                   // Додаткові параметри
        .deconfigure = false};                        // Не деініціалізувати канал

    ledc_channel_config(&channel_config);

    while (1)
    {
        int adc_raw;
        int voltage_calibrated;

        // Зчитування сирого значення (0-4095)
        ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, ADC_CHANNEL_3, &adc_raw));

        // Перерахунок у мілівольти з калібруванням
        ESP_ERROR_CHECK(adc_cali_raw_to_voltage(cali_handle, adc_raw, &voltage_calibrated));

        ESP_LOGI("ADC", "Raw: %d, Voltage_calibrated: %d mV", adc_raw, voltage_calibrated);

        // Маштабуємо до duty cycle (0-1023)
        int dutyCycle = adc_raw / 4;

        ledc_set_duty(LEDC_LOW_SPEED_MODE, PWM_CHANNEL, dutyCycle);
        ledc_update_duty(LEDC_LOW_SPEED_MODE, PWM_CHANNEL);

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
