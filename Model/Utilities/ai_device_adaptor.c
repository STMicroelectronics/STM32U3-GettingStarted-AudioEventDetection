/**
 ******************************************************************************
 * @file    ai_device_adaptor.c
 * @author  MCD/AIS Team
 * @brief   Specific DEVICE port functions for AI test application
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software is licensed under terms that can be found in the LICENSE file in
 * the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

#include "ai_device_adaptor.h"
 
#ifdef __cplusplus
extern "C" {
#endif



#if defined(HAS_DWT_CTRL) && HAS_DWT_CTRL == 1

void port_dwt_init_imp()
{
  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
  DWT->CYCCNT = 0;
  DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}


#endif /* !NO_PORT_DWT_INIT_IMP */

#ifdef __cplusplus
}
#endif
