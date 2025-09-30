/**
******************************************************************************
* @file            : hsp_engine.c
* @version         : v1_0_Cube
* @brief           : This file implements the HSP_Engine
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

#include "main.h"
#include "hsp_engine.h"
#include "app_hsp_bram_alloc.h"

/* Private includes ----------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/


/* Private variables ---------------------------------------------------------*/


/* HSP Middleware handle */
/* HSP core handle declaration */
hsp_engine_context_t hmw;

/* Private function prototypes -----------------------------------------------*/

/* Application functions prototypes *******/
extern void Error_Handler(void);

/* Private user code ---------------------------------------------------------*/

/**
 * @brief Middleware HSP Initialization Function
 * @param None
 * @retval None
 */
void HSP_Engine_Init(void)
{
  if (HSP_Engine_IF_Init(&hmw) != HSP_IF_OK)
  {
    Error_Handler();
  }

  if (HSP_CORE_Init(&hmw, hmw.hdriver) != HSP_CORE_OK)
  {
    Error_Handler();
  }
}

/**
 * @brief Middleware HSP Process Function
 * @param None
 * @retval None
 */
void HSP_Engine_Process(void)
{

}

/**
 * @brief Middleware HSP get context Function
 * @param None
 * @retval None
 */
hsp_engine_context_t* HSP_Engine_GetContext(void)
{

  return &hmw;
}
