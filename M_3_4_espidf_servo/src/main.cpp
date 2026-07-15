/*Драйвер реалізовано з використанням патерну контексту та дескриптора (Context/Handle pattern), де структура контексту зберігає стан модуля, а дескриптор забезпечує доступ до цього об’єкта
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "adc.h"
#include "timer.h"

#include "esp_log.h"

static const char *TAG = "SERVO";
constexpr gpio_num_t GPIO_16 = GPIO_NUM_16;
static constexpr uint8_t channel_count = 1;
static constexpr uint16_t FREQ_50HZ = 50;
static constexpr uint16_t PERIOD_MS = 1000 / FREQ_50HZ;
static constexpr uint16_t angle_180 = 180;
static constexpr adc_bitwidth_t adc_bit = ADC_BITWIDTH_12;
static constexpr ledc_timer_bit_t ledc_timer_bit = LEDC_TIMER_12_BIT;

extern "C" void app_main(void)
{
    //-------------ADC1 Init + Calibration ---------------
    adc_cali_handle_t cali_handles[channel_count] = {NULL}; // Дескриптор калібрування
    adc_channel_t channels[channel_count] = {ADC_CHANNEL_3};

    static adc_context adc_ctx = {};
    adc_ctx.unit_id = ADC_UNIT_1;
    adc_ctx.bitwidth = adc_bit;
    adc_ctx.channels = channels;
    adc_ctx.channel_count = channel_count;
    adc_ctx.cali_handles = cali_handles;

    adc_init(&adc_ctx);

    //-------------Timer1 LED Init---------------
    pwm_context pwm1_ctx = {};
    pwm1_ctx.gpio_num = GPIO_16;
    pwm1_ctx.timer_num = LEDC_TIMER_0;
    pwm1_ctx.channel = LEDC_CHANNEL_0;
    pwm1_ctx.freq_hz = FREQ_50HZ;
    pwm1_ctx.duty_resolution = ledc_timer_bit;
    pwm1_ctx.speed_mode = LEDC_LOW_SPEED_MODE;
    pwm1_ctx.initialized = false;

    pwm_init(&pwm1_ctx);

    //-------------Level LOG---------------
    esp_log_level_set(TAG, ESP_LOG_INFO);

    while (1)
    {
        int raw[channel_count] = {0};
        int voltage_mv[channel_count] = {0};
        pwm_context *pwm_ctx[channel_count] = {&pwm1_ctx};

        ESP_ERROR_CHECK(adc_read_all_voltage_mv(&adc_ctx, raw, voltage_mv));

        for (uint8_t i = 0; i < channel_count; i++)
        {
            // Код АЦП (0...4095) -> кут (0...180°)
            uint16_t angle = (raw[i] * angle_180) / (1 << adc_bit);
            // кут (0...180°) -> код PWM
            uint16_t pwm_servo = ((1 << ledc_timer_bit) + (1 << ledc_timer_bit) * angle / angle_180) / (PERIOD_MS);

            ESP_ERROR_CHECK(pwm_set_duty(pwm_ctx[i], pwm_servo));

            ESP_LOGI(TAG, "Angle=%u deg, Pulse=%lu us", angle,
                     pwm_servo * PERIOD_MS * 1000 / (1 << ledc_timer_bit));
        }

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
