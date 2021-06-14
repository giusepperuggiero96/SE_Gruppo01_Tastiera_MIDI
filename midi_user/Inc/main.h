/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "stm32f4_discovery.h"

#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_midi.h"
#include "usbd_midi_if.h"
#include "math.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* User can use this section to tailor ADCx instance used and associated 
   resources */
/* Definition for ADCx clock resources */
#define ADCx                            ADC1
#define ADCx_CLK_ENABLE()               __HAL_RCC_ADC1_CLK_ENABLE()
#define ADCx_CHANNEL_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOA_CLK_ENABLE()

#define ADCx_FORCE_RESET()              __HAL_RCC_ADC_FORCE_RESET()
#define ADCx_RELEASE_RESET()            __HAL_RCC_ADC_RELEASE_RESET()

/* Definition for ADCx Channel Pin */
#define ADCx_CHANNEL_PIN                GPIO_PIN_4
#define ADCx_CHANNEL_GPIO_PORT          GPIOA

/* Definition for ADCx's Channel */
#define ADCx_CHANNEL                    ADC_CHANNEL_4

/* Definition for ADCx's NVIC */
#define ADCx_IRQn                       ADC_IRQn
#define ADCx_IRQHandler                 ADC_IRQHandler

/* Definition for TIMx clock resources */
#define TIMx                            TIM2
#define TIMx_CLK_ENABLE()               __HAL_RCC_TIM2_CLK_ENABLE()

#define TIMx_FORCE_RESET()              __HAL_RCC_TIM2_FORCE_RESET()
#define TIMx_RELEASE_RESET()            __HAL_RCC_TIM2_RELEASE_RESET()

#define TIMx TIM2

/* Exported functions ------------------------------------------------------- */
void SystemClock_Config(void);
//void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* tim);
#endif /* __MAIN_H */

/****************************END OF FILE****/
