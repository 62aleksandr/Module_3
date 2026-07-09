#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "adc.h"
#include "timer.h"

constexpr int LED_PIN16 = 16;
constexpr int LED_PIN18 = 18;
constexpr int FREQ_5KHZ = 5000;
constexpr int FREQ_1KHZ = 1000;
constexpr uint8_t channel_count = 2;
;

extern "C" void app_main(void)
{
    //-------------ADC1 Init + Calibration ---------------
    adc_oneshot_unit_handle_t adc_handle;                   // Дескриптор апаратного модуля ADC 1
    adc_cali_handle_t cali_handles[channel_count] = {NULL}; // Дескриптор калібрування
    adc_channel_t channels[channel_count] = {ADC_CHANNEL_3, ADC_CHANNEL_4};

    adc_init_conf adc1_param = {
        .unit_id = ADC_UNIT_1,
        .bitwidth = ADC_BITWIDTH_12,
        .channels = channels,
        .channel_count = channel_count,
        .cali_handles = cali_handles};

    adc_init(&adc_handle, &adc1_param);

    //-------------Timer1 LED Init---------------
    timer_parameters timer1_param = {
        .gpio_num = LED_PIN16,
        .timer_num = LEDC_TIMER_0,
        .channel = LEDC_CHANNEL_0,
        .freq_hz = FREQ_5KHZ,
        .duty_resolution = LEDC_TIMER_10_BIT};

    pwm_init(&timer1_param);

    //-------------Timer2 LED Init---------------
    static timer_parameters timer2_param = {
        .gpio_num = LED_PIN18,
        .timer_num = LEDC_TIMER_0,
        .channel = LEDC_CHANNEL_1,
        .freq_hz = FREQ_1KHZ,
        .duty_resolution = LEDC_TIMER_10_BIT};

    pwm_init(&timer2_param);

    while (1)
    {
        int adc1_raw;
        int adc2_raw;
        int voltage_calibrated;

        // Зчитування сирого значення (0-4095)
        ESP_ERROR_CHECK(adc_oneshot_read(adc_handle, ADC_CHANNEL_3, &adc1_raw));
        ESP_ERROR_CHECK(adc_oneshot_read(adc_handle, ADC_CHANNEL_4, &adc2_raw));

        // Перерахунок у мілівольти з калібруванням
        // ESP_ERROR_CHECK(adc_cali_raw_to_voltage(cali1_handle, adc1_raw, &voltage_calibrated));

        // ESP_LOGI("ADC", "Raw: %d, Voltage_calibrated: %d mV", adc1_raw, voltage_calibrated);

        // Маштабуємо до duty cycle (0-1023)
        int dutyCycle1 = adc1_raw / 4;
        int dutyCycle2 = adc2_raw / 4;

        pwm_set_duty(dutyCycle1, timer1_param.channel);
        pwm_set_duty(dutyCycle2, timer2_param.channel);

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
