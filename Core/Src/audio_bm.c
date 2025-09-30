/**
  ******************************************************************************
  * @file    audio_bm.c
  * @author  STMicroelectronics AIS application team
  * @brief   bare metal audio
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <stdio.h>
#include "preproc_dpu.h"
#include "audio_bm.h"
#include "cpu_stats.h"
#include "gs_utils.h"
    
/* Globals--------------------------------------------------------------------*/
AudPreProc_t audio_pre_proc_ctx ;
AudProc_t    audio_proc_ctx ;
volatile int audio_acq_buff_size;
volatile uint8_t * p_acquired_audio_samples;
/* Private function prototypes -----------------------------------------------*/
static void StartAudioSensors(void);
static void StopAudioSensors(void);
/* Private variables ---------------------------------------------------------*/
static const char *sAiAudioClassLabels[CTRL_X_CUBE_AI_MODEL_CLASS_NUMBER] = \
                                                 CTRL_X_CUBE_AI_MODEL_CLASS_LIST;

int audio_de_init_bm(void)
{
  AiDPUReleaseModel(&audio_proc_ctx.ai);
  StopAudioSensors();

  return 0;
}

int audio_init_bm(void)
{
  /* get the AI model   */
  AiDPULoadModel(&audio_proc_ctx.ai);
  audio_proc_ctx.ai.classes = sAiAudioClassLabels;
  audio_proc_ctx.ai_in_p    = (int8_t* ) audio_proc_ctx.ai.p_stai_inputs[0];
  audio_proc_ctx.ai_out_p   = (float* )  audio_proc_ctx.ai.p_stai_outputs[0];
  AudioPreProc_DPUInit( &audio_pre_proc_ctx.dpu) ;

  /* transfer quantization parameters included in AI model to the Audio DPU */
  audio_pre_proc_ctx.dpu.out_Q_offset    = audio_proc_ctx.ai.input_Q_offset;
  audio_pre_proc_ctx.dpu.out_Q_inv_scale = audio_proc_ctx.ai.input_Q_inv_scale;
  audio_pre_proc_ctx.out_p = audio_proc_ctx.ai_in_p ;

  StartAudioSensors();

  return 0;
}

int audio_exec_bm(void)
{
    
  if (audio_acq_buff_size >=  AUDIO_HALF_BUFF_SIZE)
  {
    audio_acq_buff_size -= AUDIO_HALF_BUFF_SIZE;
    uint8_t* p_in = (uint8_t* )p_acquired_audio_samples;

    AudioPreProc_DPU(&audio_pre_proc_ctx.dpu,p_in,audio_pre_proc_ctx.out_p);
    if (audio_pre_proc_ctx.dpu.S_Spectr.spectro_sum > CTRL_X_CUBE_AI_SPECTROGRAM_SILENCE_THR)
    {
      AiDPUProcess(&audio_proc_ctx.ai,audio_proc_ctx.ai_out_p);
      PrintAIClassesOutput(audio_proc_ctx.ai_out_p,audio_proc_ctx.ai.classes);
    }
    audio_pre_proc_ctx.dpu.S_Spectr.spectro_sum = 0 ;
  }
  return 0;
}

/* Callback Definition -------------------------------------------------------*/
/**
  * @brief  MDF acquisition complete callback.
  * @param  hmdf MDF handle.
  * @retval None.
  */
void HAL_MDF_AcqCpltCallback(MDF_HandleTypeDef *hmdf)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hmdf);
  audio_acq_buff_size += AUDIO_HALF_BUFF_SIZE;
  p_acquired_audio_samples = audio_pre_proc_ctx.acq_p;
}

/**
  * @brief  MDF acquisition half complete callback.
  * @param  hmdf MDF handle.
  * @retval None.
  */
void HAL_MDF_AcqHalfCpltCallback(MDF_HandleTypeDef *hmdf)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hmdf);
  audio_acq_buff_size += AUDIO_HALF_BUFF_SIZE;
  p_acquired_audio_samples = audio_pre_proc_ctx.acq_p + AUDIO_HALF_BUFF_SIZE;
}

/**
  * @brief  MDF sound level callback.
  * @param  hmdf MDF handle.
  * @param  SoundLevel Sound level value computed by sound activity detector.
  *         This parameter can be a value between Min_Data = 0 and Max_Data = 32767.
  * @param  AmbientNoise Ambient noise value computed by sound activity detector.
  *         This parameter can be a value between Min_Data = 0 and Max_Data = 32767.
  * @retval None.
  */
void HAL_MDF_SndLvlCallback(MDF_HandleTypeDef *hmdf, uint32_t SoundLevel, uint32_t AmbientNoise)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hmdf);
  UNUSED(SoundLevel);
  UNUSED(AmbientNoise);
}
  /* NOTE : This function should not be modified, when the function is needed,
            the HAL_MDF_SndLvlCallback could be implemented in the user file */
/**
  * @brief  MDF sound activity detector callback.
  * @param  hmdf MDF handle.
  * @retval None.
  */
void HAL_MDF_SadCallback(MDF_HandleTypeDef *hmdf)
{
  UNUSED(hmdf);
}

/**
 * @brief Initializes the audio sensor (digital microphone)
 * @retval None
 */
static void StartAudioSensors(void)
{
  assert_param(AUDIO_BUFF_SIZE < 0xFFFF );
  MDF_DmaConfigTypeDef dma_config;
  dma_config.Address    = (uint32_t)audio_pre_proc_ctx.acq_p;
  dma_config.DataLength = AUDIO_BUFF_SIZE;
  dma_config.MsbOnly    = ENABLE;
  if (HAL_OK != HAL_MDF_AcqStart_DMA(&AdfHandle0, &AdfFilterConfig0, &dma_config))
  {
    Error_Handler();
  }
}

/**
 * @brief Initializes the audio sensor (digital microphone)
 * @retval None
 */
static void StopAudioSensors(void)
{
  /* Stop PDM record */
  if (HAL_OK != HAL_MDF_AcqStop_DMA(&AdfHandle0))
  {
    Error_Handler();
  }
}

