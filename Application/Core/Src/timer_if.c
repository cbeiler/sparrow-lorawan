// Copyright 2021 Blues Inc.  All rights reserved.
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.

#include <math.h>
#include "timer_if.h"
#include "main.h"   // for STM32CubeMX generated RTC_N_PREDIV_S and RTC_N_PREDIV_A
#include "rtc.h"
#include "stm32_lpm.h"
#include "utilities_def.h"
#include "stm32wlxx_ll_rtc.h"

// Handle
extern RTC_HandleTypeDef hrtc;

// Timer driver callbacks handler
const UTIL_TIMER_Driver_s UTIL_TimerDriver = {
    TIMER_IF_Init,
    NULL,
    TIMER_IF_StartTimer,
    TIMER_IF_StopTimer,
    TIMER_IF_SetTimerContext,
    TIMER_IF_GetTimerContext,
    TIMER_IF_GetTimerElapsedTime,
    TIMER_IF_GetTimerValue,
    TIMER_IF_GetMinimumTimeout,
    TIMER_IF_Convert_ms2Tick,
    TIMER_IF_Convert_Tick2ms,
};

// SysTime driver callbacks handler
const UTIL_SYSTIM_Driver_s UTIL_SYSTIMDriver = {
    TIMER_IF_BkUp_Write_Seconds,
    TIMER_IF_BkUp_Read_Seconds,
    TIMER_IF_BkUp_Write_SubSeconds,
    TIMER_IF_BkUp_Read_SubSeconds,
    TIMER_IF_GetTime,
};

// Minimum timeout delay of Alarm in ticks
#define MIN_ALARM_DELAY    3

// Backup seconds register
#define RTC_BKP_SECONDS    RTC_BKP_DR0

// Backup subseconds register
#define RTC_BKP_SUBSECONDS RTC_BKP_DR1

// Backup msbticks register
#define RTC_BKP_MSBTICKS   RTC_BKP_DR2

// Map UTIL_TIMER_IRQ can be overridden in utilities_conf.h to Map on Task rather then Isr
#ifndef UTIL_TIMER_IRQ_MAP_INIT
#define UTIL_TIMER_IRQ_MAP_INIT()
#endif

#ifndef UTIL_TIMER_IRQ_MAP_PROCESS
#define UTIL_TIMER_IRQ_MAP_PROCESS() UTIL_TIMER_IRQ_Handler()
#endif

// Debug
//#define RTIF_DEBUG

#ifdef RTIF_DEBUG
#include "sys_app.h" // for app_log

// Post the RTC log string format to the circular queue for printing in using the polling mode
#define TIMER_IF_DBG_PRINTF(...) do{ {UTIL_ADV_TRACE_COND_FSend(VLEVEL_ALWAYS, T_REG_OFF, TS_OFF, __VA_ARGS__);} }while(0);
#else
// not used
#define TIMER_IF_DBG_PRINTF(...)
#endif

// Vars
static bool RTC_Initialized = false;
static uint32_t RtcTimerContext = 0;

// Forwards
static inline uint32_t GetTimerTicks(void);
static void TIMER_IF_BkUp_Write_MSBticks(uint32_t MSBticks);
static uint32_t TIMER_IF_BkUp_Read_MSBticks(void);

// Init interface
UTIL_TIMER_Status_t TIMER_IF_Init(void)
{
    UTIL_TIMER_Status_t ret = UTIL_TIMER_OK;
    if (RTC_Initialized == false) {
        hrtc.IsEnabled.RtcFeatures = UINT32_MAX;
        MX_RTC_Init();
        TIMER_IF_StopTimer();
        // DeActivate the Alarm A enabled by STM32CubeMX during MX_RTC_Init()
        HAL_RTC_DeactivateAlarm(&hrtc, RTC_ALARM_A);
        // overload RTC feature enable
        hrtc.IsEnabled.RtcFeatures = UINT32_MAX;

        // Enable Direct Read of the calendar registers (not through Shadow)
        HAL_RTCEx_EnableBypassShadow(&hrtc);
        // Initialize MSB ticks
        TIMER_IF_BkUp_Write_MSBticks(0);

        TIMER_IF_SetTimerContext();

        // Register a task to associate to UTIL_TIMER_Irq() interrupt
        UTIL_TIMER_IRQ_MAP_INIT();

        RTC_Initialized = true;
    }

    return ret;
}

// Start a timer
UTIL_TIMER_Status_t TIMER_IF_StartTimer(uint32_t timeout)
{
    UTIL_TIMER_Status_t ret = UTIL_TIMER_OK;
    RTC_AlarmTypeDef sAlarm = {0};

    // Stop timer if one is already started
    TIMER_IF_StopTimer();
    timeout += RtcTimerContext;

    TIMER_IF_DBG_PRINTF("Start timer: time=%d, alarm=%d\n\r",  GetTimerTicks(), timeout);

    // starts timer
    sAlarm.BinaryAutoClr = RTC_ALARMSUBSECONDBIN_AUTOCLR_NO;
    sAlarm.AlarmTime.SubSeconds = UINT32_MAX - timeout;
    sAlarm.AlarmMask = RTC_ALARMMASK_NONE;
    sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDBINMASK_NONE;
    sAlarm.Alarm = RTC_ALARM_A;
    if (HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BCD) != HAL_OK) {
        Error_Handler();
    }

    return ret;
}

// Stop timer
UTIL_TIMER_Status_t TIMER_IF_StopTimer(void)
{
    UTIL_TIMER_Status_t ret = UTIL_TIMER_OK;

    // Clear RTC Alarm Flag
    __HAL_RTC_ALARM_CLEAR_FLAG(&hrtc, RTC_FLAG_ALRAF);

    // Disable the Alarm A interrupt
    HAL_RTC_DeactivateAlarm(&hrtc, RTC_ALARM_A);

    // overload RTC feature enable
    hrtc.IsEnabled.RtcFeatures = UINT32_MAX;

    return ret;
}

// Set up the timer context
uint32_t TIMER_IF_SetTimerContext(void)
{
    RtcTimerContext = GetTimerTicks();
    TIMER_IF_DBG_PRINTF("TIMER_IF_SetTimerContext=%d\n\r", RtcTimerContext);
    return RtcTimerContext;
}

// Get the context
uint32_t TIMER_IF_GetTimerContext(void)
{
    TIMER_IF_DBG_PRINTF("TIMER_IF_GetTimerContext=%d\n\r", RtcTimerContext);
    return RtcTimerContext;
}

// Get elapsed time
uint32_t TIMER_IF_GetTimerElapsedTime(void)
{
    uint32_t ret = 0;
    ret = ((uint32_t)(GetTimerTicks() - RtcTimerContext));
    return ret;
}

// Get the timer
uint32_t TIMER_IF_GetTimerValue(void)
{
    uint32_t ret = 0;
    if (RTC_Initialized == true) {
        ret = GetTimerTicks();
    }
    return ret;
}

// Get the minimum valid timeout
uint32_t TIMER_IF_GetMinimumTimeout(void)
{
    uint32_t ret = 0;
    ret = (MIN_ALARM_DELAY);
    return ret;
}

// Convert milliseconds to ticks
uint32_t TIMER_IF_Convert_ms2Tick(uint32_t timeMilliSec)
{
    uint32_t ret = 0;
    ret = ((uint32_t)((((uint64_t) timeMilliSec) << RTC_N_PREDIV_S) / 1000));
    return ret;
}

// Convert ticks to milliseconds
uint32_t TIMER_IF_Convert_Tick2ms(uint32_t tick)
{
    uint32_t ret = 0;
    ret = ((uint32_t)((((uint64_t)(tick)) * 1000) >> RTC_N_PREDIV_S));
    return ret;
}

// Delay the specified number of ms
void TIMER_IF_DelayMs(uint32_t delay)
{
    uint32_t delayTicks = TIMER_IF_Convert_ms2Tick(delay);
    uint32_t timeout = GetTimerTicks();
    while (((GetTimerTicks() - timeout)) < delayTicks) {
        __NOP();
    }
}

// Alarm A
void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{
    UTIL_TIMER_IRQ_MAP_PROCESS();
}

// Event
void HAL_RTCEx_SSRUEventCallback(RTC_HandleTypeDef *hrtc)
{
    // called every 48 days with 1024 ticks per seconds
    TIMER_IF_DBG_PRINTF(">>Handler SSRUnderflow at %d\n\r", GetTimerTicks());
    // Increment MSBticks
    uint32_t MSB_ticks = TIMER_IF_BkUp_Read_MSBticks();
    TIMER_IF_BkUp_Write_MSBticks(MSB_ticks + 1);
}

// Get time
uint32_t TIMER_IF_GetTime(uint16_t *mSeconds)
{
    uint32_t seconds = 0;
    uint64_t ticks;
    uint32_t timerValueLsb = GetTimerTicks();
    uint32_t timerValueMSB = TIMER_IF_BkUp_Read_MSBticks();
    ticks = (((uint64_t) timerValueMSB) << 32) + timerValueLsb;
    seconds = (uint32_t)(ticks >> RTC_N_PREDIV_S);
    ticks = (uint32_t) ticks & RTC_PREDIV_S;
    *mSeconds = TIMER_IF_Convert_Tick2ms(ticks);
    return seconds;
}

// Write the backup
void TIMER_IF_BkUp_Write_Seconds(uint32_t Seconds)
{
    HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_SECONDS, Seconds);
}

// Write subseconds backup
void TIMER_IF_BkUp_Write_SubSeconds(uint32_t SubSeconds)
{
    HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_SUBSECONDS, SubSeconds);
}

// Read secs from backup
uint32_t TIMER_IF_BkUp_Read_Seconds(void)
{
    uint32_t ret = 0;
    ret = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_SECONDS);
    return ret;
}

// Read backup subsecs
uint32_t TIMER_IF_BkUp_Read_SubSeconds(void)
{
    uint32_t ret = 0;
    ret = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_SUBSECONDS);
    return ret;
}

// Writes MSBticks to backup register
// Absolute RTC time in tick is (MSBticks)<<32 + (32bits binary counter)
// Note: MSBticks incremented every time the 32bits RTC timer wraps around (~44days)
// [in] MSBticks
static void TIMER_IF_BkUp_Write_MSBticks(uint32_t MSBticks)
{
    HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_MSBTICKS, MSBticks);
}

// Reads MSBticks from backup register
// Absolute RTC time in tick is (MSBticks)<<32 + (32bits binary counter)
// Note: MSBticks incremented every time the 32bits RTC timer wraps around (~44days)
// Returns MSBticks
static uint32_t TIMER_IF_BkUp_Read_MSBticks(void)
{
    uint32_t MSBticks;
    MSBticks = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_MSBTICKS);
    return MSBticks;
}

// Get rtc timer Value in rtc tick
// Return val the rtc timer value (upcounting)
static inline uint32_t GetTimerTicks(void)
{
    return (UINT32_MAX - LL_RTC_TIME_GetSubSecond(RTC));
}

