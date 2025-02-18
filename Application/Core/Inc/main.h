// Copyright 2021 Blues Inc.  All rights reserved.
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "stm32wlxx.h"
#include "stm32wlxx_hal.h"
#include "config_sys.h"
#include "board.h"

void Error_Handler(void);
void MX_AppMain(void);
void MX_AppISR(uint16_t GPIO_Pin);
void MX_UTIL_Init(void);
void MX_Breakpoint(void);
void MX_GPIO_Init(void);
void MX_DMA_Init(void);
void MX_DBG_Init(void);
void MX_DBG_DeInit(void);
void MX_DBG_Suspend(void);
void MX_DBG_Resume(void);
void MX_DBG(const char *message, size_t length, uint32_t timeout);
void MX_DBG_RxCallback(void (*cb)(uint8_t *rxChar, uint16_t size, uint8_t error));
void MX_DBG_Disable(void);
void MX_DBG_Enable(void);
bool MX_DBG_Enabled(void);
bool MX_DBG_Active(void);
bool MX_DBG_Available(void);
void MX_DBG_TxCpltCallback(void (*cb)(void *));
void MX_UART_TxCpltCallback(UART_HandleTypeDef *huart, void (*cb)(void *));
uint8_t MX_DBG_Receive(bool *underrun, bool *overrun);
void MX_USART1_UART_Init(void);
void MX_USART1_UART_Transmit(uint8_t *buf, uint32_t len, uint32_t timeoutMs);
void MX_USART1_UART_DeInit(void);
void MX_USART2_UART_Init(void);
void MX_USART2_UART_Suspend(void);
void MX_USART2_UART_Resume(void);
void MX_USART2_UART_Transmit(uint8_t *buf, uint32_t len, uint32_t timeoutMs);
void MX_USART2_UART_DeInit(void);
void MX_LPUART1_UART_Init(void);
void MX_LPUART1_UART_Suspend(void);
void MX_LPUART1_UART_Resume(void);
void MX_LPUART1_UART_Transmit(uint8_t *buf, uint32_t len, uint32_t timeoutMs);
void MX_LPUART1_UART_DeInit(void);
void MX_I2C2_Init(void);
void MX_I2C2_DeInit(void);
void MY_ActivePeripherals(char *buf, uint32_t buflen);
bool MY_I2C2_ReadRegister(uint16_t i2cAddress, uint8_t Reg, void *data, uint16_t maxdatalen, uint32_t timeoutMs);
bool MY_I2C2_WriteRegister(uint16_t i2cAddress, uint8_t Reg, void *data, uint16_t datalen, uint32_t timeoutMs);
bool MY_I2C2_Transmit(uint16_t i2cAddress, void *data, uint16_t datalen, uint32_t timeoutMs);
bool MY_I2C2_Receive(uint16_t i2cAddress, void *data, uint16_t maxdatalen, uint32_t timeoutMs);
void MX_SPI1_Init(void);
void MX_SPI1_DeInit(void);
void MX_SUBGHZ_Init(void);

void Error_Handler(void);

size_t strlcpy(char *dst, const char *src, size_t siz);
size_t strlcat(char *dst, const char *src, size_t siz);

extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef hlpuart1;
extern DMA_HandleTypeDef hdma_usart2_tx;

