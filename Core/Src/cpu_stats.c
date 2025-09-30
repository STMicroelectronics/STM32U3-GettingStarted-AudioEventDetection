/**
 ******************************************************************************
 * @file    cpu_stats.c
 * @author  STMicroelectronics AIS application team
 * @version $Version$
 * @date    $Date$
 * @brief   utility to collect satistics
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
#include <string.h>
#include <float.h>

#include "app_config.h"
#include "cpu_stats.h"
#include "main.h"

typedef struct
{
  float latest;
  unsigned int cnt;
  float sum;
  float min;
  float max;
}time_stats_t;

static time_stats_t time_stats_buf[TIME_STAT_NB_ENTRIES];
static uint32_t time_from_init_in_ms;

void time_stats_init(void)
{
  /* Clear buffer */
  memset(time_stats_buf, 0, sizeof(time_stats_buf));
  for (int i = 0 ; i < TIME_STAT_NB_ENTRIES ; i++ )
  {
    time_stats_buf[i].min = FLT_MAX;
  }
  time_from_init_in_ms = HAL_GetTick();
}

void time_stats_reset(unsigned int idx)
{
  /* Clear buffer */
  memset(&time_stats_buf[idx], 0, sizeof(time_stats_t));
  time_stats_buf[idx].min = FLT_MAX;
}

void time_stats_store(unsigned int idx, float time_in_ms)
{
  assert_param(idx < TIME_STAT_NB_ENTRIES);
  time_stats_buf[idx].latest = time_in_ms;
  time_stats_buf[idx].sum   += time_in_ms;
  time_stats_buf[idx].cnt++;
  if (time_in_ms < time_stats_buf[idx].min )
  {
    time_stats_buf[idx].min = time_in_ms;
  }
  if (time_in_ms > time_stats_buf[idx].max )
  {
    time_stats_buf[idx].max = time_in_ms;
  }
}
float time_stats_get_runtime(void)
{
  return ((float)(HAL_GetTick()-time_from_init_in_ms));
}

float time_stats_get_sum(unsigned int idx)
{
  return (time_stats_buf[idx].sum);
}
float time_stats_get_avg(unsigned int idx)
{
  return (time_stats_buf[idx].cnt == 0 ) ? 0.0F : time_stats_buf[idx].sum/time_stats_buf[idx].cnt;
}
float time_stats_get_min(unsigned int idx)
{
  return (time_stats_buf[idx].cnt == 0 ) ? 0.0F : time_stats_buf[idx].min;
}
float time_stats_get_max(unsigned int idx)
{
  return (time_stats_buf[idx].max);
}
float time_stats_get_latest(unsigned int idx)
{
  return (time_stats_buf[idx].latest);
}
unsigned int time_stats_get_cnt(unsigned int idx)
{
  return (time_stats_buf[idx].cnt);
}
