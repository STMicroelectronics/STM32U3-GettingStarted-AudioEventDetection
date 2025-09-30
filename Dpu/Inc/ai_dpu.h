/**
  ******************************************************************************
  * @file    ai_dpu.h
  * @author  MCD Application Team
  * @brief   Header for ai_dpu.c module
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


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _AI_DPU_H
#define _AI_DPU_H

/* Includes ------------------------------------------------------------------*/
#include "dpu.h"
#include "stai.h"     /* include ST Edge AI macros, types and data structures */
#include "network.h"
#include "network_data.h"

/* Exported constants --------------------------------------------------------*/
#define AI_MNETWORK_NUMBER         (1U)

#define AI_DPU_CHANNEL_LAST

#ifdef AI_DPU_CHANNEL_LAST
#define AI_DPU_BATCH       (0)
#define AI_DPU_WIDTH       (1)
#define AI_DPU_HEIGHT      (2)
#define AI_DPU_CHANNEL     (3)
#endif

#define AI_DPU_X_CUBE_AI_API_MAJOR (1)
#define AI_DPU_X_CUBE_AI_API_MINOR (0)
#define AI_DPU_X_CUBE_AI_API_MICRO (0)
#define AI_DPU_NB_MAX_INPUT        (1U)
#define AI_DPU_NB_MAX_OUTPUT       (1U)

/* Exported types ------------------------------------------------------------*/

typedef struct {
  /* Global handle to reference the instantiated C-model */
  stai_network * p_network ;
  stai_ptr     * p_stai_inputs;
  stai_ptr     * p_stai_outputs;
  stai_ptr     * p_activations;
  
  stai_network_info info;
  
  uint32_t sensor_type;                         /* Specifies AI data nature. */
  stai_format input_format;   /* input format, output format is always float */
  float  input_Q_inv_scale;                  /* quantization scale parameter */
  int16_t input_Q_offset;                   /* quantization offset parameter */
  int in_height;
  int in_width;
  int out_height;
  const char ** classes;
}AIProcCtx_t;

/* Exported functions --------------------------------------------------------*/
DPU_StatusTypeDef AiDPULoadModel(AIProcCtx_t * pxCtx);
DPU_StatusTypeDef AiDPUReleaseModel(AIProcCtx_t * pxCtx);
DPU_StatusTypeDef AiDPUProcess(AIProcCtx_t *pxCtx, float *pf_out);

#endif /* _AI_DPU_H */
