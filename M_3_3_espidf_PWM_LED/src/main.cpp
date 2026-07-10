#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "adc.h"
#include "timer.h"

constexpr int LED_PIN16 = 16;
constexpr int LED_PIN18 = 18;
constexpr int FREQ_5KHZ = 5000;
constexpr int FREQ_1KHZ = 1000;
constexpr int FREQ_50HZ = 50;
constexpr uint8_t channel_count = 2;
;

extern "C" void app_main(void)
{
    //-------------ADC1 Init + Calibration ---------------
    adc_oneshot_unit_handle_t adc_handle;                   // Дескриптор апаратного модуля ADC 1
    adc_cali_handle_t cali_handles[channel_count] = {NULL}; // Дескриптор калібрування
    adc_channel_t channels[channel_count] = {ADC_CHANNEL_3, ADC_CHANNEL_4};

    adc_context adc_ctx = {};
    adc_ctx.unit_id = ADC_UNIT_1;
    adc_ctx.bitwidth = ADC_BITWIDTH_12;
    adc_ctx.channels = channels;
    adc_ctx.channel_count = channel_count;
    adc_ctx.cali_handles = cali_handles;

    adc_init(&adc_handle, &adc_ctx);

    //-------------Timer1 LED Init---------------
    pwm_context pwm1_ctx = {
        .gpio_num = LED_PIN16,
        .timer_num = LEDC_TIMER_0,
        .channel = LEDC_CHANNEL_0,
        .freq_hz = FREQ_5KHZ,
        .duty_resolution = LEDC_TIMER_10_BIT};

    pwm_init(&pwm1_ctx);

    //-------------Timer2 LED Init---------------
    pwm_context pwm2_ctx = {
        .gpio_num = LED_PIN18,
        .timer_num = LEDC_TIMER_0,
        .channel = LEDC_CHANNEL_1,
        .freq_hz = FREQ_50HZ,
        .duty_resolution = LEDC_TIMER_12_BIT};

    pwm_init(&pwm2_ctx);

    while (1)
    {
        int raw[channel_count] = {0};
        pwm_context *pwm_ctx[channel_count] = {&pwm1_ctx, &pwm2_ctx};

        for (int i = 0; i < channel_count; i++)
        {
            // Зчитування сирого значення
            ESP_ERROR_CHECK(adc_oneshot_read(adc_handle, channels[i], &raw[i]));

            pwm_set_duty(pwm_ctx[i], raw[i]);
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
