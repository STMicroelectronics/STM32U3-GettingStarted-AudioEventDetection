/**
  ******************************************************************************
  * @file    gs_utils.c
  * @author  STMicroelectronics AIS application team
  * @version $Version$
  * @date    $Date$
  * @brief   utilities for audio and sensing getting started
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
#include "dpu.h"
#include "ai_utility.h"
#include "cpu_stats.h"

#include <stdio.h>
/* Private function prototypes -----------------------------------------------*/
#if (CTRL_X_CUBE_AI_SENSOR_TYPE == COM_TYPE_MIC )
static void AdfVumeter(void);
#endif
static void PrintCpuStats(void);
static void ResetCpuStats(void);
/* External functions --------------------------------------------------------*/

/**
* @brief  Displays System settings
* @param  None
* @retval None
*/
void PrintSystemSetting(void)
{
  LogSys("\n\n\r");
  LogSys(SEPARATION_LINE);
  LogSys("        System configuration (%s)\n\r",APP_CONF_STR);
  LogSys(SEPARATION_LINE);
  LogSys("\n\rLog Level: %s\n\n\r", getLogLevelStr(LOG_LEVEL));

  systemSettingLog();
  LogSys("\r\n");

#ifdef USE_HSP
  LogSys("\r\nHSP Acceleration used for Preprocessing\r\n\n");
#endif
  LogSys(SEPARATION_LINE);
  LogSys("\r\n");
  
#if defined (APP_DVFS) || defined (APP_LP)
  LogSys("\r\nLow Power Options:");
#ifdef APP_LP
  LogSys("\r\n DPS");
#endif /* APP_LP */
#ifdef APP_DVFS
  LogSys("\r\n DFVS");
#endif /* APP_DFVS */
  LogSys("\r\n");
#endif /* APP_DVFS || APP_LP */
}

/**
* @brief  Displays The main menu
* @param  None
* @retval None
*/
void PrintMenu(void)
{
  static  uint32_t prev_tick=0;
  
  uint32_t tick = HAL_GetTick();
  if (tick - prev_tick >= MENU_DISP_PERIOD )
  {
    time_stats_store(TIME_STAT_TOT, (float)(tick - prev_tick));
    prev_tick = tick ;
#if (CTRL_X_CUBE_AI_SENSOR_TYPE == COM_TYPE_MIC)
    AdfVumeter();
#else
    printf("                    ");
    fflush(stdout);
#endif      
    PrintCpuStats();
  }
}

/**
* @brief  Displays Classification results
* @param  p_out pointer to classification distribution
* @param  classes pointer to lasses array of string
* @retval None
*/
void PrintAIClassesOutput(float *p_out, const char ** classes )
{
#if (CTRL_X_CUBE_AI_MODEL_OUTPUT_1 == CTRL_AI_CLASS_DISTRIBUTION)
  {
    float max_out = p_out[0];
    int max_idx = 0;
    for(int i = 1; i < CTRL_X_CUBE_AI_MODEL_CLASS_NUMBER; i++)
    {
      if(p_out[i] > max_out)
      {
        max_idx = i;
        max_out = p_out[i];
      }
    }
    if (max_out > CTRL_X_CUBE_AI_OOD_THR)
    {
      LogInfo("\r\n\n               \"%s\" detected after %d s",classes[max_idx],\
                                     (uint32_t)(time_stats_get_runtime())/1000);
    }
    else
    {
      LogInfo("\r\n\n                    Unknown class after %d s",\
                                     (uint32_t)(time_stats_get_runtime())/1000);
    }
    LogInfo("                        \n\r");
    LogInfo(SEPARATION_LINE);
    
    for (int i = 0 ; i < CTRL_X_CUBE_AI_MODEL_CLASS_NUMBER-1; i++)
    {
      LogInfo("%2d ",(int)(p_out[i]*100));
    }
    LogInfo("%2d", (int)(p_out[CTRL_X_CUBE_AI_MODEL_CLASS_NUMBER-1]*100));
    
    LogInfo("\r\x1B[A\x1B[A\x1B[A\x1B[A");
  }
#endif
}

/**
* @brief  Displays Processing start message
* @param  None
* @retval None
*/
void PrintHeader(void)
{
  LogInfo("\r\n");
  LogInfo(SEPARATION_LINE);
  LogInfo("# Start Processing\n\r");
  LogInfo(SEPARATION_LINE);
#if (CTRL_X_CUBE_AI_SENSOR_TYPE == COM_TYPE_MIC)
  LogInfo("| Vu meter          ");
#elif (CTRL_X_CUBE_AI_SENSOR_TYPE == COM_TYPE_ACC)
  LogInfo("                    ");
#endif

#ifdef CPU_STATS
  LogInfo("| Time(s) |  Cpu  |  Pre  |  AI   | Post  |");
#endif
  LogInfo("\033[?25l\r\n"); /* hide cursor (\033[?25l) */
}

/**
* @brief  Displays Processing stop message
* @param  None
* @retval None
*/
void PrintFooter(void)
{
  LogInfo("\r\n\n\n\n\n");
  LogInfo(SEPARATION_LINE);
  LogInfo("# Stop Processing\n\r");
  LogInfo(SEPARATION_LINE);
  LogInfo("\033[?25h\r\n"); /* un-hide cursor (\033[?25h) */
}

/**
* @brief  Init CPU usage statistics
* @param  None
* @retval None
*/
void InitCpuStats(void)
{
  time_stats_init();
  port_dwt_init(); 
  ResetCpuStats();
}
/**
* @brief  Displays CPU usage statistics
* @param  None
* @retval None
*/
void PrintCpuStatsSummary(void)
{
#ifdef CPU_STATS
  uint32_t Pre_cnt = time_stats_get_cnt(TIME_STAT_PRE_PROC);
  uint32_t AI_cnt  = time_stats_get_cnt(TIME_STAT_AI_PROC);
  float Pre_avg    = (Pre_cnt == 0 ) ? 0.0F : \
    time_stats_get_sum(TIME_STAT_PRE_PROC) / Pre_cnt;
  float AI_avg    = (AI_cnt == 0 ) ? 0.0F : \
    time_stats_get_sum(TIME_STAT_AI_PROC) / AI_cnt;

  LogSys("                       CPU timing summary \n\r");
  LogSys(SEPARATION_LINE,"\n\r");
  LogSys("| Statistics                  | Pre-Processing | AI inference |\n\r");
  LogSys(SEPARATION_LINE,"\n\r");
  LogSys("| Number of call              |        %8.0d|      %8.0d|\r\n",\
    time_stats_get_cnt(TIME_STAT_PRE_PROC),\
    time_stats_get_cnt(TIME_STAT_AI_PROC));
  LogSys("| Average (ms)                |        %8.2f|      %8.2f|\r\n",\
    Pre_avg,AI_avg);
  LogSys("| Relative load (%%)          |        %8.2f|      %8.2f|\r\n",\
    100*time_stats_get_sum(TIME_STAT_PRE_PROC) / time_stats_get_runtime(),\
    100*time_stats_get_sum(TIME_STAT_AI_PROC) / time_stats_get_runtime() );
  LogSys(SEPARATION_LINE,"\n\r");
#endif
}

/* Private user code ---------------------------------------------------------*/

static float pre_time_prev, ai_time_prev, post_time_prev, run_time_prev ;

static void ResetCpuStats(void)
{ 
  pre_time_prev  = 0.0F;
  ai_time_prev   = 0.0F;
  post_time_prev = 0.0F;
  run_time_prev  = 0.0F; 
}

static void PrintCpuStats(void)
{ 
  /* display real time statistics */
  float run_time      = time_stats_get_runtime();
  float pre_time      = time_stats_get_sum(TIME_STAT_PRE_PROC);
  float ai_time       = time_stats_get_sum(TIME_STAT_AI_PROC);
  float post_time     = time_stats_get_sum(TIME_STAT_POST_PROC);
  float run_time_dif  = run_time - run_time_prev;
  float pre_time_dif  = pre_time - pre_time_prev;
  float ai_time_dif   = ai_time - ai_time_prev ;
  float post_time_dif = post_time - post_time_prev ;
  run_time_prev       = run_time ;
  pre_time_prev       = pre_time ;
  ai_time_prev        = ai_time ;
  post_time_prev      = post_time_prev;

  LogInfo ("| %-8d|%6.2f%%|%6.2f%%|%6.2f%%|%6.2f%%|\r",\
    (uint32_t)(run_time/1000),\
    100*(pre_time_dif+ai_time_dif+post_time_dif)/(run_time_dif),\
    100*pre_time_dif/run_time_dif,\
    100*ai_time_dif/run_time_dif,\
    100*post_time_dif/run_time_dif);

  fflush(stdout);
}
#if (CTRL_X_CUBE_AI_SENSOR_TYPE == COM_TYPE_MIC )
static void AdfVumeter(void)
{
  float lev_db = (float)(10*(log10(AdfHandle0.Instance->SADSDLVR) - 9.03089986F));
  AdfHandle0.Instance->DFLTISR |= MDF_DFLTISR_SDLVLF;
  // Casting it back to int and removing scale factor for display
  int lev = (int) (lev_db + 10 * 9.03089986F) / 5;
  lev=(lev<0)? 0 : lev;
  lev=(lev>20)? 20 : lev;
  printf("\r\033[42m");
  for (int i = 0 ; i < lev && i < 6 ; i ++)
  {
    printf(" ");
  }
  printf("\033[43m");
  for (int i = 6 ; i < lev && i < 12 ; i ++)
  {
    printf(" ");
  }
  printf("\033[41m");
  for (int i = 12 ; i < lev ; i ++)
  {
    printf(" ");
  }
  printf("\033[0m");
  for (int i = 0 ; i < 20 - lev ; i ++)
  {
    printf(" ");
  }
  fflush(stdout);
}
#endif
