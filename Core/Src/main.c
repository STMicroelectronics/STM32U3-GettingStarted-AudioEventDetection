/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "hsp_engine.h"

/* Private includes ----------------------------------------------------------*/
#include "ai_utility.h"
#include "gs_utils.h"    
#include "audio_bm.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/


/* Private define ------------------------------------------------------------*/


/* Private macro -------------------------------------------------------------*/


/* Private variables ---------------------------------------------------------*/
MDF_HandleTypeDef AdfHandle0;
MDF_FilterConfigTypeDef AdfFilterConfig0;
DMA_NodeTypeDef Node_GPDMA1_Channel0;
DMA_QListTypeDef List_GPDMA1_Channel0;
DMA_HandleTypeDef handle_GPDMA1_Channel0;

UART_HandleTypeDef hlpuart1;

static __IO uint32_t exit_loop = 0;


/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void GPIO_Init(void);
static void GPDMA1_Init(void);
static void ICACHE_Init(void);
static void ADF1_Init(void);
static void LPUART1_UART_Init(void);
static void exec_bm(void);
static void app_display_info_header(void);

/* Private user code ---------------------------------------------------------*/


/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Enable the SMPS regulator to improve power efficiency */
  if(HAL_PWREx_ConfigSupply(PWR_SMPS_SUPPLY) != HAL_OK)
  {
    Error_Handler();
  }

  /* Initialize all configured peripherals */
  GPIO_Init();
  GPDMA1_Init();
  ICACHE_Init();
  ADF1_Init();
  LPUART1_UART_Init();
  HSP_Engine_Init();
  app_display_info_header();
  PrintSystemSetting();

  exec_bm();
  
  /* Infinite loop */
  while (1)
  {
  }
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /* Enable Epod Booster */
  if (HAL_RCCEx_EpodBoosterClkConfig(RCC_EPODBOOSTER_SOURCE_MSIS, RCC_EPODBOOSTER_DIV1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_PWREx_EnableEpodBooster() != HAL_OK)
  {
    Error_Handler();
  }

  /* Configure the main internal regulator output voltage */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /* Set Flash latency before increasing MSIS */
  __HAL_FLASH_SET_LATENCY(FLASH_LATENCY_3);

  /* Initializes the CPU, AHB and APB buses clocks */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSIS|RCC_OSCILLATORTYPE_MSIK;
  RCC_OscInitStruct.MSISState = RCC_MSI_ON;
  RCC_OscInitStruct.MSISSource = RCC_MSI_RC0;
  RCC_OscInitStruct.MSISDiv = RCC_MSI_DIV1;
  RCC_OscInitStruct.MSIKState = RCC_MSI_ON;
  RCC_OscInitStruct.MSIKSource = RCC_MSI_RC1;
  RCC_OscInitStruct.MSIKDiv = RCC_MSI_DIV1;

  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /* Initializes the CPU, AHB and APB buses clocks */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_PCLK3;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSIS;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADF1 Initialization Function
  * @param None
  * @retval None
  */
static void ADF1_Init(void)
{
  /* AdfHandle0 structure initialization and HAL_MDF_Init function call */
  AdfHandle0.Instance = ADF1_Filter0;
  AdfHandle0.Init.CommonParam.ProcClockDivider = 1;
  AdfHandle0.Init.CommonParam.OutputClock.Activation = ENABLE;
  AdfHandle0.Init.CommonParam.OutputClock.Pins = MDF_OUTPUT_CLOCK_0;
  AdfHandle0.Init.CommonParam.OutputClock.Divider = 12;
  AdfHandle0.Init.CommonParam.OutputClock.Trigger.Activation = DISABLE;
  AdfHandle0.Init.SerialInterface.Activation = ENABLE;
  AdfHandle0.Init.SerialInterface.Mode = MDF_SITF_NORMAL_SPI_MODE;
  AdfHandle0.Init.SerialInterface.ClockSource = MDF_SITF_CCK0_SOURCE;
  AdfHandle0.Init.SerialInterface.Threshold = 31;
  AdfHandle0.Init.FilterBistream = MDF_BITSTREAM0_RISING;
  if (HAL_MDF_Init(&AdfHandle0) != HAL_OK)
  {
    Error_Handler();
  }

  /**
    AdfFilterConfig0 structure initialization
    WARNING : only structure is filled, no specific init function call for filter
  */
  AdfFilterConfig0.DataSource = MDF_DATA_SOURCE_BSMX;
  AdfFilterConfig0.Delay = 0;
  AdfFilterConfig0.CicMode = MDF_ONE_FILTER_SINC4;
  AdfFilterConfig0.DecimationRatio = 32;
  AdfFilterConfig0.Gain = 10;
  AdfFilterConfig0.ReshapeFilter.Activation = ENABLE;
  AdfFilterConfig0.ReshapeFilter.DecimationRatio = MDF_RSF_DECIMATION_RATIO_4;
  AdfFilterConfig0.HighPassFilter.Activation = ENABLE;
  AdfFilterConfig0.HighPassFilter.CutOffFrequency = MDF_HPF_CUTOFF_0_000625FPCM;
  AdfFilterConfig0.SoundActivity.Activation = ENABLE;
  AdfFilterConfig0.SoundActivity.Mode = MDF_SAD_SOUND_DETECTOR;
  AdfFilterConfig0.SoundActivity.FrameSize = MDF_SAD_64_PCM_SAMPLES;
  AdfFilterConfig0.SoundActivity.Hysteresis = ENABLE;
  AdfFilterConfig0.SoundActivity.SoundTriggerEvent = MDF_SAD_ENTER_EXIT_DETECT;
  AdfFilterConfig0.SoundActivity.DataMemoryTransfer = MDF_SAD_MEMORY_TRANSFER_IN_DETECT;
  AdfFilterConfig0.SoundActivity.MinNoiseLevel = 1000;
  AdfFilterConfig0.SoundActivity.HangoverWindow = MDF_SAD_HANGOVER_512_FRAMES;
  AdfFilterConfig0.SoundActivity.LearningFrames = MDF_SAD_LEARNING_2_FRAMES;
  AdfFilterConfig0.SoundActivity.SignalNoiseThreshold = MDF_SAD_SIGNAL_NOISE_6DB;
  AdfFilterConfig0.SoundActivity.SoundLevelInterrupt = DISABLE;
  AdfFilterConfig0.AcquisitionMode = MDF_MODE_ASYNC_CONT;
  AdfFilterConfig0.FifoThreshold = MDF_FIFO_THRESHOLD_NOT_EMPTY;
  AdfFilterConfig0.DiscardSamples = 0;

}

/**
  * @brief GPDMA1 Initialization Function
  * @param None
  * @retval None
  */
static void GPDMA1_Init(void)
{
  /* Peripheral clock enable */
  __HAL_RCC_GPDMA1_CLK_ENABLE();

  /* GPDMA1 interrupt Init */
    HAL_NVIC_SetPriority(GPDMA1_Channel0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(GPDMA1_Channel0_IRQn);
}

/**
  * @brief ICACHE Initialization Function
  * @param None
  * @retval None
  */
static void ICACHE_Init(void)
{
  /* Enable instruction cache in 1-way (direct mapped cache) */
  if (HAL_ICACHE_ConfigAssociativityMode(ICACHE_1WAY) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_ICACHE_Enable() != HAL_OK)
  {
    Error_Handler();
  }

}

/**
  * @brief LPUART1 Initialization Function
  * @param None
  * @retval None
  */
static void LPUART1_UART_Init(void)
{
  hlpuart1.Instance = LPUART1;
  hlpuart1.Init.BaudRate = USE_UART_BAUDRATE;
  hlpuart1.Init.WordLength = UART_WORDLENGTH_8B;
  hlpuart1.Init.StopBits = UART_STOPBITS_1;
  hlpuart1.Init.Parity = UART_PARITY_NONE;
  hlpuart1.Init.Mode = UART_MODE_TX_RX;
  hlpuart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  hlpuart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  hlpuart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  hlpuart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  hlpuart1.FifoMode = UART_FIFOMODE_DISABLE;
  if (HAL_UART_Init(&hlpuart1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&hlpuart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&hlpuart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&hlpuart1) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /* Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);

  /* Configure GPIO pin : USER_BUTTON_Pin */
  GPIO_InitStruct.Pin = USER_BUTTON_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(USER_BUTTON_GPIO_Port, &GPIO_InitStruct);

  /* Configure GPIO pin : LED_Pin */
  GPIO_InitStruct.Pin = LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI10_IRQn);

  HAL_NVIC_SetPriority(EXTI13_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI13_IRQn);
}

/**
 * @brief Executes main loop.
 */
static void exec_bm(void)
{
  unsigned char c;
  
  InitCpuStats();

#if (CTRL_X_CUBE_AI_SENSOR_TYPE == COM_TYPE_MIC)
  audio_init_bm();
#endif
  
  PrintHeader();
  UART_Start_Receive_IT(&hlpuart1, &c, 1);
  while (exit_loop == 0)
  {
    PrintMenu();
    
#if (CTRL_X_CUBE_AI_SENSOR_TYPE == COM_TYPE_MIC)
    audio_exec_bm();
#endif
  
  }
  
#if (CTRL_X_CUBE_AI_SENSOR_TYPE == COM_TYPE_MIC)
  audio_de_init_bm();
#endif  

  PrintFooter();
  PrintCpuStatsSummary();
}

/**
 * @brief Displays the application information header on the output device.
 *
 * This function is responsible for presenting the header information
 * related to the application, such as version, name, or other relevant
 * details, typically at the start of the program execution.
 */
static void app_display_info_header(void)
{
  printf("========================================\r\n");
  printf("STM32U3-2M-GettingStarted-AED v1.0.0 \r\n");
  printf("Build date & time: %s %s\r\n", __DATE__, __TIME__);
#if defined(__GNUC__)
  printf("Compiler: GCC %d.%d.%d\r\n", __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);
#elif defined(__ICCARM__)
  printf("Compiler: IAR EWARM %d.%d.%d\r\n", __VER__ / 1000000, (__VER__ / 1000) % 1000 ,__VER__ % 1000);
#else
  printf("Compiler: Unknown\r\n");
#endif
  printf("HAL: %lu.%lu.%lu\r\n", __STM32U3xx_HAL_VERSION_MAIN, __STM32U3xx_HAL_VERSION_SUB1, __STM32U3xx_HAL_VERSION_SUB2);
  printf("STEdgeAI Tools: %d.%d.%d\r\n", STAI_TOOLS_VERSION_MAJOR, STAI_TOOLS_VERSION_MINOR, STAI_TOOLS_VERSION_MICRO);
  printf("NN model: %s\r\n", STAI_NETWORK_ORIGIN_MODEL_NAME);
  printf("========================================\r\n");
}

/**
 * @brief Stdout redirection helpers for different toolchains.
 *
 * - IAR: Uses __write to transmit each byte over LPUART1.
 * - GCC/STM32CubeIDE: Uses _write to transmit the full buffer over LPUART1.
 */
#if defined(__ICCARM__) /* For IAR */
/* Redirects stdout to LPUART1. */
size_t __write(int Handle, const unsigned char *Buf, size_t Bufsize)
{
  int i;

  for(i=0; i<Bufsize; i++)
  {
    (void)HAL_UART_Transmit(&hlpuart1, (uint8_t *)&Buf[i], 1, 1000);
  }

  return Bufsize;
}
#elif defined(__GNUC__)   /* For GCC / STM32CubeIDE */
/* Redirects stdout to LPUART1. */
int _write(int fd, char * ptr, int len)
{
  HAL_UART_Transmit(&hlpuart1, (uint8_t*)ptr, len, HAL_MAX_DELAY);
  return len;
}
#endif


/**
 * @brief  Rx Transfer completed callback
 * @param  UartHandle: UART handle
 * @retval None
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle)
{
  exit_loop = 1;
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
}

#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
}
#endif /* USE_FULL_ASSERT */
