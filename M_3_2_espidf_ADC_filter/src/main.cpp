#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
// #include "esp_adc/adc_oneshot.h"
// #include "esp_adc/adc_cali.h"
// #include "esp_adc/adc_cali_scheme.h"
// #include "esp_log.h"
// #include "esp_err.h"
#include "esp_random.h"

#include "filters.h"
#include "adc_driver.h"
#include "adc_calibration.h"

constexpr float VREF = 3.1f;
constexpr float ADC_RESOLUTION = 4095.0f;

constexpr int SIZE = 4; // Розмір вікна ковзного середнього

constexpr float ALPHA = 0.6f; // коефіцієнт згладжування

int getVoltage(int adcValue)
{
    return static_cast<int>((static_cast<float>(adcValue) / ADC_RESOLUTION) * VREF * 1000.0f);
}

extern "C" void app_main(void)
{
    //-------------MA and EMA Init---------------//
    static MovingAvg MAFilter;
    static ExpMovingAverage EMAFilter;

    initMovingAverage(&MAFilter, SIZE);
    initEMAFilter(&EMAFilter, ALPHA);

    //-------------ADC1 Init---------------//
    adc_oneshot_unit_handle_t adc1_handle; // Дескриптор апаратного модуля ADC 1
    adc_init(&adc1_handle);

    //-------------ADC1 Calibration Init---------------//

    adc_cali_handle_t cali_handle = NULL; // Дескриптор калібрування

    adc_calibration_init(&cali_handle);

    while (1)
    {
        int adc_raw;
        int voltage;
        int voltage_calibrated;
        int voltage_calibrated_noise;

        // Зчитування сирого значення (0-4095)
        ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, ADC_CHANNEL_3, &adc_raw));

        // Перерахунок у мілівольти без калібрування
        voltage = getVoltage(adc_raw);

        // Перерахунок у мілівольти з калібруванням
        ESP_ERROR_CHECK(adc_cali_raw_to_voltage(cali_handle, adc_raw, &voltage_calibrated));

        ESP_LOGI("ADC", "Raw: %d, Voltage: %d mV, Voltage_calibrated: %d mV", adc_raw, voltage, voltage_calibrated);

        // Сигнал + шуму

        voltage_calibrated_noise = voltage_calibrated + static_cast<int>(esp_random() % 201) - 100;

        // Сигнал після фільтрації ковзним середнім MA
        int filterMA = movingAverage(&MAFilter, voltage_calibrated_noise);

        // Сигнал після фільтрації експоненціальним середнім EMA
        int filterEMA = exponentialMovingAverage(&EMAFilter, voltage_calibrated_noise);

        ESP_LOGI("ADC", "Voltage_calibrated_noise: %d mV, Filter MA: %d mV, Filter EMA: %d mV", voltage_calibrated_noise, filterMA, filterEMA);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
