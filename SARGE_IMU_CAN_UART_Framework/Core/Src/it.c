
#include "main.h"
#include "stm32f4xx_it.h"

extern TIM_HandleTypeDef htim6;


void TIM6_DAC_IRQHandler(void)
{

  HAL_TIM_IRQHandler(&htim6);

}


