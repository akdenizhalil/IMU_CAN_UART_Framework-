/*
 * error_monitor_task.c
 *
 *  Created on: Jun 27, 2025
 *      Author: Excallibur
 */


#include "main.h"



/**
 * @brief  Hata izleme görev fonksiyonu
 * @param  param: RTOS tarafından kullanılan parametre (kullanılmıyor)
 *
 * Bu görev, diğer görevlerden gelen `xTaskNotifyGive()` sinyallerini bekler.
 * Eğer SPI veya sıcaklık ile ilgili hata bayrakları set edilmişse,
 * telemetry_packet.error_flags alanına işlenir ve flag'ler sıfırlanır.
 */
void error_monitor_task(void *param)
{
    for (;;)
    {
        // Bildirim gelene kadar bekle (bloklanır)
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        // Telemetri paketine güvenli erişim
        if (xSemaphoreTake(xTelemetryMutex, portMAX_DELAY) == pdTRUE)
        {
            // Sıcaklık hatası oluşmuşsa, ilgili flag'i set et
            if (temp_overunder_flag == 1)
            {
                telemetry_packet.error_flags |= ERROR_FLAG_TEMP_OVER_UNDER;
                temp_overunder_flag = 0;
            }

            // SPI haberleşme hatası oluşmuşsa, ilgili flag'i set et
            if (spi_timeout_flag == 1)
            {
                telemetry_packet.error_flags |= ERROR_FLAG_SPI_TIMEOUT;
                spi_timeout_flag = 0;
            }

            xSemaphoreGive(xTelemetryMutex);
        }
    }
}
