#ifndef __MAIN_H
#define __MAIN_H

/**
 * @file main.h
 * @brief Projeye ait temel tanımlar, pin ayarları, global değişkenler ve görev prototipleri.
 */

#include "stm32f4xx_hal.h"
#include <string.h>
#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"

#include "telemetry_packet.h"
#include "ADIS16475.h"

// === Global Donanım Handler’ları ===
extern SPI_HandleTypeDef hspi1;
extern CAN_HandleTypeDef hcan1;
extern UART_HandleTypeDef huart2;
extern ADC_HandleTypeDef temp_sensor_adc;

// === Global RTOS Nesneleri ===
extern SemaphoreHandle_t xTelemetryMutex;

// === Global Bayraklar ===
extern volatile uint8_t spi_timeout_flag;
extern volatile uint8_t temp_overunder_flag;
extern volatile uint32_t system_seconds;

// === Görev Handle'ları ===
extern TaskHandle_t handle_error_monitor_task;

// === Pin Tanımları ===
#define SPI1_SCK_Pin              GPIO_PIN_5
#define SPI1_SCK_GPIO_Port        GPIOA
#define SPI1_MISO_Pin             GPIO_PIN_6
#define SPI1_MISO_GPIO_Port       GPIOA
#define SPI1_MOSI_Pin             GPIO_PIN_7
#define SPI1_MOSI_GPIO_Port       GPIOA
#define ADIS16475_CS_GPIO_Port    GPIOA
#define ADIS16475_CS_Pin          GPIO_PIN_4

// === Hata Bayrakları ===
#define ERROR_FLAG_SPI_TIMEOUT      (1 << 0)
#define ERROR_FLAG_TEMP_OVER_UNDER  (1 << 1)

// === Görev Prototipleri ===
void sensor_task();
void temp_task();
void comms_task();
void error_monitor_task();

void Error_Handler(void);

#endif /* __MAIN_H */
