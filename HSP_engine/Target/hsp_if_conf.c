/**
  **********************************************************************************************
  * @file           : Target/hsp_if_conf.c
  * @version        : v1_0_Cube
  * @brief          : This file implements the interface between the STM32 HSP MW and the HSP IP
  ***********************************************************************************************
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

#include "hsp_if_conf.h"

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/


HSP_HandleTypeDef hhsp1;
void Error_Handler(void);

/*
 * -- Insert your variables declaration here --
 */

/*
 * -- Insert your external function declaration here --
 */


/* MSP Init */

void HAL_HSP_MspInit(HSP_HandleTypeDef* hspHandle)
{
  if(hspHandle->Instance==HSP1)
  {
    /* Enable Peripheral clock */
    __HAL_RCC_HSP1_CLK_ENABLE();
  }
}

void HAL_HSP_MspDeInit(HSP_HandleTypeDef* hspHandle)
{
  if(hspHandle->Instance==HSP1)
  {
    /* Peripheral clock disable */
    __HAL_RCC_HSP1_CLK_DISABLE();
  }
}

/*******************************************************************************
                       HAL Driver Interface (HSP Engine Library --> HSP)
*******************************************************************************/
/**
  * @brief  Initializes the low level portion of the driver.
  * @param  hmw  HSP Engine handle
  * @retval hsp_if_status_t status
  */
hsp_if_status_t HSP_Engine_IF_Init(hsp_engine_context_t *hmw)
{
  /* Init HSP Driver. */
  hhsp1.global_state = HAL_HSP_STATE_RESET;

  hhsp1.Instance = HSP1;
  if (HAL_HSP_Init(&hhsp1) != HAL_OK)
  {
    return HSP_IF_ERROR;
  }

  /* Link the driver to the MW. */
  hmw->hdriver = &hhsp1;

  return HSP_IF_OK;
}
/**
  * @}
  */

/**
  * @}
  */

