// Copyright 2021 Blues Inc.  All rights reserved.
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.

#pragma once

#include "main.h"
#include "config_sys.h"

// bin.c
void binBegin(uint8_t *buf, uint8_t len, uint8_t flagbytes);
uint8_t binEnd(void);
void binAppendBool(bool value);
void binAppendByte(uint8_t databyte);
void binAppendInt8(int8_t value);
void binAppendInt16(int16_t value);
void binAppendInt24(int32_t value);
void binAppendInt32(int32_t value);
void binAppendInt64(int64_t value);
void binAppendString(char *p, uint32_t len);
void binAppendBytes(uint8_t *data, uint32_t datalen, uint32_t templateLen);
void binAppendReal16(float number);
void binAppendReal32(float number);
void binAppendReal64(double number);

// led.c
void ledJoined(void);
void ledIndicateJoinInProgress(bool on);
void ledToggleJoin(void);
void ledIndicateReceiveInProgress(bool on);
void ledIndicateTransmitInProgress(bool on);

// App logging macros
#include "stm32_adv_trace.h"
#define APP_PPRINTF(...)  do{ } while( UTIL_ADV_TRACE_OK \
                              != UTIL_ADV_TRACE_COND_FSend(VLEVEL_ALWAYS, T_REG_OFF, TS_OFF, __VA_ARGS__) ) /* Polling Mode */
#define APP_TPRINTF(...)   do{ {UTIL_ADV_TRACE_COND_FSend(VLEVEL_ALWAYS, T_REG_OFF, TS_ON, __VA_ARGS__);} }while(0); /* with timestamp */
#define APP_PRINTF(...)   do{ {UTIL_ADV_TRACE_COND_FSend(VLEVEL_ALWAYS, T_REG_OFF, TS_OFF, __VA_ARGS__);} }while(0);
#if defined (APP_LOG_ENABLED) && (APP_LOG_ENABLED == 1)
#define APP_LOG(TS,VL,...)   do{ {UTIL_ADV_TRACE_COND_FSend(VL, T_REG_OFF, TS, __VA_ARGS__);} }while(0);
#elif defined (APP_LOG_ENABLED) && (APP_LOG_ENABLED == 0) /* APP_LOG disabled */
#define APP_LOG(TS,VL,...)
#else
#error "APP_LOG_ENABLED not defined or out of range <0,1>"
#endif
