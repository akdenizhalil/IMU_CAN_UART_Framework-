/**
 * @file main.c
 * @brief FreeRTOS tabanlı telemetry sistemi ana dosyası.
 *
 * Bu projede STM32F4 mikrodenetleyicisi kullanılarak
 * ADIS16475 IMU sensöründen veri okunur,
 * sıcaklık ölçülür ve bu veriler UART/CAN ile iletilir.
 *
 * RTOS görevleri, donanım başlatmaları ve sistem zamanı burada tanımlanır.
 */

#include "main.h"

// === Özel fonksiyon prototipleri ===

void SystemClock_Config(void);
static void TEMP_ADC_Init(void);
static void UART2_Init(void);
static void SPI1_Init(void);
static void CAN1_Init(void);
static void ADS16475_CSGPIO_Init(void);

// === Donanım tanımları ===
CAN_HandleTypeDef hcan1;
UART_HandleTypeDef huart2;
ADC_HandleTypeDef temp_sensor_adc;
SPI_HandleTypeDef hspi1;

// === Sistem global flag'leri ===
volatile uint8_t spi_timeout_flag = 0;
volatile uint8_t temp_overunder_flag = 0;
volatile uint32_t system_seconds = 0;

// === RTOS görev handle'ları ===
xTaskHandle handle_sensor_task;
xTaskHandle handle_temp_task;
xTaskHandle handle_comms_task;
xTaskHandle handle_error_monitor_task;

// === Ortak veri için mutex ===
SemaphoreHandle_t xTelemetryMutex;


int main(void)
{
    // Sistem ve saat konfigürasyonu
    HAL_Init();
    SystemClock_Config();

    // Donanım başlatmaları
    CAN1_Init();
    UART2_Init();
    TEMP_ADC_Init();
    SPI1_Init();
    ADS16475_CSGPIO_Init();

    // IMU başlat
    ADIS16475_Init(&hspi1);

    // Telemetry yapısı sıfırlanır
    telemetry_init();

    // === Görevler oluşturuluyor ===
    BaseType_t status;

    status = xTaskCreate(sensor_task, "sensor_task", 250, NULL, 2, &handle_sensor_task);
    configASSERT(status == pdPASS);

    status = xTaskCreate(temp_task, "temp_task", 250, NULL, 2, &handle_temp_task);
    configASSERT(status == pdPASS);

    status = xTaskCreate(comms_task, "comms_task", 250, NULL, 2, &handle_comms_task);
    configASSERT(status == pdPASS);

    status = xTaskCreate(error_monitor_task, "error_monitor_task", 250, NULL, 2, &handle_error_monitor_task);
    configASSERT(status == pdPASS);

    // === Mutex oluşturuluyor ===
    xTelemetryMutex = xSemaphoreCreateMutex();
    if (xTelemetryMutex == NULL)
    {
        Error_Handler();
    }


    vTaskStartScheduler();


    for (;;);
}

/**
 * @brief  STM32 sistem saat konfigürasyonu
 */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLM = 8;
    RCC_OscInitStruct.PLL.PLLN = 168;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
    RCC_OscInitStruct.PLL.PLLQ = 7;

    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                  RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
    {
        Error_Handler();
    }
}

// === Periferik Başlatma Fonksiyonları (CAN, SPI, UART, ADC) ===

static void CAN1_Init(void)
{
    hcan1.Instance = CAN1;
    hcan1.Init.Prescaler = 5;
    hcan1.Init.Mode = CAN_MODE_NORMAL;
    hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
    hcan1.Init.TimeSeg1 = CAN_BS1_13TQ;
    hcan1.Init.TimeSeg2 = CAN_BS2_2TQ;
    hcan1.Init.TimeTriggeredMode = DISABLE;
    hcan1.Init.AutoBusOff = DISABLE;
    hcan1.Init.AutoWakeUp = DISABLE;
    hcan1.Init.AutoRetransmission = ENABLE;
    hcan1.Init.ReceiveFifoLocked = DISABLE;
    hcan1.Init.TransmitFifoPriority = DISABLE;

    if (HAL_CAN_Init(&hcan1) != HAL_OK)
    {
        Error_Handler();
    }
}

static void SPI1_Init(void)
{
    hspi1.Instance = SPI1;
    hspi1.Init.Mode = SPI_MODE_MASTER;
    hspi1.Init.Direction = SPI_DIRECTION_2LINES;
    hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
    hspi1.Init.CLKPolarity = SPI_POLARITY_HIGH;
    hspi1.Init.CLKPhase = SPI_PHASE_2EDGE;
    hspi1.Init.NSS = SPI_NSS_SOFT;
    hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;
    hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
    hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    hspi1.Init.CRCPolynomial = 10;

    if (HAL_SPI_Init(&hspi1) != HAL_OK)
    {
        Error_Handler();
    }
}

static void UART2_Init(void)
{
    huart2.Instance = USART2;
    huart2.Init.BaudRate = 115200;
    huart2.Init.WordLength = UART_WORDLENGTH_8B;
    huart2.Init.StopBits = UART_STOPBITS_1;
    huart2.Init.Parity = UART_PARITY_NONE;
    huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart2.Init.Mode = UART_MODE_TX_RX;

    if (HAL_UART_Init(&huart2) != HAL_OK)
    {
        Error_Handler();
    }
}

static void TEMP_ADC_Init(void)
{
    ADC_ChannelConfTypeDef sConfig = {0};

    __HAL_RCC_ADC1_CLK_ENABLE();

    temp_sensor_adc.Instance = ADC1;
    temp_sensor_adc.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
    temp_sensor_adc.Init.Resolution = ADC_RESOLUTION_12B;
    temp_sensor_adc.Init.ScanConvMode = DISABLE;
    temp_sensor_adc.Init.ContinuousConvMode = ENABLE;
    temp_sensor_adc.Init.DiscontinuousConvMode = DISABLE;
    temp_sensor_adc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    temp_sensor_adc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    temp_sensor_adc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    temp_sensor_adc.Init.NbrOfConversion = 1;
    temp_sensor_adc.Init.DMAContinuousRequests = DISABLE;
    temp_sensor_adc.Init.EOCSelection = ADC_EOC_SINGLE_CONV;

    if (HAL_ADC_Init(&temp_sensor_adc) != HAL_OK)
    {
        Error_Handler();
    }

    sConfig.Channel = ADC_CHANNEL_TEMPSENSOR;
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;

    if (HAL_ADC_ConfigChannel(&temp_sensor_adc, &sConfig) != HAL_OK)
    {
        Error_Handler();
    }
}

static void ADS16475_CSGPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitStruct.Pin = ADIS16475_CS_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

    HAL_GPIO_Init(ADIS16475_CS_GPIO_Port, &GPIO_InitStruct);
    HAL_GPIO_WritePin(ADIS16475_CS_GPIO_Port, ADIS16475_CS_Pin, GPIO_PIN_SET);
}

/**
 * @brief FreeRTOS SysTick kullandığı için HAL_Tick arttırmak için TIM6 kullanılmıştır.
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM6)
    {
        HAL_IncTick();

        static uint16_t tick_counter = 0;
        tick_counter++;

        if (tick_counter >= 1000)
        {
            system_seconds++;
            tick_counter = 0;
        }
    }
}

/**
 * @brief  Kritik hata durumunda sistem sonsuz döngüye girer
 */
void Error_Handler(void)
{
    while (1)
    {

    }
}
