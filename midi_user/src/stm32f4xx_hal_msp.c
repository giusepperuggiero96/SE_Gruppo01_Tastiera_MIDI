/* Includes ------------------------------------------------------------------*/
#include "main.h"

/** @addtogroup STM32F4xx_HAL_Applications
  * @{
  */

/** @defgroup HAL_MSP
  * @brief HAL MSP module.
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup HAL_MSP_Private_Functions
  * @{
  */

void HAL_MspInit(void)
{
  /* Configure LED3, LED4, LED5 and LED6 */
  BSP_LED_Init(LED3);
  BSP_LED_Init(LED4);
  BSP_LED_Init(LED5);
  BSP_LED_Init(LED6);

  /* Configure USER Button */
  // BSP_PB_Init(BUTTON_KEY, BUTTON_MODE_EXTI);
  //we don't use BSP_BP_Init since it just activates rising edge interrupts
  /*start setup button */
  GPIO_InitTypeDef GPIO_InitStruct;
  BUTTONx_GPIO_CLK_ENABLE(BUTTON_KEY);
  GPIO_InitStruct.Pin = KEY_BUTTON_PIN;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  HAL_GPIO_Init(KEY_BUTTON_GPIO_PORT, &GPIO_InitStruct);
  HAL_NVIC_SetPriority((IRQn_Type)(KEY_BUTTON_EXTI_IRQn), 0x0e, 0);
  HAL_NVIC_EnableIRQ((IRQn_Type)(KEY_BUTTON_EXTI_IRQn));
  /* end setup button */

  // Configure breadboard buttons
  __HAL_RCC_GPIOE_CLK_ENABLE();
  GPIO_InitStruct.Pin = GPIO_PIN_7;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_8;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_9;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_10;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_11;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_12;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_14;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_15;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  HAL_NVIC_SetPriority((IRQn_Type)(EXTI9_5_IRQn), 0x0e, 0);
  HAL_NVIC_EnableIRQ((IRQn_Type)(EXTI9_5_IRQn));

  HAL_NVIC_SetPriority((IRQn_Type)(EXTI15_10_IRQn), 0x0e, 0);
  HAL_NVIC_EnableIRQ((IRQn_Type)(EXTI15_10_IRQn));

}

void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
{
  GPIO_InitTypeDef          GPIO_InitStruct;

  /*##-1- Enable peripherals and GPIO Clocks */
  /* Enable GPIO clock*/
  ADCx_CHANNEL_GPIO_CLK_ENABLE();
  /* ADC3 Periph clock enable */
  ADCx_CLK_ENABLE();

  /*##-2- Configure peripheral GPIO */
  /* ADC Channel GPIO pin configuration */
  GPIO_InitStruct.Pin = ADCx_CHANNEL_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(ADCx_CHANNEL_GPIO_PORT, &GPIO_InitStruct);

  /*##-3- Configure the NVIC */
  HAL_NVIC_SetPriority(ADCx_IRQn, 15, 0);
  HAL_NVIC_EnableIRQ(ADCx_IRQn);
}

void HAL_ADC_MspDeInit(ADC_HandleTypeDef *hadc)
{
  ADCx_FORCE_RESET();
  ADCx_RELEASE_RESET();

  /*##-2- Disable peripherals and GPIO Clocks ################################*/
  /* De-initialize the ADC Channel GPIO pin */
  HAL_GPIO_DeInit(ADCx_CHANNEL_GPIO_PORT, ADCx_CHANNEL_PIN);
}

