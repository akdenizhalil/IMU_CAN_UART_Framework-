/*
 * sensor_task.c
 *
 *  Created on: Jun 27, 2025
 *      Author: Excallibur
 *
 *  Açıklama:
 *  Bu FreeRTOS görevi, ADIS16475 IMU sensöründen
 *  gyro ve ivme verilerini okur ve telemetry_packet yapısına yazar.
 *  SPI iletişiminde hata olursa, error_monitor_task'a bildirim gönderilir.
 */

#include "main.h"
#include "ADIS16475.h"
#include "telemetry_packet.h"



/**
 * @brief  IMU sensör verisini okuyan görev fonksiyonu
 *
 * Her 50 ms’de (20 Hz) çalışır.
 * - ADIS16475 sensöründen gyro ve ivme değerlerini SPI ile alır
 * - Bu verileri global telemetry_packet içine yazar
 * - SPI hatası oluşursa error monitor görevine bildirim gönderir
 */
void sensor_task(void *params)
{
    float gx, gy, gz;
    float ax, ay, az;

    for (;;)
    {
        ADIS16475_ReadGyro(&gx, &gy, &gz);
        ADIS16475_ReadAccel(&ax, &ay, &az);

        // SPI iletişim hatası varsa error monitor görevine bildir
        if (spi_timeout_flag == 1)
        {
            xTaskNotifyGive(handle_error_monitor_task);
            continue;
        }

        // Telemetry verisine güvenli erişim
        if (xSemaphoreTake(xTelemetryMutex, pdMS_TO_TICKS(10)) == pdTRUE)
        {
            telemetry_packet.gyro[0] = gx;
            telemetry_packet.gyro[1] = gy;
            telemetry_packet.gyro[2] = gz;

            telemetry_packet.accel[0] = ax;
            telemetry_packet.accel[1] = ay;
            telemetry_packet.accel[2] = az;

            xSemaphoreGive(xTelemetryMutex);
        }

        vTaskDelay(pdMS_TO_TICKS(50)); // 20 Hz
    }
}
