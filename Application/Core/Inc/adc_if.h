// Copyright 2021 Blues Inc.  All rights reserved.
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.

#pragma once

#include "adc.h"

// Battery level in mV
#define BAT_CR2032                  ((uint32_t) 3000)

// Maximum battery level in mV
#define VDD_BAT                     BAT_CR2032

// Minimum battery level in mV
#define VDD_MIN                     1800

// Initializes the ADC input
void SYS_InitMeasurement(void);

// DeInitializes the ADC
void SYS_DeInitMeasurement(void);

// Get the current temperature
// returns temperature in degree Celsius
int16_t SYS_GetTemperatureLevel(void);

// Get the current battery level
// value battery level in linear scale
uint16_t SYS_GetBatteryLevel(void);

