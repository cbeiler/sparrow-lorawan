// Copyright 2021 Blues Inc.  All rights reserved.
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.

#include "subghz.h"

// Handle
SUBGHZ_HandleTypeDef hsubghz;

// SUBGHZ init function
void MX_SUBGHZ_Init(void)
{
    hsubghz.Init.BaudratePrescaler = SUBGHZSPI_BAUDRATEPRESCALER_4;
    if (HAL_SUBGHZ_Init(&hsubghz) != HAL_OK) {
        Error_Handler();
    }
}

// MSP Init
void HAL_SUBGHZ_MspInit(SUBGHZ_HandleTypeDef* subghzHandle)
{

    // SUBGHZ clock enable
    __HAL_RCC_SUBGHZSPI_CLK_ENABLE();

    // SUBGHZ interrupt Init
    HAL_NVIC_SetPriority(SUBGHZ_Radio_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(SUBGHZ_Radio_IRQn);
}

// MSP DeInit
void HAL_SUBGHZ_MspDeInit(SUBGHZ_HandleTypeDef* subghzHandle)
{

    // Peripheral clock disable
    __HAL_RCC_SUBGHZSPI_CLK_DISABLE();

    // SUBGHZ interrupt Deinit
    HAL_NVIC_DisableIRQ(SUBGHZ_Radio_IRQn);

}
