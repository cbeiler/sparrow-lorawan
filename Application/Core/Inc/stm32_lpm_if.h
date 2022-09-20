// Copyright 2021 Blues Inc.  All rights reserved.
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.

// Low Power Manager interface configuration
#pragma once

#include "stm32_lpm.h"

// Enters Low Power Off Mode
void PWR_EnterOffMode(void);

// Exits Low Power Off Mode
void PWR_ExitOffMode(void);

// Enters Low Power Stop Mode
void PWR_EnterStopMode(void);

// Exits Low Power Stop Mode
// Note: Enables the pll at 32MHz
void PWR_ExitStopMode(void);

// Enters Low Power Sleep Mode
// Note: ARM exits the function when waking up
void PWR_EnterSleepMode(void);

// Exits Low Power Sleep Mode
// Note:ARM exits the function when waking up
void PWR_ExitSleepMode(void);
