/*
 * comms_tasks.c
 *
 *  Created on: Jun 27, 2025
 *      Author: Excallibur
 *
 */

#include "main.h"
#include <string.h>


// === PRIVATE FUNCTION PROTOTYPES ===
static void send_telemetry_uart(const volatile TelemetryPacket_t *packet);
static void send_telemetry_can(const volatile TelemetryPacket_t *packet);

/**
 * @brief  Telemetri iletişim görev fonksiyonu
 * @param  param: RTOS tarafından kullanılan parametre (kullanılmıyor)
 *
 * Bu görev her 1 saniyede bir çalışır ve telemetry_packet
 * verisini hem UART hem de CAN üzerinden gönderir.
 * Veri erişimi için mutex kullanılarak thread-safe erişim sağlanır.
 */
void comms_task(void *param)
{
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(1000); // 1 saniye aralık

    for (;;)
    {
        vTaskDelayUntil(&xLastWakeTime, xFrequency);

        // Telemetri verisine güvenli erişim
        if (xSemaphoreTake(xTelemetryMutex, pdMS_TO_TICKS(10)) == pdTRUE)
        {
            telemetry_packet.timestamp = system_seconds;

            send_telemetry_uart(&telemetry_packet);
            send_telemetry_can(&telemetry_packet);

            xSemaphoreGive(xTelemetryMutex);
        }
    }
}

/**
 * @brief  UART üzerinden telemetri verisi gönderir
 * @param  packet: Gönderilecek veri paketi
 *
 * Bu fonksiyon telemetry_packet'i UART üzerinden gönderir.
 * Gönderilen pakete PORT_UART tanımı eklenir.
 */
static void send_telemetry_uart(const volatile TelemetryPacket_t *packet)
{
    TelemetryPacket_t uart_packet = *packet;  // Kopya oluştur
    uart_packet.port_id = PORT_UART;          // UART port bilgisi

    HAL_UART_Transmit(&huart2, (uint8_t *)&uart_packet, sizeof(TelemetryPacket_t), HAL_MAX_DELAY);
}

/**
 * @brief  Telemetri verisini CAN üzerinden parçalara bölerek gönderir
 * @param  packet: Gönderilecek veri paketi
 *
 * CAN protokolü maksimum 8 byte veri taşıyabildiği için
 * paket parçalanarak 8 byte’lık çerçeveler halinde gönderilir.
 * Her parça CAN Std ID 0x123 ile gönderilir.
 */
static void send_telemetry_can(const volatile TelemetryPacket_t *packet)
{
    TelemetryPacket_t can_packet = *packet;
    can_packet.port_id = PORT_CAN;

    uint8_t buffer[sizeof(TelemetryPacket_t)];
    memcpy(buffer, &can_packet, sizeof(can_packet));

    CAN_TxHeaderTypeDef TxHeader;
    uint32_t TxMailbox;
    uint8_t can_data[8];

    TxHeader.IDE = CAN_ID_STD;
    TxHeader.RTR = CAN_RTR_DATA;
    TxHeader.StdId = 0x123;

    uint16_t total_len = sizeof(TelemetryPacket_t);
    uint16_t sent_len = 0;

    while (sent_len < total_len)
    {
        uint8_t remain = total_len - sent_len;
        uint8_t send_now = remain >= 8 ? 8 : remain;

        memcpy(can_data, &buffer[sent_len], send_now);

        // Eğer 8’den az veri varsa, kalan byte'ları sıfırla
        if (send_now < 8)
        {
            memset(&can_data[send_now], 0, 8 - send_now);
        }

        TxHeader.DLC = send_now;

        if (HAL_CAN_AddTxMessage(&hcan1, &TxHeader, can_data, &TxMailbox) != HAL_OK)
        {
            Error_Handler();  // Gönderim hatası
        }

        sent_len += send_now;
        vTaskDelay(pdMS_TO_TICKS(2));  // CAN verileri arasında küçük bekleme
    }
}
