// Copyright 2021 Blues Inc.  All rights reserved.
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.

#include "main.h"
#include "app.h"

// Indicate that we've joined
void ledJoined()
{
    for (int i=0; i<3; i++) {
        HAL_GPIO_WritePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin, GPIO_PIN_SET);
        HAL_Delay(250);
        HAL_GPIO_WritePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin, GPIO_PIN_RESET);
        HAL_Delay(250);
    }

}

// Indicate that a join is in progress
void ledIndicateJoinInProgress(bool on)
{
    HAL_GPIO_WritePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin, on ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

// Toggle join status
void ledToggleJoin()
{
    bool on = HAL_GPIO_ReadPin(LED_BLUE_GPIO_Port, LED_BLUE_Pin) == GPIO_PIN_SET;
    ledIndicateJoinInProgress(!on);
}

// Indicate that a receive is in progress
void ledIndicateReceiveInProgress(bool on)
{
    HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, on ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

// Indicate that a receive is in progress
void ledIndicateTransmitInProgress(bool on)
{
    HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, on ? GPIO_PIN_SET : GPIO_PIN_RESET);
}
