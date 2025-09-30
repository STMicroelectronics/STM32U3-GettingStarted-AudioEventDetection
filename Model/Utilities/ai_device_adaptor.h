
/**
 ******************************************************************************
 * @file    ai_device_adaptor.h
 * @author  MCD/AIS Team
 * @brief   Device port functions for DEVICE AI test application
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2019,2023 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software is licensed under terms that can be found in the LICENSE file in
 * the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

/*
 * Simple header file with helper macro/function to adapt the ai test applications.
 *  - before to include this file, _NO_DEVICE_PORT_FUNC can be defined to remove the
 *    port functions. Only ARM tool-chain and CMSIS-M-CORE related macros will be defined.
 *
 * History:
 *  - v1.0: initial version
 *  - v1.1: rework/clean port functions
 *
 *
 * ** Core functions/macros
 *
 *  cmsis_compiler.h              Compiler CMSIS Cortex-M file (implementation is Embedded ARM tool-chain dependent)
 *  _IS_XX_COMPILER               define the used Embedded ARM tool-chain
 *  MEM_ALIGNED(x)                indicate the expected memory alignment (1,2,4,8,16,32 or 64 bytes)
 *
 * ** System/IO functions
 *
 *  _NO_DEVICE_PORT_FUNC          if defined only the core functions/macros are defined/exported.
 *
 *  HAS_DWT_CTRL                  =1 indicates that a DWT IP is available
 *  HAS_PMU_CTRL                  =1 indicates that a PMU IP is available
 *  HAS_SYS_TICK                  =1 indicates that a SYS_TICK (or AGT) is available
 *
 *  port_hal_get_hal_version()    return SDK/HAL/OSAL version if used (else 0)
 *  port_hal_get_dev_id()         return device ID
 *  port_hal_get_rev_id()         return revision ID
 *
 *  port_hal_crc_ip_init()        enable the CRC IP access if used by the library
 *
 *  port_hal_get_cpu_freq()       return main CPU/MCU frequency (Hz)
 *  port_hal_get_sys_freq()       return main sys-bus frequency (Hz)
 *  port_hal_get_frequency()      return the frequency to convert clock cycles (Hz)
 *
 *  port_hal_delay(x)             wait x milliseconds
 *  port_hal_get_tick()           return number of tick (1 tick = 1ms)
 *
 *  port_get_reload_value         return the reload time value (from SysTick or AGT)
 *  port_get_time_value           return the time value (from SysTick or AGT)
 *
 *  port_dwt_init()               initialize the DWT MCU clock counter
 *  port_dwt_reset()              reset the DWT MCU clock counter
 *  port_dwt_get_cycles()         return the current DWT MCU clock counter
 *
 *
 *  HAS_DEDICATED_PRINT_PORT      if defined and equal to 1, indicate that a dedicated COM port is available
 *                                for the log services.
 *  USE_PRINTF_FROM_TOOL          if defined, indicate that the LC_PRINT(..) function is mapped on the default
 *                                printf(..) implementation from the SDK/tool-chain else low-cost implementation is
 *                                used (lc_print())
 *
 *  USE_RETARGET_IO_FROM_TOOL     if defined (and ==1) indicates that the retarget-io feature is natively
 *                                supported by the SDK/tool-chain (not provided by the aiValidation code, see aiTestUtility.c)
 *
 */

#ifndef __AI_DEVICE_ADAPTOR_H__
#define __AI_DEVICE_ADAPTOR_H__

#include "stm32u3xx_hal.h"

#if !defined(SR6X)
#include "cmsis_compiler.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif


/* -----------------------------------------------------------------------------
 * MACRO - ARM tool chain definition
 * -----------------------------------------------------------------------------
 */

#undef _IS_GCC_COMPILER
#undef _IS_IAR_COMPILER

/* ARM Compiler 5 tool-chain */
#if defined ( __CC_ARM )
// #if ((__ARMCC_VERSION >= 5000000) && (__ARMCC_VERSION < 6000000))
#define _IS_AC5_COMPILER    1

/* ARM Compiler 6 tool-chain */
#elif defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
#define _IS_AC6_COMPILER    1


/* GCC tool-chain */
#elif defined ( __GNUC__ )
#define _IS_GCC_COMPILER    1

/* IAR tool-chain */
#elif defined ( __ICCARM__ )
#define _IS_IAR_COMPILER    1

#else
#error ARM MCU tool chain is not detected/supported
#endif


/* -----------------------------------------------------------------------------
 * MACRO - MEM align definition
 * -----------------------------------------------------------------------------
 */

#define _CONCAT_ARG(a, b)     a ## b
#define _CONCAT(a, b)         _CONCAT_ARG(a, b)

#if defined(_IS_IAR_COMPILER) && _IS_IAR_COMPILER
  #define MEM_ALIGNED(x)         _CONCAT(MEM_ALIGNED_,x)
  #define MEM_ALIGNED_1          _Pragma("data_alignment = 1")
  #define MEM_ALIGNED_2          _Pragma("data_alignment = 2")
  #define MEM_ALIGNED_4          _Pragma("data_alignment = 4")
  #define MEM_ALIGNED_8          _Pragma("data_alignment = 8")
  #define MEM_ALIGNED_16         _Pragma("data_alignment = 16")
  #define MEM_ALIGNED_32         _Pragma("data_alignment = 32")
  #define MEM_ALIGNED_64         _Pragma("data_alignment = 64")
#elif defined(_IS_ACx_COMPILER) && _IS_ACx_COMPILER
  #define MEM_ALIGNED(x)         __attribute__((aligned (x)))
#elif defined(_IS_GCC_COMPILER) && _IS_GCC_COMPILER
  #define MEM_ALIGNED(x)         __attribute__((aligned(x)))
#else
  #define MEM_ALIGNED(x)
#endif


#if !defined(_NO_DEVICE_PORT_FUNC)

#include <stdint.h>
#include <stdbool.h>

#if defined(USE_PRINTF_FROM_TOOL)
// Legacy definition
#if USE_PRINTF_FROM_TOOL != 0
#undef USE_PRINTF_FROM_TOOL
#define USE_PRINTF_FROM_TOOL 1
#endif
#endif


/* -----------------------------------------------------------------------------
 * System/IO functions
 * -----------------------------------------------------------------------------
 */

/* used only by Stellar MCUs, empty for all other MCUs */
#define port_io_init()

#define DEVICE_FAMILY "STM32"

/* --------------------------------------------
 * Default STM32 series implementation.
 *  All requested HAL functions are available.
 * --------------------------------------------
 */


#define port_hal_get_hal_version()        HAL_GetHalVersion()      
#define port_hal_get_dev_id()             HAL_GetDEVID() 
#define port_hal_get_rev_id()             HAL_GetREVID() 


#define port_hal_get_cpu_freq()           HAL_RCC_GetHCLKFreq()
#define port_hal_get_frequency()          port_hal_get_cpu_freq()
#define port_hal_get_sys_freq()           HAL_RCC_GetHCLKFreq()

#define port_hal_delay(delay_)            HAL_Delay(delay_)
#define port_hal_get_tick()               HAL_GetTick()

#define port_get_reload_value()           SysTick->LOAD
#define port_get_time_value()             SysTick->VAL

#define USE_RETARGET_IO_FROM_TOOL         1

#define HAS_PMU_CTRL                      0
#define HAS_DWT_CTRL                      1
#define HAS_SYS_TICK                      1

#define port_dwt_init()                   port_dwt_init_imp()
#define port_dwt_reset()                  DWT->CYCCNT = 0
#define port_dwt_get_cycles()             DWT->CYCCNT
    
extern UART_HandleTypeDef UartHandle;

__STATIC_INLINE bool port_io_get(uint8_t *c, uint32_t timeout)
{
  HAL_StatusTypeDef status;

  if (!c)
    return false;

  status = HAL_UART_Receive(&UartHandle, (uint8_t *)c, 1, timeout);

  if (status == HAL_TIMEOUT)
    return false;

  return (status == HAL_OK);
}

__STATIC_INLINE bool port_io_write(uint8_t *buff, int count)
{
  HAL_StatusTypeDef status;

  status = HAL_UART_Transmit(&UartHandle, buff, count, HAL_MAX_DELAY);

  return (status == HAL_OK);
}

__STATIC_INLINE bool port_io_read(uint8_t *buff, int count)
{
  HAL_StatusTypeDef status;

  status = HAL_UART_Receive(&UartHandle, buff, count, HAL_MAX_DELAY);

  return (status == HAL_OK);
}

#endif /* other STM32xx series */


void port_dwt_init_imp(void);
void port_hal_set_extra_conf(uint32_t *extra);


/* -----------------------------------------------------------------------------
 * Set LC_PRINT(.) definition
 * -----------------------------------------------------------------------------
 */

#if !defined(USE_PRINTF_FROM_TOOL) || USE_PRINTF_FROM_TOOL != 1

//#include "lc_print.h"
#include <stddef.h>
#include <stdarg.h>

extern void lc_print(const char* fmt, ... );
extern size_t lc_vsnprint(char *buff, size_t size, const char* fmt, va_list vlist);

#define LC_PRINT(fmt, ...)                    lc_print(fmt, ##__VA_ARGS__)
#define LC_VSNPRINT(buff, size, fmt, va_list) lc_vsnprint(buff, size, fmt, va_list)

#else /* !USE_PRINTF_FROM_TOOL */

#include <stdio.h>

#define LC_PRINT(fmt, ...)                    printf(fmt, ##__VA_ARGS__)
#define LC_VSNPRINT(buff, size, fmt, va_list) vsnprintf(buff, size, fmt, va_list)

#endif /* USE_PRINTF_FROM_TOOL */


#endif /* !_NO_DEVICE_PORT_FUNC */

#ifdef __cplusplus
}
#endif
