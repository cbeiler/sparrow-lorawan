// Copyright 2021 Blues Inc.  All rights reserved.
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.

#pragma once

#include "stm32_timer.h"
#include "stm32_systime.h"

// Public

// Init RTC hardware
// Status based on @ref UTIL_TIMER_Status_t
UTIL_TIMER_Status_t TIMER_IF_Init(void);

// Set the alarm
// Note: The alarm is set at timeout from timer Reference (TimerContext)
// timeout Duration of the Timer in ticks
// return Status based on @ref UTIL_TIMER_Status_t
UTIL_TIMER_Status_t TIMER_IF_StartTimer(uint32_t timeout);

// Stop the Alarm
// Return Status based on @ref UTIL_TIMER_Status_t
UTIL_TIMER_Status_t TIMER_IF_StopTimer(void);

// set timer Reference (TimerContext)
// Return  Timer Reference Value in  Ticks
uint32_t TIMER_IF_SetTimerContext(void);

// Get the RTC timer Reference
// return Timer Value in  Ticks
uint32_t TIMER_IF_GetTimerContext(void);

// Get the timer elapsed time since timer Reference (TimerContext) was set
// Return RTC Elapsed time in ticks
uint32_t TIMER_IF_GetTimerElapsedTime(void);

// Get the timer value
// Return RTC Timer value in ticks
uint32_t TIMER_IF_GetTimerValue(void);

// Return the minimum timeout in ticks the RTC is able to handle
// Return minimum value for a timeout in ticks
uint32_t TIMER_IF_GetMinimumTimeout(void);

// a delay of delay ms by polling RTC
// delay in ms
void TIMER_IF_DelayMs(uint32_t delay);

// converts time in ms to time in ticks
// [in] timeMilliSec time in milliseconds
// Return time in timer ticks
uint32_t TIMER_IF_Convert_ms2Tick(uint32_t timeMilliSec);

// converts time in ticks to time in ms
// [in] tick time in timer ticks
// Return time in timer milliseconds
uint32_t TIMER_IF_Convert_Tick2ms(uint32_t tick);

// Get rtc time
// [out] subSeconds in ticks
// Return time seconds
uint32_t TIMER_IF_GetTime(uint16_t *subSeconds);

// write seconds in backUp register
// Note: Used to store seconds difference between RTC time and Unix time
// [in] Seconds time in seconds
void TIMER_IF_BkUp_Write_Seconds(uint32_t Seconds);

// reads seconds from backUp register
// Note: Used to store seconds difference between RTC time and Unix time
uint32_t TIMER_IF_BkUp_Read_Seconds(void);

// writes SubSeconds in backUp register
// Note: Used to store SubSeconds difference between RTC time and Unix time
// [in] SubSeconds time in SubSeconds
void TIMER_IF_BkUp_Write_SubSeconds(uint32_t SubSeconds);

// reads SubSeconds from backUp register
// Note: Used to store SubSeconds difference between RTC time and Unix time
// Return Time in SubSeconds
uint32_t TIMER_IF_BkUp_Read_SubSeconds(void);
