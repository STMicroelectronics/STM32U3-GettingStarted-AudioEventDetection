/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    hsp_fw_version_conf.h
  * @author  MCD Application Team
  * @brief   Header file
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
#ifndef HSP_FW_VERSION_CONF_H
#define HSP_FW_VERSION_CONF_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Includes ------------------------------------------------------------------*/
/** @addtogroup HSP_ENGINE
  * @{
  */

/** @defgroup HSP_CONF HSP Configuration
  * @{
  */

/** @defgroup defgroup HSP_CONF_Exported_Macros HSP Configuration Macros
  * @{
  */
/* HSP Firmware --------------------------------------------------------------*/
/**
  * @brief Define the version of FW to used
  */
#define HSP_CROM_VERSION(x, y, z)  (((x) << 16U) |((y) << 8U) | (z))
#define HSP_CRAM_VERSION(major, minor)  (((major) << 8U) | (minor))

#define HSP_DEVICE_CROM_VERSION      (HSP_CROM_VERSION(1U, 1U, 7U))  /*!<This value must be filled by CubeMX */
#define HSP_FW_CRAM_TO_LOAD_VERSION  (HSP_CRAM_VERSION(1U, 6U))      /*!<This value can be changed manually based on FW version */
   
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

#endif /* HSP_FW_VERSION_CONF_TEMPLATE_H */
