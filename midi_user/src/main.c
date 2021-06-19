/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private define ------------------------------------------------------------*/
#define NOTES             12
#define SONG_LENGTH       132
#define MIDI_PACKET_SIZE  4

// Nei miei sogni è Fur Elise
// I miei sogni sono realtà :D
static uint8_t song[SONG_LENGTH] = {

  MIDI_NOTE_E(6), MIDI_NOTE_Eb(6), MIDI_NOTE_E(6), MIDI_NOTE_Eb(6), MIDI_NOTE_E(6), MIDI_NOTE_B(5), MIDI_NOTE_D(6), MIDI_NOTE_C(6), MIDI_NOTE_A(5),
  MIDI_NOTE_C(5), MIDI_NOTE_E(5), MIDI_NOTE_A(5), MIDI_NOTE_B(5),
  MIDI_NOTE_E(5), MIDI_NOTE_Gs(5), MIDI_NOTE_B(5), MIDI_NOTE_C(6),
  MIDI_NOTE_E(5), MIDI_NOTE_E(6), MIDI_NOTE_Eb(6), MIDI_NOTE_E(6), MIDI_NOTE_Eb(6), MIDI_NOTE_E(6), MIDI_NOTE_B(5), MIDI_NOTE_D(6), MIDI_NOTE_C(6), MIDI_NOTE_A(5),
  MIDI_NOTE_C(5), MIDI_NOTE_E(5), MIDI_NOTE_A(5), MIDI_NOTE_B(5),
  MIDI_NOTE_E(5), MIDI_NOTE_C(6), MIDI_NOTE_B(5), MIDI_NOTE_A(5),

  MIDI_NOTE_E(6), MIDI_NOTE_Eb(6), MIDI_NOTE_E(6), MIDI_NOTE_Eb(6), MIDI_NOTE_E(6), MIDI_NOTE_B(5), MIDI_NOTE_D(6), MIDI_NOTE_C(6), MIDI_NOTE_A(5),
  MIDI_NOTE_C(5), MIDI_NOTE_E(5), MIDI_NOTE_A(5), MIDI_NOTE_B(5),
  MIDI_NOTE_E(5), MIDI_NOTE_Gs(5), MIDI_NOTE_B(5), MIDI_NOTE_C(6),
  MIDI_NOTE_E(5), MIDI_NOTE_E(6), MIDI_NOTE_Eb(6), MIDI_NOTE_E(6), MIDI_NOTE_Eb(6), MIDI_NOTE_E(6), MIDI_NOTE_B(5), MIDI_NOTE_D(6), MIDI_NOTE_C(6), MIDI_NOTE_A(5),
  MIDI_NOTE_C(5), MIDI_NOTE_E(5), MIDI_NOTE_A(5), MIDI_NOTE_B(5),
  MIDI_NOTE_E(5), MIDI_NOTE_C(6), MIDI_NOTE_B(5), MIDI_NOTE_A(5),

  MIDI_NOTE_B(5), MIDI_NOTE_C(6), MIDI_NOTE_D(6), MIDI_NOTE_E(6),
  MIDI_NOTE_G(5), MIDI_NOTE_F(6), MIDI_NOTE_E(6), MIDI_NOTE_D(6),
  MIDI_NOTE_F(5), MIDI_NOTE_E(6), MIDI_NOTE_D(6), MIDI_NOTE_C(6),
  MIDI_NOTE_E(5), MIDI_NOTE_D(6), MIDI_NOTE_C(6), MIDI_NOTE_B(5),
  MIDI_NOTE_E(5), MIDI_NOTE_E(6),

  MIDI_NOTE_E(5), MIDI_NOTE_E(6), MIDI_NOTE_Eb(6), MIDI_NOTE_E(6), MIDI_NOTE_Eb(6), MIDI_NOTE_E(6), MIDI_NOTE_Eb(6),MIDI_NOTE_E(6), MIDI_NOTE_Eb(6), MIDI_NOTE_E(6), MIDI_NOTE_Eb(6), MIDI_NOTE_E(6), MIDI_NOTE_Eb(6),MIDI_NOTE_E(6), MIDI_NOTE_B(5), MIDI_NOTE_D(6), MIDI_NOTE_C(6), MIDI_NOTE_A(5),
  MIDI_NOTE_C(5), MIDI_NOTE_E(5), MIDI_NOTE_A(5), MIDI_NOTE_B(5),
  MIDI_NOTE_E(5), MIDI_NOTE_Gs(5), MIDI_NOTE_B(5), MIDI_NOTE_C(6),
  MIDI_NOTE_E(5), MIDI_NOTE_E(6), MIDI_NOTE_Eb(6), MIDI_NOTE_E(6), MIDI_NOTE_Eb(6), MIDI_NOTE_E(6), MIDI_NOTE_B(5), MIDI_NOTE_D(6), MIDI_NOTE_C(6), MIDI_NOTE_A(5),
  MIDI_NOTE_C(5), MIDI_NOTE_E(5), MIDI_NOTE_A(5), MIDI_NOTE_B(5),
  MIDI_NOTE_E(5), MIDI_NOTE_C(6), MIDI_NOTE_B(5), MIDI_NOTE_A(5),

};

static uint8_t keyboard[7] = {
  MIDI_NOTE_C(5), MIDI_NOTE_D(5), MIDI_NOTE_E(5), MIDI_NOTE_F(5), MIDI_NOTE_G(5), MIDI_NOTE_A(5), MIDI_NOTE_B(5)
};
static uint8_t indexes[16] = {
  0, 1, 2, 3, 4, 5, 6, 1, 2, 3, 4, 5, 6, 1, 2
};

/* Private function prototypes -----------------------------------------------*/
static uint32_t USBConfig(void);
static void TIM2_config(void);
static void TIM5_config(void);
static void Error_Handler(void);
static void ADC_Config(void);
/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef    htim;
TIM_HandleTypeDef    TimHandle_5;

//uint32_t uwTimerPeriod  = 2*5040000/3; // 20 ms
uint32_t uwTimerPeriod  = 5040000;    // 30 ms
//uint32_t uwTimerPeriod  = 3*5040000;    // 90 ms


/* ADC handler declaration */
ADC_HandleTypeDef    AdcHandle;

/* Variable used to get converted value */
__IO uint16_t uhADCxConvertedValue = 15;
static uint16_t converted_old = 127;

uint32_t timestart;

__IO int index = 0;
__IO int active[16] = {0};
__IO uint8_t note = 0;
__IO uint8_t channel = 4;
__IO uint8_t velocity = 127;
__IO uint16_t pitchBend = 32;

__IO uint16_t button;

/* Variables used for USB */
USBD_HandleTypeDef  hUSBDDevice;

/* Viariables used for midi packets */
USBD_MIDI_NoteTypedef   midi_note;
USBD_MIDI_PitchTypedef  midi_pitch;

int main(void)
{
  HAL_Init();

  /* Configure the system clock to 168 MHz */
  SystemClock_Config();

  // USB configuration
  USBConfig();

  //config tim2 to one-pulse in order to debounce button
  TIM2_config();

  // config tim5
  TIM5_config();

  /*Configure the ADC peripheral */
  ADC_Config();

  if (HAL_ADC_Start_IT(&AdcHandle) != HAL_OK)
  {
    /* Start Conversation Error */
    Error_Handler();
  }

  /*TIM counter enable */
  if (HAL_TIM_Base_Start(&htim) != HAL_OK)
  {
    /* Counter Enable Error */
    Error_Handler();
  }
  /* Infinite loop */
  while (1)
  {
  }

    /* Disconnect the USB device */
  USBD_Stop(&hUSBDDevice);
  USBD_DeInit(&hUSBDDevice);
  HAL_ADC_Stop_IT(&AdcHandle);
  HAL_ADC_DeInit(&AdcHandle);
  HAL_TIM_Base_Stop_IT(&TimHandle_5);
  HAL_TIM_Base_Stop(&htim);
  HAL_TIM_Base_DeInit(&htim);
  HAL_TIM_Base_DeInit(&TimHandle_5);
  HAL_DeInit();
}

// USB configuration
static uint32_t USBConfig(void)
{
  /* Init Device Library */
  USBD_Init(&hUSBDDevice, &HID_Desc, 0);

  /* Add Supported Class */
  USBD_RegisterClass(&hUSBDDevice, USBD_MIDI_CLASS);

  /* Start Device Process */
  USBD_Start(&hUSBDDevice);

  return 0;
}

static void TIM2_config(void)
{
 TIM_MasterConfigTypeDef sMasterConfig;

  /* Time Base configuration */
  htim.Instance = TIMx;

  htim.Init.Period            = 65000;
  htim.Init.Prescaler         = 0;
  htim.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;
  htim.Init.CounterMode       = TIM_COUNTERMODE_UP;
  htim.Init.RepetitionCounter = 0x0;

  __HAL_RCC_TIM2_CLK_ENABLE();

  if (HAL_TIM_Base_Init(&htim) != HAL_OK)
  {
    /* Timer initialization Error */
    Error_Handler();
  }

  /* Timer TRGO selection */
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;

  if (HAL_TIMEx_MasterConfigSynchronization(&htim, &sMasterConfig) != HAL_OK)
  {
    /* Timer TRGO selection Error */
    Error_Handler();
  }
}

static void TIM5_config(void)
{
  TimHandle_5.Instance = TIM5;

  TimHandle_5.Init.Period            = uwTimerPeriod;
  TimHandle_5.Init.RepetitionCounter = 0;
  TimHandle_5.Init.Prescaler         = 0;
  TimHandle_5.Init.ClockDivision     = 0;
  TimHandle_5.Init.CounterMode       = TIM_COUNTERMODE_UP;
  TimHandle_5.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

  __HAL_RCC_TIM5_CLK_ENABLE();

  HAL_NVIC_EnableIRQ(TIM5_IRQn);
  HAL_NVIC_SetPriority(TIM5_IRQn, 0, 0);

  if (HAL_TIM_Base_Init(&TimHandle_5) != HAL_OK)
    /* Initialization Error */
    Error_Handler();

}

void ADC_Config(void)
{
  ADC_ChannelConfTypeDef sConfig;

  /* ADC Initialization */
  AdcHandle.Instance          = ADCx;
  
  AdcHandle.Init.ClockPrescaler        = ADC_CLOCKPRESCALER_PCLK_DIV4;
  AdcHandle.Init.Resolution            = ADC_RESOLUTION8b;
  AdcHandle.Init.DataAlign             = ADC_DATAALIGN_RIGHT;
  AdcHandle.Init.ScanConvMode          = DISABLE;                       /* Sequencer disabled (ADC conversion on only 1 channel: channel set on rank 1) */
  AdcHandle.Init.EOCSelection          = ADC_EOC_SEQ_CONV;
  AdcHandle.Init.ContinuousConvMode    = DISABLE;                       /* Continuous mode disabled to have only 1 conversion at each conversion trig */
  AdcHandle.Init.DiscontinuousConvMode = DISABLE;                       /* Parameter discarded because sequencer is disabled */
  AdcHandle.Init.ExternalTrigConv      = ADC_EXTERNALTRIGCONV_T2_TRGO;       /* Conversion start trigged at each external event */
  AdcHandle.Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_RISING;
  AdcHandle.Init.DMAContinuousRequests = ENABLE;


  if (HAL_ADC_Init(&AdcHandle) != HAL_OK)
  {
    /* ADC initialization Error */
    Error_Handler();
  }

  /* Configure ADC regular channel */
  sConfig.Channel      = ADCx_CHANNEL;
  sConfig.Rank         = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_28CYCLES;


  if (HAL_ADC_ConfigChannel(&AdcHandle, &sConfig) != HAL_OK)
  {
    /* Channel Configuration Error */
    Error_Handler();
  }
}

// Elapsed timer period callback
// Notes get sent here to prevent button bouncing
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* tim)
{
  //timestart = HAL_GetTick();
  // Note-on message sent on rising edge
  if(HAL_GPIO_ReadPin((button == KEY_BUTTON_PIN) ? KEY_BUTTON_GPIO_PORT : GPIOE, button)){
    if(!active[(int)log2(button)]) {
      active[(int)log2(button)] = 1;
      BSP_LED_On(LED3);

      midi_note.note = (button == KEY_BUTTON_PIN) ? song[index] : keyboard[indexes[(int)log2(button)]];
      midi_note.velocity = velocity;
      midi_note.channel = channel;
      midi_note.cmd = NOTE_ON;

      HAL_NVIC_DisableIRQ(ADCx_IRQn);
      if(USBD_MIDI_SendNote(&hUSBDDevice, &midi_note) != USBD_OK)
        Error_Handler();
      HAL_NVIC_EnableIRQ(ADCx_IRQn);
    }
  // Note-off message sent on falling edge
  } else if(!HAL_GPIO_ReadPin((button == KEY_BUTTON_PIN) ? KEY_BUTTON_GPIO_PORT : GPIOE, button)){
    if(active[(int)log2(button)]) {
      active[(int)log2(button)] = 0;
      BSP_LED_Off(LED3);

      midi_note.note = (button == KEY_BUTTON_PIN) ? song[index] : keyboard[indexes[(int)log2(button)]];
      midi_note.velocity = velocity;
      midi_note.channel = channel;
      midi_note.cmd = NOTE_OFF;

      HAL_NVIC_DisableIRQ(ADCx_IRQn);
      if(USBD_MIDI_SendNote(&hUSBDDevice, &midi_note) != USBD_OK)
        Error_Handler();
      HAL_NVIC_EnableIRQ(ADCx_IRQn);
      // index incremented mod(LENGHT) to avoid overflow
      if (button == KEY_BUTTON_PIN)
        index = (index+1) % SONG_LENGTH;
    }
  } 
  HAL_TIM_Base_Stop(tim);
}

 USBD_StatusTypeDef MIDI_DataRx(uint8_t *msg, uint16_t length){
    static uint32_t timeBack;
    timeBack = HAL_GetTick();
    static uint32_t diff;
    diff = timeBack - timestart;
    printf("%ld\n", diff);
	  return USBD_OK;
}
// Systick callback
void HAL_SYSTICK_Callback(void)
{
  //pass
}

/* GPIO callback */
// Starts the timer in interrupt mode and checks for errors
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  timestart = HAL_GetTick();
  button = GPIO_Pin;
  if (HAL_TIM_Base_Start_IT(&TimHandle_5) != HAL_OK)
    Error_Handler();

}

/* Conversion complete callback in non blocking mode*/
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* AdcHandle)
{
   uhADCxConvertedValue = HAL_ADC_GetValue(AdcHandle);
  /* Turn LED4 on: Transfer process is correct */
  if( abs (uhADCxConvertedValue - converted_old)  > 3)
   {
      converted_old = uhADCxConvertedValue;
      midi_pitch.pitchBend = converted_old;
      midi_pitch.channel = channel;
      USBD_MIDI_SendPitch(&hUSBDDevice, &midi_pitch);
   }
  BSP_LED_On(LED5);
}

// Error handler (Turns LED on)
static void Error_Handler(void)
{
  /* Turn LED4 on */
  BSP_LED_On(LED4);
  while(1)
  {
  }
}

void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;

  /* Enable Power Control clock */
  __HAL_RCC_PWR_CLK_ENABLE();

  /* The voltage scaling allows optimizing the power consumption when the device is
     clocked below the maximum system frequency, to update the voltage scaling value
     regarding system frequency refer to product datasheet.  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);

  /* STM32F405x/407x/415x/417x Revision Z devices: prefetch is supported  */
  if (HAL_GetREVID() == 0x1001)
  {
    /* Enable the Flash prefetch */
    __HAL_FLASH_PREFETCH_BUFFER_ENABLE();
  }
}


#ifdef  USE_FULL_ASSERT

void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}

#endif


/*****************************END OF FILE****/
