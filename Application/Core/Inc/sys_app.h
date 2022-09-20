// Copyright 2021 Blues Inc.  All rights reserved.
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.

#pragma once

#include "stdint.h"
#include "config_sys.h"

// initialize the system (dbg pins, trace, mbmux, sys timer, LPM, ...)
void SystemApp_Init(void);

// callback to get the battery level in % of full charge (254 full charge, 0 no charge)
// Returns battery level
uint8_t GetBatteryLevel(void);

// callback to get the current temperature in the MCU
// Returns temperature level
uint16_t GetTemperatureLevel(void);

// callback to get the board 64 bits unique ID
// id unique ID
void GetUniqueId(uint8_t *id);

// callback to get the board 32 bits unique ID (LSB)
// devAddr Device Address
uint32_t GetDevAddr(void);

