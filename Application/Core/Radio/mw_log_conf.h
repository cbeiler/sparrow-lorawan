// Copyright 2021 Blues Inc.  All rights reserved.
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.

#pragma once

#include "stm32_adv_trace.h"

// Constants
#define MW_LOG_ENABLED

// Macros
#ifdef MW_LOG_ENABLED
#define MW_LOG(TS,VL, ...)   do{ {UTIL_ADV_TRACE_COND_FSend(VL, T_REG_OFF, TS, __VA_ARGS__);} }while(0)
#else
#define MW_LOG(TS,VL, ...)
#endif
