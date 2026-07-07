// https://www.getzenquery.com/tools/curve-fitting-tool/
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_random.h"

#include "filters.h"
#include "adc_driver.h"
#include "adc_calibration.h"

constexpr int VREF_mV = 3100; // 3.1V in mV
constexpr int ADC_RESOLUTION = 4095;

constexpr int SIZE = 4; // Розмір вікна ковзного середнього

constexpr float ALPHA = 0.6f; // коефіцієнт згладжування

int getVoltage(int adcValue)
{
    return adcValue * VREF_mV / ADC_RESOLUTION;
}

int getPolynomial(int adcValue)
{
    int voltage = getVoltage(adcValue);

    if (voltage <= 1)
        return (int)(-voltage * voltage + 12231 * voltage - 382) / 10000;
    else if (voltage <= 2)
        return (int)(-2 * voltage * voltage + 14226 * voltage - 956886) / 10000;
    else
        return (int)(-voltage * voltage + 12231 * voltage - 382) / 10000;
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

    adc_cali_handle_t cali_handle = NULL; // Дескриптор калібрування
    adc_calibration_init(&cali_handle);

    while (1)
    {
        int adc_raw;
        int voltage_polynomial;
        int voltage_calibrated;
        float error;

        // Зчитування сирого значення (0-4095)
        ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, ADC_CHANNEL_3, &adc_raw));

        // Перерахунок у мілівольти без калібрування
        int voltage = getVoltage(adc_raw);

        voltage_polynomial = getPolynomial(adc_raw);

        // Перерахунок у мілівольти з калібруванням
        ESP_ERROR_CHECK(adc_cali_raw_to_voltage(cali_handle, adc_raw, &voltage_calibrated));

        // ESP_LOGI("ADC", "Raw: %d, Voltage: %d mV, Voltage_calibrated: %d mV", adc_raw, voltage, voltage_calibrated);

        error = (voltage_calibrated != 0) ? 100.0f * abs(voltage_calibrated - voltage_polynomial) / voltage_calibrated : 0.0f;

        // Обчислення відносної похибки

        printf("%d %d %.2f\n", voltage_calibrated, voltage_polynomial, error);
        // printf("%d %d\n", voltage, voltage_calibrated);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
