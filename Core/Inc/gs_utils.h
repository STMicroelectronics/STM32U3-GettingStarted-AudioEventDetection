/**
  ******************************************************************************
  * @file    : gs_utils.h
  * @brief   : Utility Module
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

#ifndef GS_AUDIO_SENSING_UTILS_H
#define GS_AUDIO_SENSING_UTILS_H

extern void PrintHeader(void);
extern void PrintFooter(void);
extern void InitCpuStats(void);
extern void PrintCpuStatsSummary(void);
extern void PrintAIClassesOutput(float *p_out, const char ** classes);
extern void PrintSystemSetting(void);
extern void PrintMenu(void);

#endif /* GS_AUDIO_SENSING_UTILS_H */
