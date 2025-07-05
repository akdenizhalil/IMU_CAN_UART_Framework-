#ifndef __ADIS16475_H__
#define __ADIS16475_H__

/**
 * @file ADIS16475.h
 * @brief ADIS16475 IMU sensörü için register tanımları ve API fonksiyon prototipleri.
 *
 * Bu dosya, SPI üzerinden ADIS16475 ile haberleşmek için gerekli olan
 * register adreslerini ve temel sensör fonksiyonlarını içerir.
 */

#include "main.h"

// Gyro register adresleri (16-bit)
#define ADIS16475_X_GYRO_OUT      0x06
#define ADIS16475_Y_GYRO_OUT      0x0A
#define ADIS16475_Z_GYRO_OUT      0x0E

// Accelerometer register adresleri (16-bit)
#define ADIS16475_X_ACCEL_OUT     0x12
#define ADIS16475_Y_ACCEL_OUT     0x16
#define ADIS16475_Z_ACCEL_OUT     0x1A

// Sıcaklık ve sistem bilgisi
#define ADIS16475_TEMP_OUT        0x04
#define ADIS16475_DIAG_STAT       0x02
#define ADIS16475_FILTER_SIZE     0x5E
#define ADIS16475_NULL_CFG        0x60
#define ADIS16475_GLOB_CMD        0x68

// Cihaz bilgileri
#define ADIS16475_PRODUCT_ID      0x7E
#define ADIS16475_SERIAL_NUM      0x58
#define ADIS16475_FIRM_REV        0x6C


// === API Fonksiyon Prototipleri ===
void     ADIS16475_Init(SPI_HandleTypeDef *hspi);
uint16_t ADIS16475_ReadRegister(uint8_t reg_addr);
void     ADIS16475_ReadGyro(float *gx, float *gy, float *gz);
void     ADIS16475_ReadAccel(float *ax, float *ay, float *az);
void     Delay_Us(uint16_t us);

#endif
