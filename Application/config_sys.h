// Copyright 2021 Blues Inc.  All rights reserved.
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.

// Applicative configuration, e.g. : debug, trace, low power, sensors
#pragma once

// Enables trace output and input on an attached serial terminal.  Note that
// if using LPUART1 this costs xxx nA, and using USART2 it's crazy expensive.
// Do NOT ship battery powered products with this enabled.
#define DEBUGGER_ON                                     true

// Enable tracing
#define DEBUGGER_ON_USART2                              false
#define DEBUGGER_ON_LPUART1                             true

// Special GPIO trace methods when working on radio code
#define DEBUGGER_RADIO_DBG_GPIO                         false

// Disable Low Power mode
// Note: 0: LowPowerMode enabled. MCU enters stop2 mode, 1: LowPowerMode disabled. MCU enters sleep mode only
#define LOW_POWER_DISABLE                               false

// Temperature and pressure values are retrieved from sensors shield
// (instead of sending dummy values). It requires MEMS IKS shield
#define SENSOR_ENABLED              0

// Verbose level for all trace logs
#define VERBOSE_LEVEL               VLEVEL_M

// Enable trace logs
#define APP_LOG_ENABLED             1
