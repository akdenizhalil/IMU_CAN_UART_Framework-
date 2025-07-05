/*
 * telemetry_packet.c
 *
 *  Created on: Jun 27, 2025
 *      Author: Excallibur
 *
 *  Açıklama:
 *  Bu dosya, telemetry_packet adlı global yapıyı yönetmek için kullanılır.
 *  Yapı sistem zamanı, sıcaklık, sensör verileri ve hata bayraklarını içerir.
 *  Ayrıca zaman bilgisini sağlayan yardımcı fonksiyonlar da buradadır.
 */

#include "telemetry_packet.h"
#include "main.h"

// === Global telemetry paketi ===
// Tüm görevler bu yapıya mutex ile erişerek veri günceller
volatile TelemetryPacket_t telemetry_packet;

/**
 * @brief  telemetry_packet yapısını varsayılan değerlere sıfırlar
 *
 * Bu fonksiyon sistem başlatıldığında çağrılır.
 * Tüm alanlar 0 ile temizlenir, sabit başlık ve port bilgisi eklenir.
 */
void telemetry_init(void)
{
    telemetry_packet.header = TELEMETRY_HEADER;
    telemetry_packet.timestamp = 0;
    telemetry_packet.temp_degC = 0.0f;

    for (int i = 0; i < 3; i++) {
        telemetry_packet.gyro[i] = 0.0f;
        telemetry_packet.accel[i] = 0.0f;
    }

    telemetry_packet.error_flags = 0;
    telemetry_packet.port_id = 0;
}


