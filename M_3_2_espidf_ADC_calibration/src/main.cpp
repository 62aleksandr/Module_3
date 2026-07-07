#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_random.h"
#include "filters.h"
#include "adc.h"

constexpr int VREF_mV = 3100;        // Напруга опорного джерела в мілівольтах
constexpr int ADC_RESOLUTION = 4095; // Роздільна здатність ADC

constexpr int SIZE = 4; // Розмір вікна ковзного середнього

constexpr float ALPHA = 0.6f; // коефіцієнт згладжування

static int getVoltage(int adcValue)
{
    return (adcValue * VREF_mV) / ADC_RESOLUTION;
}

int getPolynomial(int adcValue)
{
    int voltage = getVoltage(adcValue);

    if (voltage <= 1)
        return (int)(-voltage * voltage + 12231 * voltage - 382) / 10000;
    else if (voltage <= 1.5)
        return (int)(3 * voltage * voltage + 7637 * voltage + 1597776) / 10000;
    else if (voltage <= 2.0)
        return (int)(68 * voltage * voltage - 110940 * voltage + 72443561) / 10000;

    else
        return (int)(-voltage * voltage + 12231 * voltage - 382) / 10000;
}

extern "C" void app_main(void)
{
    //-------------MA and EMA Init---------------
    MovingAvg MAFilter;
    static ExpMovingAverage EMAFilter;

    initMovingAverage(&MAFilter, SIZE);
    initEMAFilter(&EMAFilter, ALPHA);

    //-------------ADC1 Init---------------
    adc_oneshot_unit_handle_t adc1_handle; // Дескриптор апаратного модуля ADC 1
    adc_init(&adc1_handle);

    //-------------ADC1 Calibration Init---------------
    adc_cali_handle_t cali_handle = NULL; // Дескриптор калібрування
    adc_calibration_init(&cali_handle);

    while (1)
    {
        int adc_raw;
        int voltage;
        int voltage_polynomial;
        int voltage_calibrated;
        int voltage_calibrated_noise;

        // Зчитування сирого значення (0-4095)
        ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, ADC_CHANNEL_3, &adc_raw));

        // Перерахунок у мілівольти без калібрування
        voltage = getVoltage(adc_raw);
        voltage_polynomial = getPolynomial(adc_raw);

        // Перерахунок у мілівольти з калібруванням
        ESP_ERROR_CHECK(adc_cali_raw_to_voltage(cali_handle, adc_raw, &voltage_calibrated));

        ESP_LOGI("ADC", "Raw: %d, Voltage: %d mV, Voltage_calibrated: %d mV", adc_raw, voltage, voltage_calibrated);

        // Сигнал + шум
        voltage_calibrated_noise = voltage_calibrated + static_cast<int>(esp_random() % 201) - 100;

        // Сигнал після фільтрації ковзним середнім MA
        int filterMA = movingAverage(&MAFilter, voltage_calibrated_noise);

        // Сигнал після фільтрації експоненціальним середнім EMA
        int filterEMA = exponentialMovingAverage(&EMAFilter, voltage_calibrated_noise);

        ESP_LOGI("ADC", "Voltage_calibrated_noise: %d mV, Filter MA: %d mV, Filter EMA: %d mV", voltage_calibrated_noise, filterMA, filterEMA);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
