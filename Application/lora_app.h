// Copyright 2021 Blues Inc.  All rights reserved.
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.

#pragma once

#include "config.h"

// Defines the application data transmission duty cycle. 10s, value in [ms].
#define APP_TX_DUTYCYCLE                            10000

// LoRaWAN User application port
// Note: do not use 224. It is reserved for certification
#define LORAWAN_USER_APP_PORT                       2

// LoRaWAN Switch class application port
// Note: do not use 224. It is reserved for certification
#define LORAWAN_SWITCH_CLASS_PORT                   3

// LoRaWAN default endNode class port
#define LORAWAN_DEFAULT_CLASS                       CLASS_A

// LoRaWAN default confirm state
#define LORAWAN_DEFAULT_CONFIRMED_MSG_STATE         LORAMAC_HANDLER_UNCONFIRMED_MSG

// LoRaWAN Adaptive Data Rate
// Note: Please note that when ADR is enabled the end-device should be static
#define LORAWAN_ADR_STATE                           LORAMAC_HANDLER_ADR_ON

// LoRaWAN Default data Rate Data Rate
// Note: The LORAWAN_DEFAULT_DATA_RATE is used only when LORAWAN_ADR_STATE is disabled
#define LORAWAN_DEFAULT_DATA_RATE                   DR_0

// LoRaWAN default activation type
#define LORAWAN_DEFAULT_ACTIVATION_TYPE             ACTIVATION_TYPE_OTAA

// User application data buffer size
#define LORAWAN_APP_DATA_BUFFER_MAX_SIZE            242

// Default Unicast ping slots periodicity
// Note: periodicity is equal to 2^LORAWAN_DEFAULT_PING_SLOT_PERIODICITY seconds
//       example: 2^3 = 8 seconds. The end-device will open an Rx slot every 8 seconds.
#define LORAWAN_DEFAULT_PING_SLOT_PERIODICITY       4

// Public
void LoRaWAN_Init(void);

