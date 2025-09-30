/**
 ******************************************************************************
 * @file    aiTestUtility.c
 * @author  MCD/AIS Team
 * @brief   Utility functions for AI test application
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
 * Description:
 *
 * History:
 *  - v1.0 - initial version (from initial aiSystemPerformance file - v5.1)
 *  - v1.1 - add Arm Compiler 6 support (MDK)
 *  - v1.2 - add io low level code to manage a COM through the STM32 USB CDC profile
 *           enabled with the USE_USB_CDC_CLASS = 1 define.
 *  - v1.3 - Fix compilation issue for H7 dual core
 *  - v2.0 - add Cortex-m0/m0plus support
 *  - v2.1 - add U575ZI id
 *  - v2.2 - clean the way to build/report the STM32 configuration
 *  - v3.1 - use ai_device_adaptor.h
 *
 */

/*
 *  NOTE about the USE_USB_CDC_CLASS support
 *
 *   When USE_USB_CDC_CLASS is set to 1 the code to manage the
 *   STM32 USB_DEVICE and the CDC class should added in the project.
 *
 *   CDC_Receive_FS()/CDC_Control_FS() functions (file: "usb_cdc_if.c")
 *   should be adapted/patched
 *
 *   o CDC_Receive_FS() to handle the received data
 *    ...
 *    static int8_t CDC_Receive_FS(uint8_t* Buf, uint32_t *Len) {
 *      extern void ioPushInUserUsb(uint8_t *pw, uint32_t *len);
 *      ioPushInUserUsb(Buf, Len);
 *      USBD_CDC_SetRxBuffer(&hUsbDeviceFS, &Buf[0]);
 *      USBD_CDC_ReceivePacket(&hUsbDeviceFS);
 *      return (USBD_OK);
 *      }
 *    ...
 *
 *   o CDC_Control_FS() to return the valid data for CDC_GET_LINE_CODING
 *     request.This is requested by the Python module in charge of the
 *     Serial COM with the validation process.
 *
 *      ...
 *      case CDC_GET_LINE_CODING:
 *
 *        if (length == 7) {
 *        *(uint32_t *)pbuf = 115200;
 *        pbuf[4] = 0;
 *        pbuf[5] = 0;
 *        pbuf[6] = 8;
 *        }
 *
 *      break;
 *      ...
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#define USE_RETARGET_IO_FROM_TOOL 1
#define USE_PRINTF_FROM_TOOL      1

#include "ai_utility.h"
#include "logging.h"

#ifdef LC_PRINT
#undef LC_PRINT
#endif
#define LC_PRINT LogSys

#ifndef USE_USB_CDC_CLASS
#define USE_USB_CDC_CLASS 0
#endif

#define MCONF_FPU       (1 << 16)

/* -----------------------------------------------------------------------------
 * Sanity check
 * -----------------------------------------------------------------------------
 */

#if defined(_IS_GCC_COMPILER) && _IS_GCC_COMPILER
#if defined(__ARMCC_VERSION)
#error GCC tool-chain is used, __ARMCC_VERSION should be not defined
#endif
#endif

#if defined(_IS_IAR_COMPILER) && _IS_IAR_COMPILER
#if !defined(__ICCARM__)
#error IAR tool-chain is used, __ICCARM__ should be defined
#endif
#endif

/* -----------------------------------------------------------------------------
 * IO re-target functions
 * -----------------------------------------------------------------------------
 */

#if defined(_IS_GCC_COMPILER) && _IS_GCC_COMPILER
#include <errno.h>
#include <sys/unistd.h> /* STDOUT_FILENO, STDERR_FILENO */

#elif defined(_IS_IAR_COMPILER) && _IS_IAR_COMPILER
#if (__IAR_SYSTEMS_ICC__ <= 8)
/* Temporary workaround - LowLevelIOInterface.h seems not available
   with IAR 7.80.4 */
#define _LLIO_STDIN  0
#define _LLIO_STDOUT 1
#define _LLIO_STDERR 2
#define _LLIO_ERROR ((size_t)-1) /* For __read and __write. */
#else
#include <LowLevelIOInterface.h> /* _LLIO_STDOUT, _LLIO_STDERR */
#endif

#elif defined(_IS_ACx_COMPILER) && _IS_ACx_COMPILER

#endif


 int ioRawGetUint8(uint8_t *c, uint32_t timeout)
 {
   return port_io_get(c, timeout) == true?1:-1;
 }


bool ioRawWriteBuffer(uint8_t *buff, int count)
{
  return port_io_write(buff, count);
}

bool ioRawReadBuffer(uint8_t *buff, int count)
{
  return port_io_read(buff, count);
}



/* -----------------------------------------------------------------------------
 * Log functions
 * -----------------------------------------------------------------------------
 */

static char bfr[20+1];

char* uint64ToDecimal(uint64_t v)
{ /* see https://www.eevblog.com/forum/microcontrollers/printf-uint64_t-with-arm-gcc/ */
  char* p = bfr + sizeof(bfr);
  *(--p) = '\0';
  for (bool first = true; v || first; first = false) {
    const uint32_t digit = v % 10;
    const char c = '0' + digit;
    *(--p) = c;
    v = v / 10;
  }
  return p;
}

void uint32_to_str(uint32_t val, char* dest, int max)
{
  int i;
  dest[0] = ' ';
  for (i = 0; i < 8; i++)
  {
    int off = (val >> ((7 - i) * 4)) & 0xF;
    max -= 1;
    if (max < 2)
      break;
    if (off > 9) {
      dest[i+1] = 'A' + (off - 10);
    }
    else {
      dest[i+1] = '0' + off;
    }
  }
  dest[i+1] = 0;
}



/* -----------------------------------------------------------------------------
 * HEAP Monitor functions
 * -----------------------------------------------------------------------------
 */

#if defined(_IS_GCC_COMPILER) && _IS_GCC_COMPILER

#define MAGIC_MALLOC_NUMBER 0xefdcba98

struct io_malloc io_malloc;

void* __real_malloc(size_t bytes);
void __real_free(void *ptr);

void* __wrap_malloc(size_t bytes)
{
  uint8_t *ptr;

  io_malloc.cfg |= 1 << 1;

  /* ensure alignment for magic number */
  bytes = (bytes + 3) & ~3;

  /* add 2x32-bit for size and magic  number */
  ptr = (uint8_t*)__real_malloc(bytes + 8);

  /* remember size */
  if (ptr) {
    *((uint32_t*)ptr) = bytes;
    *((uint32_t*)(ptr + 4 + bytes)) = MAGIC_MALLOC_NUMBER;
  }

  if ((ptr) && (io_malloc.cfg & 1UL)) {
    io_malloc.alloc_req++;
    io_malloc.alloc += bytes;

    io_malloc.used += bytes;

    if (io_malloc.used > io_malloc.max) {
      io_malloc.max = io_malloc.used;
    }

#if _IO_MALLOC_TRACK_MODE == 1
    io_malloc.a_ptr[io_malloc.a_idx] = (ptr + 4);
    io_malloc.a_s[io_malloc.a_idx] = bytes;
    io_malloc.a_idx++;

    if (io_malloc.a_idx >= _IO_MALLOC_TRACK_MODE)
      io_malloc.a_idx = 0;
#endif
  }
  return ptr?(ptr + 4):NULL;
}

void __wrap_free(void *ptr)
{
  uint8_t* p;
  uint32_t bytes;

  io_malloc.cfg |= 1 << 2;

  if (!ptr)
    return;

  p = (uint8_t*)ptr - 4;
  bytes = *((uint32_t*)p);

  if (*((uint32_t*)(p + 4 + bytes)) == MAGIC_MALLOC_NUMBER) {
    *((uint32_t*)(p + 4 + bytes)) = 0;
  }

  if (io_malloc.cfg & 1UL) {
    io_malloc.free_req++;
    io_malloc.free += bytes;
    io_malloc.used -= bytes;

#if _IO_MALLOC_TRACK_MODE == 1
    io_malloc.f_ptr[io_malloc.f_idx] = ptr;
    io_malloc.f_s[io_malloc.f_idx] = bytes;
    io_malloc.f_idx++;

    if (io_malloc.f_idx >= _IO_MALLOC_TRACK_MODE)
      io_malloc.f_idx = 0;
#endif
  }
  __real_free(p);
}

#endif


/* -----------------------------------------------------------------------------
 * STACK Monitor functions
 * -----------------------------------------------------------------------------
 */

#if defined(_IS_GCC_COMPILER) && _IS_GCC_COMPILER
extern uint32_t _estack[];

#elif defined(_IS_IAR_COMPILER) && _IS_IAR_COMPILER
extern int CSTACK$$Limit;
extern int CSTACK$$Base;

#endif

struct io_stack io_stack;

#if defined(_APP_STACK_MONITOR_) && _APP_STACK_MONITOR_ == 1 && defined(_IS_GCC_COMPILER) && _IS_GCC_COMPILER

void stackMonInit(uint32_t ctrl, uint32_t cstack, uint32_t msize)
{
  memset(&io_stack, 0, sizeof(struct io_stack));

  /* Reading ARM Core registers */
  io_stack.ctrl = ctrl;
  io_stack.cstack = cstack;

#if defined(_IS_GCC_COMPILER) && _IS_GCC_COMPILER
  io_stack.estack = (uint32_t)_estack;
  io_stack.bstack = io_stack.estack - msize;
  io_stack.mstack_size = msize;
#elif defined(_IS_IAR_COMPILER) && _IS_IAR_COMPILER
  io_stack.estack = (uint32_t)&CSTACK$$Limit;
  io_stack.bstack = (uint32_t)&CSTACK$$Base;
  io_stack.mstack_size = (uint32_t)&CSTACK$$Limit - (uint32_t)&CSTACK$$Base;
#elif defined(_IS_HTC_COMPILER) && _IS_HTC_COMPILER
  (void)msize;
#endif

  /* Check that MSP is the active stack */
  if (io_stack.ctrl & CONTROL_SPSEL_Msk) {
    LC_PRINT("E: MSP is not the active stack (stack monitoring is disabled)\r\n");
    io_stack.stack_mon = false;
  } else
    io_stack.stack_mon = true;

  /* Calculating used stack before test */
  io_stack.ustack_size = io_stack.estack - io_stack.cstack;

  if ((io_stack.stack_mon) && (io_stack.ustack_size > io_stack.mstack_size)) {
    LC_PRINT("E: !stack overflow detected %d > %d\r\n", (int)io_stack.ustack_size,
        (int)io_stack.mstack_size);
    io_stack.stack_mon = false;
  }
}

#endif

/* -----------------------------------------------------------------------------
 * Device HW-setting functions
 * -----------------------------------------------------------------------------
 */

struct cyclesCount cyclesCount;

int dwtCyclesToTime(uint64_t clks, struct dwtTime *t)
{
  if (!t)
    return -1;
  uint32_t fcpu = port_hal_get_frequency();
  uint64_t s  = clks / fcpu;
  uint64_t ms = (clks * 1000) / fcpu;
  uint64_t us = (clks * 1000 * 1000) / fcpu;
  ms -= (s * 1000);
  us -= (ms * 1000 + s * 1000000);
  t->fcpu = fcpu;
  t->s = s;
  t->ms = ms;
  t->us = us;
  return 0;
}

float dwtCyclesToFloatMs(uint64_t clks)
{
  float res;
  float fcpu = (float)port_hal_get_frequency();
  res = ((float)clks * (float)1000.0) / fcpu;
  return res;
}

static const char *devIdToStr(uint16_t dev_id)
{
  const char *str;
  switch (dev_id)
  {
#if defined(STM32U3)
    case 0x42A: str = "STM32U3x5";
      break;
    case 0x42B: str = "STM32U3x6";
      break;
    case 0x454: str = "STM32U3x5";
      break;
#endif
    default:    str = "UNKNOWN";
  }
  return str;
}

__STATIC_INLINE const char* bitToStr(uint32_t val)
{
  if (val)
    return "True";
  else
    return "False";
}

void getSysConf(struct mcu_conf *conf)
{
  if (!conf)
    return;

  conf->sclk = port_hal_get_cpu_freq();
  conf->hclk = port_hal_get_sys_freq();
  conf->conf = getFlashCacheConf();
  conf->devid = port_hal_get_dev_id();
  conf->revid = port_hal_get_rev_id();

}


uint32_t getFlashCacheConf(void)
{
  uint32_t mconf = 0;

#if (__FPU_PRESENT == 1) && __FPU_USED
  mconf |= MCONF_FPU;  /* FPU present and used */
#endif

  return mconf;
}


void systemSettingLog(void)
{
  struct mcu_conf sys_conf;

  /* Display Complier version */
#if defined(_IS_GCC_COMPILER) && _IS_GCC_COMPILER
  LC_PRINT("Compiled with GCC %d.%d.%d\r\n", __GNUC__, __GNUC_MINOR__,
      __GNUC_PATCHLEVEL__);
#elif defined(_IS_IAR_COMPILER) && _IS_IAR_COMPILER
  LC_PRINT("Compiled with IAR %d (build %d)\r\n", __IAR_SYSTEMS_ICC__,
      __BUILD_NUMBER__
  );
#endif

  /* Display Device configuration */
  getSysConf(&sys_conf);

  LC_PRINT(DEVICE_FAMILY " device configuration...\r\n");

  LC_PRINT(" Device       : DevID:0x%04x (%s) RevID:0x%04x\r\n",
      (int)sys_conf.devid,
      devIdToStr(sys_conf.devid),
      (int)sys_conf.revid );

#if (__FPU_PRESENT == 1)
  LC_PRINT(" Core Arch.   : M%d - %s %s\r\n", __CORTEX_M, "FPU ", ((__FPU_USED) ? "used" : "not used!"));
#else
  LC_PRINT(" Core Arch.   : R%d - %s\r\n", __CORTEX_R, "no FPU");
#endif

  LC_PRINT(" HAL version  : 0x%08x\r\n", (int)port_hal_get_hal_version());
  LC_PRINT(" SYSCLK clock : %u MHz\r\n", (int)sys_conf.sclk/1000000);
  LC_PRINT(" HCLK clock   : %u MHz\r\n", (int)sys_conf.hclk/1000000);

#if defined(STM32U3)
  LC_PRINT(" ICACHE       : %s\r\n", bitToStr(READ_BIT(ICACHE->CR, ICACHE_CR_EN)));
#endif


}

