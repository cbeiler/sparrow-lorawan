// Copyright 2021 Blues Inc.  All rights reserved.
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.

// To give info to the application about LoRaWAN configuration
#pragma once

#include <stdint.h>
// Exported types

// To give info to the application about LoraWAN capability
// it can depend how it has been compiled (e.g. compiled regions ...)
// Params should be better uint32_t foe easier alignment with info_table concept
typedef struct {
    uint32_t ActivationMode;  /*!< 1: ABP, 2 : OTAA, 3: ABP & OTAA   */
    uint32_t Region;   /*!< Combination of regions compiled on MW  */
    uint32_t ClassB;   /*!< 0: not compiled in Mw, 1 : compiled in MW  */
    uint32_t Kms;      /*!< 0: not compiled in Mw, 1 : compiled in MW  */
} LoraInfo_t;

// Public
// Initialize the LoraInfo table
void LoraInfo_Init(void);

// Returns the pointer to the LoraInfo capabilities table
LoraInfo_t *LoraInfo_GetPtr(void);
