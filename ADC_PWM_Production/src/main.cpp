#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "adc.h"
#include "timer.h"

const gpio_num_t GPIO_16 = GPIO_NUM_16;
const gpio_num_t GPIO_18 = GPIO_NUM_18;
constexpr uint8_t channel_count = 2;
constexpr uint16_t FREQ_5KHZ = 5000;
constexpr uint16_t FREQ_1KHZ = 1000;
constexpr uint16_t FREQ_50HZ = 50;

extern "C" void app_main(void)
{
    //-------------ADC1 Init + Calibration ---------------
    adc_cali_handle_t cali_handles[channel_count] = {NULL}; // Дескриптор калібрування
    adc_channel_t channels[channel_count] = {ADC_CHANNEL_3, ADC_CHANNEL_4};

    adc_context adc_ctx = {};
    adc_ctx.unit_id = ADC_UNIT_1;
    adc_ctx.bitwidth = ADC_BITWIDTH_12;
    adc_ctx.channels = channels;
    adc_ctx.channel_count = channel_count;
    adc_ctx.cali_handles = cali_handles;

    adc_init(&adc_ctx);

    //-------------Timer1 LED Init---------------
    pwm_context pwm1_ctx = {};
    pwm1_ctx.gpio_num = GPIO_16;
    pwm1_ctx.timer_num = LEDC_TIMER_0;
    pwm1_ctx.channel = LEDC_CHANNEL_0;
    pwm1_ctx.freq_hz = FREQ_5KHZ;
    pwm1_ctx.duty_resolution = LEDC_TIMER_12_BIT;
    pwm1_ctx.speed_mode = LEDC_LOW_SPEED_MODE;
    pwm1_ctx.initialized = false;

    pwm_init(&pwm1_ctx);

    //-------------Timer2 LED Init---------------
    pwm_context pwm2_ctx = {};
    pwm2_ctx.gpio_num = GPIO_18;
    pwm2_ctx.timer_num = LEDC_TIMER_1;
    pwm2_ctx.channel = LEDC_CHANNEL_1;
    pwm2_ctx.freq_hz = FREQ_50HZ;
    pwm2_ctx.duty_resolution = LEDC_TIMER_12_BIT;
    pwm2_ctx.speed_mode = LEDC_LOW_SPEED_MODE;
    pwm2_ctx.initialized = false;

    pwm_init(&pwm2_ctx);

    while (1)
    {
        int raw[channel_count] = {0};
        int voltage_mv[channel_count] = {0};
        pwm_context *pwm_ctx[channel_count] = {&pwm1_ctx, &pwm2_ctx};

        ESP_ERROR_CHECK(adc_read_all_voltage_mv(&adc_ctx, raw, voltage_mv));

        for (uint8_t i = 0; i < channel_count; i++)
        {
            ESP_ERROR_CHECK(pwm_set_duty(pwm_ctx[i], raw[i]));
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
