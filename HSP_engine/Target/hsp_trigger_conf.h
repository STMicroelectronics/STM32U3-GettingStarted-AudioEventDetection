/**
  ******************************************************************************
  * @file    hsp_trigger_conf_template.h
  * @brief   Header file for input trigger definition
  ******************************************************************************
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef HSP_TRIGGER_CONF_TEMPLATE_H
#define HSP_TRIGGER_CONF_TEMPLATE_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Includes ------------------------------------------------------------------*/

/** @addtogroup HSP_ENGINE
  * @{
  */

/** @defgroup HSP_CONF
  * @{
  */

/** @defgroup HSP_TRIGGER_CONF
  * @{
  */

/** @defgroup HSP_TRIGGER_CONF_Exported_Constants
  * @{
  */
/**
  * @brief Defines to identify all the input triggers interconnected with the HSP.
  * Ex of trigger input definitions for STM32U3C5
  *   #define HSP_TRGI_0  0U
  *   #define HSP_TRGI_1  1U
  *   #define HSP_TRGI_2  2U
  *   ...
  * 
  *   #define HSP_DMA1_CHANNEL8_TC  HSP_TRGI_0
  *   #define HSP_DMA1_CHANNEL9_TC  HSP_TRGI_1
  *   #define HSP_DMA1_CHANNEL10_TC  HSP_TRGI_2
  *   ...
  */
/* Defines for TRGIN signals ------------------------------------------------ */
/* #define HSP_TRGI_x  xU */

/* #define HSP_<literal>  HSP_TRGI_x */

/**
  * @brief Defines to identify all the outputs triggers interconnected with the HSP.
  * Ex of trigger outputs definitions for STM32U3C5
  *   #define HSP_TRGO_0  (0x01UL << 0U)
  *   #define HSP_TRGO_1  (0x01UL << 1U)
  *   #define HSP_TRGO_2  (0x01UL << 2U)
  *   #define HSP_TRGO_3  (0x01UL << 3U)
  *   ...
  *
  *   #define HSP_TRGO_DMA1_TRIG_34  HSP_TRGO_0
  *   #define HSP_TRGO_DMA1_TRIG_35  HSP_TRGO_1
  *   #define HSP_TRGO_DMA1_TRIG_36  HSP_TRGO_2
  *   #define HSP_TRGO_DMA1_TRIG_37  HSP_TRGO_3
  *   ...
  *
  *   #define HSP_GPO_MASK_POS  16U
  *   #define HSP_GPO_0  (1UL << HSP_GPO_MASK_POS)
  *   #define HSP_GPO_1  (1UL << (HSP_GPO_MASK_POS + 1U))
  *   #define HSP_GPO_2  (1UL << (HSP_GPO_MASK_POS + 2U))
  *   #define HSP_GPO_3  (1UL << (HSP_GPO_MASK_POS + 3U))
  *   ...
  *
  *   #define HSP_GPO_DMA1_TRIG_38  HSP_GPO_0
  *   #define HSP_GPO_DMA1_TRIG_39  HSP_GPO_1
  *   #define HSP_GPO_DMA1_TRIG_40  HSP_GPO_2
  *   #define HSP_GPO_DMA1_TRIG_41  HSP_GPO_3
  *   ...
  */
/* Defines for TRGO signals ------------------------------------------------- */
/* #define HSP_TRGO_x  xU */

/* #define HSP_TRGO_<literal>  HSP_TRGO_x */

/* Defines for GPO signals -------------------------------------------------- */
/* #define HSP_GPO_x  xU */

/* #define HSP_GPO_<literal>  HSP_GPO_x */

/**
  * @}
  */

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

#endif /* HSP_TRIGGER_CONF_TEMPLATE_H */
