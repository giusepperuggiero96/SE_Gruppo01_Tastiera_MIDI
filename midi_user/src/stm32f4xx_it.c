/**
  ******************************************************************************
  * @file    Demonstrations/Src/stm32f4xx_it.c 
  * @author  MCD Application Team
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx_it.h"
   
/** @addtogroup STM32F4xx_HAL_Applications
  * @{
  */

/** @addtogroup Demo
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern PCD_HandleTypeDef hpcd;
extern    TIM_HandleTypeDef    TimHandle_5;
extern ADC_HandleTypeDef AdcHandle;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief   This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
  HAL_IncTick();
  
  /* Call user callback */
  HAL_SYSTICK_IRQHandler();
  
}

/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) , for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f407xx.s).                                               */
/******************************************************************************/

/**
  * @brief  This function handles External line 0 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI0_IRQHandler(void)
{
  HAL_NVIC_DisableIRQ((IRQn_Type)(KEY_BUTTON_EXTI_IRQn));
  HAL_GPIO_EXTI_IRQHandler(KEY_BUTTON_PIN);
}

void EXTI9_5_IRQHandler(void)
{
  HAL_NVIC_DisableIRQ((IRQn_Type)(EXTI15_10_IRQn));
  HAL_NVIC_DisableIRQ((IRQn_Type)(EXTI9_5_IRQn));
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_7);
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_8);
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_9);
}

void EXTI15_10_IRQHandler(void)
{
  HAL_NVIC_DisableIRQ((IRQn_Type)(EXTI9_5_IRQn));
  HAL_NVIC_DisableIRQ((IRQn_Type)(EXTI15_10_IRQn));

  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_10);
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_11);
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_12);
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_14);
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_15);
}

/**
  * @brief  OTG_FS_IRQHandler
  *          This function handles USB-On-The-Go FS global interrupt request.
  *          requests.
  * @param  None
  * @retval None
  */
void OTG_FS_IRQHandler(void)
{  
   HAL_PCD_IRQHandler(&hpcd);
}

void OTG_FS_WKUP_IRQHandler(void)
{
  if ((&hpcd)->Init.low_power_enable)
  {
    /* Reset SLEEPDEEP bit of Cortex System Control Register */
    SCB->SCR &=
      (uint32_t) ~
      ((uint32_t) (SCB_SCR_SLEEPDEEP_Msk | SCB_SCR_SLEEPONEXIT_Msk));

    /* Configures system clock after wake-up from STOP: enable HSE, PLL and
     * select PLL as system clock source (HSE and PLL are disabled in STOP
     * mode) */
    SystemClock_Config();

    /* ungate PHY clock */
    __HAL_PCD_UNGATE_PHYCLOCK((&hpcd));
  }
  /* Clear EXTI pending Bit */
  __HAL_USB_OTG_FS_WAKEUP_EXTI_CLEAR_FLAG();
}


void ADCx_IRQHandler(void)
{
  HAL_ADC_IRQHandler(&AdcHandle);
}

void TIM5_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&TimHandle_5);
    HAL_NVIC_EnableIRQ((IRQn_Type)(KEY_BUTTON_EXTI_IRQn));
    HAL_NVIC_EnableIRQ((IRQn_Type)(EXTI9_5_IRQn));
    HAL_NVIC_EnableIRQ((IRQn_Type)(EXTI15_10_IRQn));
}


/**
* @brief  This function handles DMA interrupt request.
* @param  None
* @retval None
*/


void ADCx_DMA_IRQHandler(void)
{
  HAL_DMA_IRQHandler(AdcHandle.DMA_Handle);
}

/**
  * @}
  */ 

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/