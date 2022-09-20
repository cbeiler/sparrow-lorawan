// Copyright 2021 Blues Inc.  All rights reserved.
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.

#include "main.h"
#include "stm32wlxx_it.h"
#include "stm32wlxx_hal_gpio.h"
#include "stm32wlxx_hal_cryp.h"
#include "stm32wlxx_hal_rng.h"

// Handles

extern DMA_HandleTypeDef hdma_i2c2_rx;
extern DMA_HandleTypeDef hdma_i2c2_tx;
extern I2C_HandleTypeDef hi2c2;
extern DMA_HandleTypeDef hdma_spi1_rx;
extern DMA_HandleTypeDef hdma_spi1_tx;
extern SPI_HandleTypeDef hspi1;
extern DMA_HandleTypeDef hdma_usart1_rx;
extern DMA_HandleTypeDef hdma_usart1_tx;
extern DMA_HandleTypeDef hdma_usart2_rx;
extern DMA_HandleTypeDef hdma_usart2_tx;
extern UART_HandleTypeDef hlpuart1;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern RTC_HandleTypeDef hrtc;
extern SUBGHZ_HandleTypeDef hsubghz;
extern DMA_HandleTypeDef hdma_usart2_tx;

// Forwards
void EXTI0_IRQHandler(void);
void EXTI1_IRQHandler(void);
void EXTI2_IRQHandler(void);
void EXTI3_IRQHandler(void);
void EXTI4_IRQHandler(void);
void EXTI9_5_IRQHandler(void);
void EXTI15_10_IRQHandler(void);
void GPIO_EXTI_IRQHandler(uint16_t GPIO_Pin);

// For panic breakpoint
void MX_Breakpoint()
{
    if (MX_DBG_Active()) {
        asm ("BKPT 0");
    }
}

// Cortex core interrupt handlers
void NMI_Handler(void)
{
    MX_Breakpoint();
    NVIC_SystemReset();
}
void HardFault_Handler(void)
{
    MX_Breakpoint();
    NVIC_SystemReset();
}
void MemManage_Handler(void)
{
    MX_Breakpoint();
    NVIC_SystemReset();
}
void BusFault_Handler(void)
{
    MX_Breakpoint();
    NVIC_SystemReset();
}
void UsageFault_Handler(void)
{
    MX_Breakpoint();
}
void SVC_Handler(void)
{
}
void DebugMon_Handler(void)
{
}
void PendSV_Handler(void)
{
}
void SysTick_Handler(void)
{
}

// Peripheral interrupt handlers
// please refer to the startup file (startup_stm32wlxx.s)

// This function handles RTC Tamper, RTC TimeStamp, LSECSS and RTC SSRU Interrupts.
void TAMP_STAMP_LSECSS_SSRU_IRQHandler(void)
{
    HAL_RTCEx_SSRUIRQHandler(&hrtc);
}

// DMA Handlers
void SPI1_RX_DMA_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma_spi1_rx);
}
void SPI1_TX_DMA_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma_spi1_tx);
}
void I2C2_RX_DMA_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma_i2c2_rx);
}
void I2C2_TX_DMA_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma_i2c2_tx);
}
void USART1_RX_DMA_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma_usart1_rx);
}
void USART1_TX_DMA_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma_usart1_tx);
}
#ifdef USE_USART2_RX_DMA
void USART2_RX_DMA_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma_usart2_rx);
}
#endif
void USART2_TX_DMA_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma_usart2_tx);
}

// I2C2 Event Interrupt
void I2C2_EV_IRQHandler(void)
{
    HAL_I2C_EV_IRQHandler(&hi2c2);
}

// I2C2 Error Interrupt
void I2C2_ER_IRQHandler(void)
{
    HAL_I2C_ER_IRQHandler(&hi2c2);
}

// SPI1 Interrupt
void SPI1_IRQHandler(void)
{
    HAL_SPI_IRQHandler(&hspi1);
}

// USART1 Interrupt
void USART1_IRQHandler(void)
{
    HAL_UART_IRQHandler(&huart1);
}

// USART2 Interrupt
void USART2_IRQHandler(void)
{
    HAL_UART_IRQHandler(&huart2);
}

// LPUART1 Interrupt
void LPUART1_IRQHandler(void)
{
    HAL_UART_IRQHandler(&hlpuart1);
}

// Method called when an external interrupt happens
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    MX_AppISR(GPIO_Pin);
}

void GPIO_EXTI_IRQHandler(uint16_t GPIO_Pin)
{
    if(__HAL_GPIO_EXTI_GET_IT(GPIO_Pin) != RESET) {
        uint16_t GPIO_Line = GPIO_Pin & EXTI->PR1;
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_Pin);
        HAL_GPIO_EXTI_Callback(GPIO_Line);
    }
}
void EXTI0_IRQHandler( void )
{
    GPIO_EXTI_IRQHandler(GPIO_PIN_0);
}
void EXTI1_IRQHandler( void )
{
    GPIO_EXTI_IRQHandler(GPIO_PIN_1);
}
void EXTI2_IRQHandler( void )
{
    GPIO_EXTI_IRQHandler(GPIO_PIN_2);
}
void EXTI3_IRQHandler( void )
{
    GPIO_EXTI_IRQHandler(GPIO_PIN_3);
}
void EXTI4_IRQHandler( void )
{
    GPIO_EXTI_IRQHandler(GPIO_PIN_4);
}
void EXTI9_5_IRQHandler( void )
{
    GPIO_EXTI_IRQHandler(GPIO_PIN_9|GPIO_PIN_8|GPIO_PIN_7|GPIO_PIN_6|GPIO_PIN_5);
}
void EXTI15_10_IRQHandler( void )
{
    GPIO_EXTI_IRQHandler(GPIO_PIN_15|GPIO_PIN_14|GPIO_PIN_13|GPIO_PIN_12|GPIO_PIN_11|GPIO_PIN_10);
}

// RTC Alarms (A and B) Interrupt.
void RTC_Alarm_IRQHandler(void)
{
    HAL_RTC_AlarmIRQHandler(&hrtc);
}

// SubGHZ Radio Interrupt
void SUBGHZ_Radio_IRQHandler(void)
{
    HAL_SUBGHZ_IRQHandler(&hsubghz);
}

// Error handling
void Error_Handler(void)
{
    MX_Breakpoint();
    NVIC_SystemReset();
}

// Assertion handling
#ifdef  USE_FULL_ASSERT
// Reports the name of the source file and the source line number
// where the assert_param error has occurred.
//  file: pointer to the source file name
//  line: assert_param error line source number
void assert_failed(uint8_t *file, uint32_t line)
{
    MX_Breakpoint();
    NVIC_SystemReset();
}
#endif // USE_FULL_ASSERT
