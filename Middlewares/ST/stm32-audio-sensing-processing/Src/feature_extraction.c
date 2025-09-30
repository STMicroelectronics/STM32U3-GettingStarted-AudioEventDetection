/**
  ******************************************************************************
  * @file    feature_extraction.c
  * @author  MCD Application Team
  * @brief   Spectral feature extraction functions
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
 
#include "feature_extraction.h"
#include "audio_din.h"

#ifdef USE_HSP
#include "hsp_conf.h"
#include "hsp_direct_command.h"
extern hsp_engine_context_t hmw;
#include "hsp_memcpy.h"
extern hsp_engine_context_t hmw;
#define memcpy(dst, src, size)  stm32_hsp_memcpy((int8_t *) dst, (int8_t *) src, size)
#endif



#ifndef M_PI
#define M_PI    3.14159265358979323846264338327950288 /*!< pi */
#endif

#define NORM_Q15  (1.0F/32768.0F)

/**
 * @defgroup groupFeature Feature Extraction
 * @brief Spectral feature extraction functions
 * @{
 */

/**
 * @brief      Convert 16-bit PCM into floating point values
 *
 * @param      *pInSignal  points to input signal buffer
 * @param      *pOutSignal points to output signal buffer
 * @param      len         signal length
 */
void buf_to_float(int16_t *pInSignal, float32_t *pOutSignal, uint32_t len)
{
  for (uint32_t i = 0; i < len; i++)
  {
    pOutSignal[i] = (float32_t) pInSignal[i];
  }
}

/**
 * @brief      Convert 16-bit PCM into normalized floating point values
 *
 * @param      *pInSignal   points to input signal buffer
 * @param      *pOutSignal  points to output signal buffer
 * @param      len          signal length
 */
void buf_to_float_normed(int16_t *pInSignal, float32_t *pOutSignal, uint32_t len)
{
  for (uint32_t i = 0; i < len; i++)
  {
    pOutSignal[i] = (float32_t) pInSignal[i] / (1 << 15);
  }
}

/**
 * @brief      Power Spectrogram column
 *
 * @param      *S          points to an instance of the floating-point Spectrogram structure.
 * @param      *pInSignal  points to the in-place input signal frame of length FFTLen.
 * @param      *pOutCol    points to  output Spectrogram column.
 * @param	     n_fft number of points in FFT
 * @param      eTtype spectrogram type (magnitude or squared)
 * @param      p_sum spectrum sum
 * @return     None
 */
void SpectrogramColumn(float32_t *pInSignal, float32_t *pOutCol, size_t n_fft, eSpectrogram_TypeTypedef eType, float32_t *p_sum)
{
    
#ifdef USE_HSP
    if (eType == SPECTRUM_TYPE_MAGNITUDE)
    {
        HSP_ACC_CmplxMag_f32(&hmw, pInSignal, pOutCol, n_fft/2);
    }
    else
    {
        HSP_ACC_CmplxMagSquared_f32(&hmw, pInSignal, pOutCol, n_fft/2);
    }
#else    
    /* Power spectrum */
  float32_t first_energy;
  float32_t last_energy;

  /* Power spectrum */
  first_energy = pInSignal[0] * pInSignal[0];
  last_energy = pInSignal[1] * pInSignal[1];
  pOutCol[0] = first_energy;
  pOutCol[n_fft / 2] = last_energy;

  /* Magnitude spectrum */
  if (eType == SPECTRUM_TYPE_MAGNITUDE)
  {
    arm_sqrt_f32(pOutCol[0], &pOutCol[0]);
    arm_sqrt_f32(pOutCol[n_fft / 2], &pOutCol[n_fft / 2]);
    arm_cmplx_mag_f32(&pInSignal[2], &pOutCol[1], (n_fft / 2) - 1);
  } else {
    arm_cmplx_mag_squared_f32(&pInSignal[2], &pOutCol[1], (n_fft / 2) - 1);
  }
#endif
#ifdef CMSIS_DSP_VERSION_CORRECT
  arm_accumulate_f32(pOutCol, (n_fft / 2) , p_sum);
#else
  for (size_t i = 0; i < (n_fft / 2) ; i++) {
    *p_sum += pOutCol[i];
  }
#endif
}

/**
 * @brief      Power Spectrogram column
 *
 * @param      *S          points to an instance of the floating-point Spectrogram structure.
 * @param      *pInSignal  points to the in-place input signal frame of length FFTLen/2.
 * @param      *pOutCol    points to  output Spectrogram column.
 * @return     None
 */
void SpectrogramOneColumn(Spectrogram_t *S, float32_t *pInSignal, float32_t *pOutCol)
{
  uint32_t n_fft = S->pFFT->FFTLen;

  float32_t first_energy;
  float32_t last_energy;
  float32_t *p_sum = &S->spectro_sum;

  /* Power spectrum */
  first_energy = pInSignal[0] * pInSignal[0];
  last_energy = pInSignal[1] * pInSignal[1];
  pOutCol[0] = first_energy;
  pOutCol[n_fft / 2] = last_energy;

  /* Magnitude spectrum */
  if (S->Type == SPECTRUM_TYPE_MAGNITUDE)
  {
    arm_sqrt_f32(pOutCol[0], &pOutCol[0]);
    arm_sqrt_f32(pOutCol[n_fft / 2], &pOutCol[n_fft / 2]);
    arm_cmplx_mag_f32(&pInSignal[2], &pOutCol[1], (n_fft / 2) - 1);
  } else {
    arm_cmplx_mag_squared_f32(&pInSignal[2], &pOutCol[1], (n_fft / 2) - 1);
  }
#ifdef CMSIS_DSP_VERSION_CORRECT
  arm_accumulate_f32(pOutCol, (n_fft / 2) + 1, p_sum);
#else
  for (size_t i = 0; i < (n_fft / 2) + 1; i++) {
    *p_sum += pOutCol[i];
  }
#endif
}

/**
 * @brief      Mel Spectrogram column
 *
 * @param      *S          points to an instance of the floating-point Mel structure.
 * @param      *pInSignal  points to input signal frame of length FFTLen.
 * @param      *pOutCol    points to  output Mel Spectrogram column.
 * @return     None
 */
void MelSpectrogramColumn(MelSpectrogramTypeDef *S, float32_t *pInSignal, float32_t *pOutCol)
{
  float32_t *tmp_buffer = S->SpectrogramConf->pScratch1;

  /* Power Spectrogram */
  SpectrogramColumn(pInSignal, tmp_buffer, S->SpectrogramConf->FFTLen, S->SpectrogramConf->Type, &(S->SpectrogramConf->spectro_sum));

  /* Mel Filter Banks Application */
  MelFilterbank(S->MelFilter, tmp_buffer, pOutCol);
}

/**
 * @brief      Log-Mel Spectrogram column
 *
 * @param      *S          points to an instance of the floating-point Log-Mel structure.
 * @param      *pInSignal  points to input signal frame of length FFTLen.
 * @param      *pOutCol    points to  output Log-Mel Spectrogram column.
 * @return     None
 */
void LogMelSpectrogramColumn(LogMelSpectrogramTypeDef *S, float32_t *pInSignal, float32_t *pOutCol)
{
  uint32_t n_mels = S->MelSpectrogramConf->MelFilter->NumMels;
  float32_t top_dB = S->TopdB;
  float32_t ref = S->Ref;
  SpectrogramTypeDef *p_spectro = S->MelSpectrogramConf->SpectrogramConf;
  float32_t *tmp_buffer = p_spectro->pScratch1;

  SpectrogramColumn(pInSignal, tmp_buffer, p_spectro->FFTLen, p_spectro->Type, &(p_spectro->spectro_sum));

  /* Mel Filter Banks Application to power spectrum column */
  MelFilterbank(S->MelSpectrogramConf->MelFilter, tmp_buffer, pOutCol);

  /* Scaling */
  for (uint32_t i = 0; i < n_mels; i++) {
    pOutCol[i] /= ref;
  }

  /* Avoid log of zero or a negative number */
  for (uint32_t i = 0; i < n_mels; i++) {
    if (pOutCol[i] <= 0.0f) {
      pOutCol[i] = FLT_MIN;
    }
  }
  if (S->LogFormula == LOGMELSPECTROGRAM_SCALE_DB)
  {
    /* Convert power spectrogram to decibel */
    for (uint32_t i = 0; i < n_mels; i++) {
      pOutCol[i] = 10.0f * log10f(pOutCol[i]);
    }

    /* Threshold output to -top_dB dB */
    for (uint32_t i = 0; i < n_mels; i++) {
      pOutCol[i] = (pOutCol[i] < -top_dB) ? (-top_dB) : (pOutCol[i]);
    }
  }
  else
  {
    /* Convert power spectrogram to log scale */
    for (uint32_t i = 0; i < n_mels; i++) {
      pOutCol[i] = logf(pOutCol[i]);
    }
  }
}

/**
 * @brief      Compute FFT (float132) from Q15 input 
                    returned FFT is packed to NFFT/2 samples
 *
 * @param      *pInSignal points to input signal buffer
 * @param      *pOut      points to output FFT
 * @param      *S         points to FFT structure
 * @return     None
 */
void audio_is16of32_fft(const int16_t *pInSignal, FFTTypeDef_t *S, float32_t *pOut) 
{
  // Get one padded frame from int16 to float32
    
#ifdef USE_HSP
    int16_t *p16_in = (int16_t *) (pOut + S->FFTLen/2);
    memset(p16_in, 0, S->sDin.pad_left*sizeof(int16_t));
    memcpy((int8_t *) (p16_in + S->sDin.pad_left), (int8_t *) pInSignal, 2 * S->sDin.FrameLen);
    
    HSP_ACC_VectQ152F(&hmw, (int32_t *) p16_in , pOut, S->FFTLen);
#else  
  audio_is16of32_pad(pInSignal, S->pScratch, S->sDin.pad_left, S->sDin.FrameLen, S->sDin.pad_right);
#endif
  /* In-place window application (on signal length, not entire n_fft) */
  /* @note: OK to typecast because hannWin content is not modified */
  if (NULL != S->pWindow) {
#ifdef USE_HSP
        HSP_ACC_WinSym_f32(&hmw, pOut + S->sDin.pad_left, S->pWindow, pOut + S->sDin.pad_left, S->sDin.FrameLen, S->sDin.pad_right);
#else
    arm_mult_f32(S->pScratch+S->sDin.pad_left, S->pWindow, S->pScratch+S->sDin.pad_left, S->sDin.FrameLen);
#endif
  }
  /* FFT */
#ifdef USE_HSP

#define DIRECT_FFT 0    
#define LOG2_NFFT (31 - __CLZ(S->FFTLen))
#define BIT_REVERSE_FFT 1
/* Real FFT Mode1 : N points (No Nyquist) */
#define REAL_FFT_MODE1 1
  hsp_ftt_lognbp_cmd_t lognbp; 
switch (LOG2_NFFT)
{
case (5):
  lognbp = HSP_LOG2NBP_32;           /**< Value of Log2(32) for FFT 32 points */
  break;
case (6):
  lognbp = HSP_LOG2NBP_64;           /**< Value of Log2(64) for FFT 64 points */
  break;
case (7):
  lognbp = HSP_LOG2NBP_128;         /**< Value of Log2(128) forFFT 128 points */
  break;
case (8):
  lognbp = HSP_LOG2NBP_256;         /**< Value of Log2(256) forFFT 256 points */
  break;
case (9):
  lognbp = HSP_LOG2NBP_512;         /**< Value of Log2(512) forFFT 512 points */
  break;
case (10):
  lognbp = HSP_LOG2NBP_1024;      /**< Value of Log2(1024) forFFT 1024 points */
  break;
case (11):
  lognbp = HSP_LOG2NBP_2048;      /**< Value of Log2(2048) forFFT 2048 points */
  break;
case (12):
  lognbp = HSP_LOG2NBP_4096;      /**< Value of Log2(4096) forFFT 4096 points */
  break;
default:
  lognbp = HSP_LOG2NBP_32 ;                   /* invalid value is missing ... */
  break;
}
  
    
    HSP_ACC_Rfft_f32(&hmw, pOut, lognbp, DIRECT_FFT, BIT_REVERSE_FFT, HSP_RFFT_TYPE_1);
#else  
    arm_rfft_fast_f32(S->pRfft, S->pScratch, pOut, 0);
#endif  

}

/**
 * @brief      Compute Log Mel Spectrogram for one frame input
 *
 * @param      *S         points to LogMelSpectrogram structure
 * @param      *pInSignal points to input signal int16 buffer
 * @param      *pOutCol   points to output logmelspectrogram for the frame
 * @param      offset    quantization offset 
 * @param      inv_scale quantization inverse scale 

 * @return     None
 */
void LogMelSpectrogramColumn_q15_Q8(LogMelSpectrogramTypeDef *S, const int16_t *pInSignal, int8_t *pOutCol, int8_t offset, float32_t inv_scale)
{
  float32_t *p_in, *p_out;
  SpectrogramTypeDef *p_spectro = S->MelSpectrogramConf->SpectrogramConf;
  FFTTypeDef_t sFFT = { 
    .pRfft = p_spectro->pRfft,
    .pWindow = p_spectro->pWindow,
    .pScratch= p_spectro->pScratch1,
    .FFTLen = p_spectro->FFTLen,
    .sDin.FrameLen = p_spectro->FrameLen,
    .sDin.pad_left = p_spectro->pad_left,
    .sDin.pad_right = p_spectro->pad_right
  };

  FFTTypeDef_t *pFFT = &sFFT; 
#ifndef USE_HSP
  float32_t top_dB              = S->TopdB;
#endif  
  float32_t ref                 = S->Ref;
  uint32_t n_mels               = S->MelSpectrogramConf->MelFilter->NumMels;

  // First scratch buffer for float data after optional windowing
  // Output of FFT in second scratch buffer
  audio_is16of32_fft(pInSignal, pFFT, p_spectro->pScratch2);
                     
    /* swap input & output buffers*/
    p_in  = p_spectro->pScratch2;
    p_out  = p_spectro->pScratch1;

  /* compute spectrogram  with second scratch buffer as input*/

  SpectrogramColumn(p_in, p_out, p_spectro->FFTLen, p_spectro->Type, &(p_spectro->spectro_sum));

 
    /* swap input & output buffers*/
  p_out = p_spectro->pScratch2;
    p_in  = pFFT->pScratch;
    
  /* Mel Filter Banks Application to power spectrum column */
  MelFilterbank(S->MelSpectrogramConf->MelFilter,p_in, p_out);

#ifdef USE_HSP
  HSP_ACC_VectIScale_f32(&hmw, p_out, 1/ref, p_out, n_mels);
  
  HSP_ACC_VectIOffset_f32(&hmw, p_out, FLT_MIN, p_out, n_mels);
  
  HSP_ACC_VectLn_f32(&hmw, p_out,  p_out, n_mels);
#else  
  
  /* Scaling */
  for (uint32_t i = 0; i < n_mels; i++) {
    p_out[i] /= ref;
  }

  /* Avoid log of zero or a negative number */
  for (uint32_t i = 0; i < n_mels; i++) {
    if (p_out[i] <= 0.0f) {
      p_out[i] = FLT_MIN;
    }
  }

  if (S->LogFormula == LOGMELSPECTROGRAM_SCALE_DB)
  {
    /* Convert power spectrogram to decibel */
    for (uint32_t i = 0; i < n_mels; i++) {
      p_out[i] = 10.0f * log10f(p_out[i]);
    }

    /* Threshold output to -top_dB dB */
    for (uint32_t i = 0; i < n_mels; i++) {
      p_out[i] = (p_out[i] < -top_dB) ? (-top_dB) : (p_out[i]);
    }
  }
  else
  {
    /* Convert power spectrogram to log scale */
    for (uint32_t i = 0; i < n_mels; i++) {
      p_out[i] = logf(p_out[i]);
    }
  }
#endif

  /*  Quantization */
  for (uint32_t i=0 ; i < n_mels ; i++ ){
    pOutCol[i]= (int8_t)__SSAT((int32_t)roundf(p_out[i]*inv_scale + (float) offset), 8);
  }

}


/**
 * @brief      Mel-Frequency Cepstral Coefficients (MFCCs) column
 *
 * @param      *S          points to an instance of the floating-point MFCC structure.
 * @param      *pInSignal  points to input signal frame of length FFTLen.
 * @param      *pOutCol    points to  output MFCC spectrogram column.
 * @return     None
 */
void MfccColumn(MfccTypeDef *S, float32_t *pInSignal, float32_t *pOutCol)
{
  float32_t *tmp_buffer = S->pScratch;

  LogMelSpectrogramColumn(S->LogMelConf, pInSignal, tmp_buffer);

  /* DCT for computing MFCCs from spectrogram slice. */
  DCT(S->pDCT, tmp_buffer, pOutCol);
}

/**
 * @brief      Compute Log Mel Spectrograms for nb_frames input 
               (Audio event detection preprocess)
 *
 * @param      *S         points to LogMelSpectrogram structure
 * @param      *pInSignal points to input signal int16 buffer
 * @param      *pOut      points to output logmelspectrogram for nb_frame

 * @return     None
 */
void LogMelSpectrogramColumn_q15_os8_batch(LogMelSpectrogramArray *S, int16_t *pInSignal, int8_t *pOut) { 
    
  uint32_t n_mels = S->S_LogMelSpect->MelSpectrogramConf->MelFilter->NumMels;
  for (uint32_t i = 0; i < S->nb_frames; i++ )
  {
    // Process one column
    LogMelSpectrogramColumn_q15_Q8(S->S_LogMelSpect, pInSignal, S->pScratchBuffer, S->output_Q_offset, S->output_Q_inv_scale);
    pInSignal += S->hop_length; 
    // transpose
    if (E_SPECTROGRAM_TRANSPOSE == S->transpose) {
      for (uint32_t j=0 ; j < n_mels ; j++ ){
        pOut[i+S->nb_frames*j]= S->pScratchBuffer[j];
      }
    } else {
      for (uint32_t j=0 ; j < n_mels ; j++ ){
        pOut[S->nb_frames*i+j]= S->pScratchBuffer[j];
      }
    }
  }

}

/**
 * @} end of groupFeature
 */
