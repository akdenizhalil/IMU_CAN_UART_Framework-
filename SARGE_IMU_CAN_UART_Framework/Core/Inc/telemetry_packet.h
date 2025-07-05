#ifndef INC_TELEMETRY_PACKET_H_
#define INC_TELEMETRY_PACKET_H_

/**
 * @file telemetry_packet.h
 * @brief Telemetri verisi yapısı ve zaman yönetimi fonksiyonları
 *
 * Bu dosya, UART/CAN ile gönderilecek telemetry verisinin yapısını tanımlar.
 * Ayrıca zaman bilgisini sağlayan yardımcı fonksiyonları içerir.
 */

#include <stdint.h>
#include "main.h"

// === Paket Tanımları ===
#define TELEMETRY_HEADER  0xDD
#define PORT_UART         0x01
#define PORT_CAN          0x02

// === Telemetri Yapısı ===

typedef struct __attribute__((packed)) {
    uint8_t  header;         // Başlık (0xDD)
    uint32_t timestamp;      // Zaman (saniye)
    float    temp_degC;      // Sıcaklık
    float    gyro[3];        // Gyro verileri (x, y, z)
    float    accel[3];       // İvme verileri (x, y, z)
    uint8_t  error_flags;    // Hata bayrakları (bit mask)
    uint8_t  port_id;        // UART: 0x01, CAN: 0x02
} TelemetryPacket_t;

// === Global Telemetry Paketi ===

extern volatile TelemetryPacket_t telemetry_packet;

// === Fonksiyon Prototipleri ===

void     telemetry_init(void);
uint32_t get_timestamp_s(void);

#endif /* INC_TELEMETRY_PACKET_H_ */
