/**
  ******************************************************************************
  * @file   : audio_bm.h
  * @author : STMicroelectronics AIS application team
  * @brief  : Main program body
  * @version $Version$
  * @date    $Date$
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
#ifndef __AUDIO_BM_H__
#define __AUDIO_BM_H__
#include "ai_dpu.h"
#include "preproc_dpu.h"

typedef struct _AudPreProc_t
{
   AudioPreProcCtx_t dpu;
   uint8_t           acq_p[AUDIO_BUFF_SIZE];
   int8_t*           out_p;
}AudPreProc_t;

typedef struct _AudProc_t
{
  AIProcCtx_t ai;
  int8_t*     ai_in_p;
  float*      ai_out_p;
}AudProc_t;

extern int audio_init_bm(void);
extern int audio_de_init_bm(void);
extern int audio_exec_bm(void);

#endif /* __AUDIO_BM_H__ */
