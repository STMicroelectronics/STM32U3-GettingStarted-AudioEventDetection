/**
  ******************************************************************************
  * @file    app_config.h
  * @author  STMicroelectronics AIS application team
  * @version $Version$
  * @date    $Date$
  *
  * @brief
  *
  * <DESCRIPTIOM>
  *
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

#ifndef __APP_CONFIG_H__
#define __APP_CONFIG_H__

#include "logging.h"
//#define LOG_LEVEL LOG_DEBUG
#define LOG_LEVEL LOG_INFO
#define my_printf LogInfo

#define USE_UART_BAUDRATE               (115200) /* can up set up upto 921600 */
#define APP_CONF_STR "Bare Metal"
#define CPU_STATS
#define MENU_DISP_PERIOD (1000)

#define NO_TRANSPOSE 1
#define SEPARATION_LINE "---------------------------------------------------------------\n\r"

#endif /* __APP_CONFIG_H__ */
