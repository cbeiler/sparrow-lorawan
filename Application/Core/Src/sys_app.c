// Copyright 2021 Blues Inc.  All rights reserved.
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.

#include <stdio.h>
#include "sys_app.h"
#include "adc_if.h"
#include "stm32_seq.h"
#include "stm32_lpm.h"
#include "timer_if.h"
#include "utilities_def.h"
#include "main.h"

// Init SystemApp subsystem
void SystemApp_Init(void)
{
    SYS_InitMeasurement();
}

// Get battery voltage in a way required by LoRaWAN
#define LORAWAN_MAX_BAT   254
uint8_t GetBatteryLevel(void)
{
    uint8_t batteryLevel = 0;
    uint16_t batteryLevelmV = (uint16_t) SYS_GetBatteryLevel();
    // Convert battery level from mV to linear scale: 1 (very low) to 254 (fully charged)
    if (batteryLevelmV > VDD_BAT) {
        batteryLevel = LORAWAN_MAX_BAT;
    } else if (batteryLevelmV < VDD_MIN) {
        batteryLevel = 0;
    } else {
        batteryLevel = (((uint32_t)(batteryLevelmV - VDD_MIN) * LORAWAN_MAX_BAT) / (VDD_BAT - VDD_MIN));
    }
    return batteryLevel;  // 1 (very low) to 254 (fully charged)
}

// Get temperature in a way required by LoRaWAN
uint16_t GetTemperatureLevel(void)
{
    uint16_t temperatureLevel = 0;
    temperatureLevel = (uint16_t)(SYS_GetTemperatureLevel() / 256);
    return temperatureLevel;
}

// Get a unique device ID
void GetUniqueId(uint8_t *id)
{
    uint32_t val = 0;
    val = LL_FLASH_GetUDN();
    if (val == 0xFFFFFFFF) { // Normally this should not happen
        uint32_t ID_1_3_val = HAL_GetUIDw0() + HAL_GetUIDw2();
        uint32_t ID_2_val = HAL_GetUIDw1();
        id[7] = (ID_1_3_val) >> 24;
        id[6] = (ID_1_3_val) >> 16;
        id[5] = (ID_1_3_val) >> 8;
        id[4] = (ID_1_3_val);
        id[3] = (ID_2_val) >> 24;
        id[2] = (ID_2_val) >> 16;
        id[1] = (ID_2_val) >> 8;
        id[0] = (ID_2_val);
    } else {                // Expected
        id[7] = val & 0xFF;
        id[6] = (val >> 8) & 0xFF;
        id[5] = (val >> 16) & 0xFF;
        id[4] = (val >> 24) & 0xFF;
        val = LL_FLASH_GetDeviceID();
        id[3] = val & 0xFF;
        val = LL_FLASH_GetSTCompanyID();
        id[2] = val & 0xFF;
        id[1] = (val >> 8) & 0xFF;
        id[0] = (val >> 16) & 0xFF;
    }
}

// Get the device address
uint32_t GetDevAddr(void)
{
    uint32_t val = 0;
    val = LL_FLASH_GetUDN();
    if (val == 0xFFFFFFFF) {
        val = ((HAL_GetUIDw0()) ^ (HAL_GetUIDw1()) ^ (HAL_GetUIDw2()));
    }
    return val;
}

// HAL overrides for tick handling
HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority)
{
    // Don't enable SysTick if TIMER_IF is based on other counters (e.g. RTC)
    return HAL_OK;
}
uint32_t HAL_GetTick(void)
{
    // TIMER_IF can be based on other counter the SysTick e.g. RTC
    return TIMER_IF_GetTimerValue();
}
void HAL_Delay(__IO uint32_t Delay)
{
    // TIMER_IF can be based on other counter the SysTick e.g. RTC
    TIMER_IF_DelayMs(Delay);
}
