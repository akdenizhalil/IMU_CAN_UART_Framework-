

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_tim.h"

TIM_HandleTypeDef        htim6;



HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority)
{

  HAL_StatusTypeDef     status;



  	 // 1. TIM6 saat sinyalini etkinleştir
    __HAL_RCC_TIM6_CLK_ENABLE();

    // 2️. Timer yapılandırması başlat
    htim6.Instance = TIM6;

    //3.  Prescaler = 41999:
    // Timer clock = 42 MHz (APB1 timer clock)
    // 3. 1 MHz timer clock (42 MHz / (Prescaler + 1) = 1 MHz → 1 us)
    htim6.Init.Prescaler = 41;  // 42 MHz / 42 = 1 MHz

    // 4️. Period = 999:
    // Timer her 1000 sayımda kesme üretir → 1000 / 1000MHz = 1 milisaniye
    htim6.Init.Period = 999;

    // 5️. Sayaç yukarı doğru saysın
    htim6.Init.CounterMode = TIM_COUNTERMODE_UP;

    // 6️. Otomatik yeniden yükleme preload'u devre dışı bırak
    htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

  status = HAL_TIM_Base_Init(&htim6);
  if (status == HAL_OK)
  {
    /* Start the TIM time Base generation in interrupt mode */
    status = HAL_TIM_Base_Start_IT(&htim6);
    if (status == HAL_OK)
    {
    /* Enable the TIM6 global Interrupt */
        HAL_NVIC_EnableIRQ(TIM6_DAC_IRQn);
      /* Configure the SysTick IRQ priority */
      if (TickPriority < (1UL << __NVIC_PRIO_BITS))
      {
        /* Configure the TIM IRQ priority */
        HAL_NVIC_SetPriority(TIM6_DAC_IRQn, TickPriority, 0U);
        uwTickPrio = TickPriority;
      }
      else
      {
        status = HAL_ERROR;
      }
    }
  }


  return status;
}

