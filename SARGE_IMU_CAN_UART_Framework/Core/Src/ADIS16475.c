#include "ADIS16475.h"
#include "main.h"

// ADIS16475 SPI handler için global pointer
static SPI_HandleTypeDef *adis_spi = NULL;

/**
 * @brief  ADIS16475 sensörü başlatır
 * @param  hspi: SPI handle (örnek: &hspi1)
 */
void ADIS16475_Init(SPI_HandleTypeDef *hspi)
{
    adis_spi = hspi;
}

/**
 * @brief  ADIS16475 register'ından 16-bit veri okur
 * @param  reg_addr: okunacak register adresi
 * @retval 16-bit register verisi
 */
uint16_t ADIS16475_ReadRegister(uint8_t reg_addr)
{
    uint8_t tx_buf[2];
    uint8_t rx_buf[2];
    uint16_t received = 0;

    // İlk SPI aktarımı: register adresi gönderiliyor (okuma için MSB 0 olmalı)
    tx_buf[0] = reg_addr & 0x7F;
    tx_buf[1] = 0x00;

    HAL_GPIO_WritePin(ADIS16475_CS_GPIO_Port, ADIS16475_CS_Pin, GPIO_PIN_RESET);
    if (HAL_SPI_Transmit(adis_spi, tx_buf, 2, HAL_MAX_DELAY) != HAL_OK)
    {
        spi_timeout_flag = 1;  // SPI haberleşme hatası
    }
    HAL_GPIO_WritePin(ADIS16475_CS_GPIO_Port, ADIS16475_CS_Pin, GPIO_PIN_SET);

    Delay_Us(15);  // Min. 16 tSCLK delay (datasheet gereği)

    // İkinci SPI aktarımı: veriyi oku
    tx_buf[0] = 0x00;
    tx_buf[1] = 0x00;

    HAL_GPIO_WritePin(ADIS16475_CS_GPIO_Port, ADIS16475_CS_Pin, GPIO_PIN_RESET);
    if (HAL_SPI_TransmitReceive(adis_spi, tx_buf, rx_buf, 2, HAL_MAX_DELAY) != HAL_OK)
    {
        spi_timeout_flag = 1;  // SPI haberleşme hatası
    }
    HAL_GPIO_WritePin(ADIS16475_CS_GPIO_Port, ADIS16475_CS_Pin, GPIO_PIN_SET);

    // 16-bit veriyi birleştir
    received = ((uint16_t)rx_buf[0] << 8) | rx_buf[1];

    return received;
}

/**
 * @brief  Gyro verilerini okur (X, Y, Z)
 * @param  gx: X-axis (°/s)
 * @param  gy: Y-axis (°/s)
 * @param  gz: Z-axis (°/s)
 */
void ADIS16475_ReadGyro(float *gx, float *gy, float *gz)
{
    int16_t raw_x = (int16_t)ADIS16475_ReadRegister(ADIS16475_X_GYRO_OUT);
    int16_t raw_y = (int16_t)ADIS16475_ReadRegister(ADIS16475_Y_GYRO_OUT);
    int16_t raw_z = (int16_t)ADIS16475_ReadRegister(ADIS16475_Z_GYRO_OUT);

    *gx = raw_x * 0.00625f;
    *gy = raw_y * 0.00625f;
    *gz = raw_z * 0.00625f;
}

/**
 * @brief  İvme verilerini okur (X, Y, Z)
 * @param  ax: X-axis (g)
 * @param  ay: Y-axis (g)
 * @param  az: Z-axis (g)
 */
void ADIS16475_ReadAccel(float *ax, float *ay, float *az)
{
    int16_t raw_x = (int16_t)ADIS16475_ReadRegister(ADIS16475_X_ACCEL_OUT);
    int16_t raw_y = (int16_t)ADIS16475_ReadRegister(ADIS16475_Y_ACCEL_OUT);
    int16_t raw_z = (int16_t)ADIS16475_ReadRegister(ADIS16475_Z_ACCEL_OUT);

    *ax = raw_x * 0.00025f;  // Sensör çözünürlüğü: 0.25 mg per LSB
    *ay = raw_y * 0.00025f;
    *az = raw_z * 0.00025f;
}

/**
 * @brief  Mikro-saniye cinsinden gecikme fonksiyonu (NOP tabanlı)
 * @param  us: gecikme süresi (mikrosaniye)
 * @note   Zaman hassasiyeti düşük, kaba bir bekleme sağlar.
 */
void Delay_Us(uint16_t us)
{
    for (uint16_t i = 0; i < us * 10; i++)
    {
        __NOP();  // No operation (CPU’yu boşta beklet)
    }
}
