#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "esp_log.h"
#include "esp_err.h"

constexpr float VREF = 3.3f;
constexpr float ADC_RESOLUTION = 4095.0f;

int getVoltage(int adcValue)
{
    return static_cast<int>((static_cast<float>(adcValue) / ADC_RESOLUTION) * VREF * 1000.0f);
}

extern "C" void app_main(void)
{
    // 1. Конфігурація модуля (Unit)
    adc_oneshot_unit_handle_t adc1_handle; // Дескриптор апаратного модуля ADC 1

    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = ADC_UNIT_1,              // Використання ADC 1
        .clk_src = ADC_RTC_CLK_SRC_DEFAULT, // стандартне джерело CLK
        .ulp_mode = ADC_ULP_MODE_DISABLE,   // вимкнення режиму ULP
    };

    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc1_handle)); // Створення дескриптора ADC 1

    // 2. Конфігурація каналу
    adc_oneshot_chan_cfg_t config = {
        .atten = ADC_ATTEN_DB_12,         // Діапазон до ~3.1В
        .bitwidth = ADC_BITWIDTH_DEFAULT, // Зазвичай 12 біт
    };

    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, ADC_CHANNEL_3, &config)); // Налаштування каналу 3 ADC 1

    // 3. (Опціонально) Налаштування калібрування для отримання мВ
    adc_cali_handle_t cali_handle = NULL; // Дескриптор калібрування

    adc_cali_curve_fitting_config_t cali_config = {
        .unit_id = ADC_UNIT_1,
        .chan = ADC_CHANNEL_3,
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    }; // Налаштування калібрування для каналу 3 ADC 1

    ESP_ERROR_CHECK(adc_cali_create_scheme_curve_fitting(&cali_config, &cali_handle)); // Створення дескриптора калібрування

    while (1)
    {
        int adc_raw;
        int voltage;
        int voltage_calibrated;

        // Зчитування сирого значення (0-4095)
        ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, ADC_CHANNEL_3, &adc_raw));

        voltage = getVoltage(adc_raw);

        // Перерахунок у мілівольти (якщо калібрування успішне)
        ESP_ERROR_CHECK(adc_cali_raw_to_voltage(cali_handle, adc_raw, &voltage_calibrated));

        ESP_LOGI("ADC", "Raw: %d, Voltage: %d mV, Voltage_calibrated: %d mV", adc_raw, voltage, voltage_calibrated);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}