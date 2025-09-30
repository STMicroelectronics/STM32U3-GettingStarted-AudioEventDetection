/* USER CODE BEGIN Header */
/**
  **********************************************************************************************
  * @file           : App/app_hsp_bram_alloc.h
  * @version        : v1_0_Cube
  * @brief          :This file contains resources allocated in HSP BRAM region memory
  ***********************************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
*/
/* Define to prevent recursive inclusion ------------------------------------ */
#ifndef APP_HSP_BRAM_ALLOC_H
#define APP_HSP_BRAM_ALLOC_H
/* USER CODE END Header */

#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */

/* Includes ----------------------------------------------------------------- */
#include "hsp.h"

/* USER CODE BEGIN INCLUDE */

/* USER CODE END INCLUDE */

/** @addtogroup APP_HSP_ENGINE
  * @{
  */
/** @defgroup APP_HSP_BRAM_ALLOC_Exported_Constants Application HSP BRAM Allocation Constants
  * @{
  */

/* USER CODE BEGIN BRAM Constants */

/* USER CODE END BRAM Constants */

/**
  * @}
  */

/** @defgroup HSP_BRAM_STATIC_Resources HSP BRAM Static Resources
  * @{
  */
/* Externalize any buffer/variable required to be use with any HSP Processing functions */

/* USER CODE BEGIN BRAM Static Allocation */

/* USER CODE END BRAM Static Allocation */

/**
  * @}
  */

/** @defgroup HSP_BRAM_DYNAMIC_Resources HSP BRAM Dynamic Resources
  * @{
  */
/* Externalize any buffer/variable required to be use with any HSP Processing functions */
/* USER CODE BEGIN BRAM Dynamic Allocation */

/* USER CODE END BRAM Dynamic Allocation */

/**
  * @}
  */

/** @defgroup APP_HSP_BRAM_ALLOC_Exported_Functions Application HSP BRAM Exported Functions
  * @{
  */
uint32_t HSP_BRAM_Allocation(hsp_engine_context_t *hmw);

/* USER CODE BEGIN Exported Functions */

/* USER CODE END Exported Functions */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* APP_HSP_BRAM_ALLOC_H */
