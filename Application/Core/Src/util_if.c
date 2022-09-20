// Copyright 2021 Blues Inc.  All rights reserved.
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.

#include "main.h"
#include "stm32_seq.h"
#include "stm32_lpm.h"
#include "stm32_timer.h"
#include "stm32_systime.h"
#include "utilities_def.h"
#include "stm32_adv_trace.h"

// Forwards
#define MAX_TS_SIZE     ((int) 16)
static void TimestampNow(uint8_t *buff, uint16_t *size);
static void tiny_snprintf_like(char *buf, uint32_t maxsize, const char *strFormat, ...);

// Initializes ST's utility packages
void MX_UTIL_Init(void)
{

    // Initialises timer and RTC
    UTIL_TIMER_Init();

    // Initialize the trace terminal
    UTIL_ADV_TRACE_Init();
    UTIL_ADV_TRACE_RegisterTimeStampFunction(TimestampNow);
    UTIL_ADV_TRACE_SetVerboseLevel(VERBOSE_LEVEL);

    // Init low power manager
    UTIL_LPM_Init();

    // Disable Stand-by mode
    UTIL_LPM_SetOffMode((1 << CFG_LPM_APPLI_Id), UTIL_LPM_DISABLE);

#if defined (LOW_POWER_DISABLE) && (LOW_POWER_DISABLE == 1)
    // Disable Stop Mode
    UTIL_LPM_SetStopMode((1 << CFG_LPM_APPLI_Id), UTIL_LPM_DISABLE);
#elif !defined (LOW_POWER_DISABLE)
#error LOW_POWER_DISABLE not defined
#endif // LOW_POWER_DISABLE

}

// Redefines __weak function in stm32_seq.c such to enter low power
void UTIL_SEQ_Idle(void)
{
    UTIL_LPM_EnterLowPower();
}

// Returns sec and msec based on the systime in use
// buff to update with timestamp
// size of updated buffer
static void TimestampNow(uint8_t *buff, uint16_t *size)
{
    SysTime_t curtime = SysTimeGet();
    tiny_snprintf_like((char *)buff, MAX_TS_SIZE, "%ds%03d:", curtime.Seconds, curtime.SubSeconds);
    *size = strlen((char *)buff);
}

// Disable StopMode when traces need to be printed
void UTIL_ADV_TRACE_PreSendHook(void)
{
    UTIL_LPM_SetStopMode((1 << CFG_LPM_UART_TX_Id), UTIL_LPM_DISABLE);
}

// Re-enable StopMode when traces have been printed
void UTIL_ADV_TRACE_PostSendHook(void)
{
    UTIL_LPM_SetStopMode((1 << CFG_LPM_UART_TX_Id), UTIL_LPM_ENABLE);
}

// UTIL_ADV_TRACE_VSNPRINTF wrapper
static void tiny_snprintf_like(char *buf, uint32_t maxsize, const char *strFormat, ...)
{
    va_list vaArgs;
    va_start(vaArgs, strFormat);
    UTIL_ADV_TRACE_VSNPRINTF(buf, maxsize, strFormat, vaArgs);
    va_end(vaArgs);
}
