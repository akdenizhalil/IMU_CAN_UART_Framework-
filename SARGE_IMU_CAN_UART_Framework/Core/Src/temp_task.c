/*
 * temp_task.c
 *
 *  Created on: Jun 27, 2025
 *      Author: Excallibur
 *
 *  Açıklama:
 *  Bu FreeRTOS görevi, dahili sıcaklık sensöründen ADC ile
 *  sıcaklık verisini okur ve telemetry_packet’e yazar.
 *  Aşırı sıcaklık/soğukluk durumunda error_monitor_task'a haber verir.
 */

#include "main.h"


/**
 * @brief  Dahili sıcaklık verisini okuyan görev fonksiyonu
 *
 * Her 200 ms’de çalışır.
 * - ADC’den sıcaklık değerini hesaplar
 * - Değer 0–70°C dışında ise error monitor görevine bildirir
 * - Aksi durumda telemetry_packet.temp_degC içine yazar
 */
void temp_task()
{
    uint32_t adc_val = 0;
    float vsense = 0.0f;
    float temp = 0.0f;

    for (;;)
    {
        HAL_ADC_Start(&temp_sensor_adc);
        if (HAL_ADC_PollForConversion(&temp_sensor_adc, 10) == HAL_OK)
        {
            adc_val = HAL_ADC_GetValue(&temp_sensor_adc);
            vsense = (adc_val / 4095.0f) * 3.3f;

            // ST'nin dahili sıcaklık sensör formülü
            temp = ((vsense - 0.76f) / 0.0025f) + 25.0f;

            // Aşırı sıcaklık/soğukluk tespiti
            if (temp > 70.0f || temp < 0.0f)
            {
                temp_overunder_flag = 1;
                xTaskNotifyGive(handle_error_monitor_task);
                continue;
            }

            // Telemetry'ye sıcaklık verisini güvenli şekilde yaz
            if (xSemaphoreTake(xTelemetryMutex, portMAX_DELAY) == pdTRUE)
            {
                telemetry_packet.temp_degC = temp;
                xSemaphoreGive(xTelemetryMutex);
            }
        }

        vTaskDelay(pdMS_TO_TICKS(200)); // 5 Hz
    }
}
