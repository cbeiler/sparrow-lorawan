// Copyright 2021 Blues Inc.  All rights reserved.
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.

#include "main.h"
#include "stm32wlxx_hal_cryp.h"
#include "stm32wlxx_hal_rng.h"
#include "stm32wlxx_ll_lpuart.h"

// See reference manual RM0453.  The size is the last entry's address in Table 89 + sizeof(uint32_t).
// (Don't be confused into using the entry number rather than the address, because there are negative
// entry numbers. The highest address is the only accurate way of determining the actual table size.)
#if defined(STM32WL55xx) || defined(STM32WLE5xx)
#define VECTOR_TABLE_SIZE_BYTES (0x0134+sizeof(uint32_t))
#else
#error "Please look up NVIC table size for this processor in reference manual."
#endif
#if defined ( __ICCARM__ ) /* IAR Compiler */
#pragma data_alignment=512
#elif defined ( __CC_ARM ) /* ARM Compiler */
__align(512)
#elif defined ( __GNUC__ ) /* GCC Compiler */
__attribute__ ((aligned (512)))
#endif
static uint8_t vector_t[VECTOR_TABLE_SIZE_BYTES];

// HAL data
UART_HandleTypeDef hlpuart1;
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_usart1_rx;
DMA_HandleTypeDef hdma_usart1_tx;
DMA_HandleTypeDef hdma_usart2_rx;
DMA_HandleTypeDef hdma_usart2_tx;
I2C_HandleTypeDef hi2c2;
DMA_HandleTypeDef hdma_i2c2_rx;
DMA_HandleTypeDef hdma_i2c2_tx;
SPI_HandleTypeDef hspi1;
DMA_HandleTypeDef hdma_spi1_rx;
DMA_HandleTypeDef hdma_spi1_tx;
uint32_t i2c2IOCompletions = 0;
static void (*TxCpltCallback_USART1)(void *) = NULL;
static void (*TxCpltCallback_USART2)(void *) = NULL;
static void (*TxCpltCallback_LPUART1)(void *) = NULL;

// Peripheral mask, so we can easily tell what is enabled and what is not
#define PERIPHERAL_LPUART1  0x00000001
#define PERIPHERAL_USART1   0x00000002
#define PERIPHERAL_USART2   0x00000004
#define PERIPHERAL_I2C2     0x00000008
#define PERIPHERAL_SPI1     0x00000010
#define PERIPHERAL_SPI1DMA  0x00000020
uint32_t peripherals = 0;

#define CHANDLER_TX_EN_Pin GPIO_PIN_2
#define CHANDLER_TX_EN_GPIO_Port GPIOB

// Forwards
void SystemClock_Config(void);

//transmission variables
//uint8_t TxData[16]0403000802459C
//char TxData[16] = {0x04,0x03,0x00,0x00,0x08,0x00,0x02,0x45,0x9C};
uint8_t TxData[16] = {0x04030000080002459C};
uint8_t RxData[16];
int indx = 0;


//data transmission functions
void sendData (uint8_t *data)
{
	// Pull DE high to enable TX operation
	//PA15 is connected to both Receive enable and transmit enable
	HAL_GPIO_WritePin(CHANDLER_TX_EN_GPIO_Port, CHANDLER_TX_EN_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, GPIO_PIN_SET);
	HAL_UART_Transmit(&huart1, data, strlen (data) , 1000);
	// Pull RE Low to enable RX operation
	HAL_GPIO_WritePin(CHANDLER_TX_EN_GPIO_Port, CHANDLER_TX_EN_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, GPIO_PIN_RESET);

}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	HAL_UARTEx_ReceiveToIdle_IT(&huart1, RxData, 16);
}

// Main entry
int main(void)
{

    // Copy the vectors
    memcpy(vector_t, (uint8_t *) FLASH_BASE, sizeof(vector_t));
    SCB->VTOR = (uint32_t) vector_t;

    // Reset of all peripherals, Initializes the Flash interface and the Systick.
    HAL_Init();

    // Configure clock
    SystemClock_Config();

    // Initialize for IO
    MX_GPIO_Init();
    MX_DMA_Init();
    MX_DBG_Init();

    // Initialize ST utilities
    MX_UTIL_Init();

    // Run the app, which will init its own peripherals
    // not using this
   // MX_AppMain();
    MX_USART1_UART_Init();

    //receive to idle interrupt
    HAL_UARTEx_ReceiveToIdle_IT(&huart1, RxData, 16);
    while(1){
    	//if statement using interrupt for running once every 5min
    	//sprintf(TxData, "0403000802459C", indx++);
    	sendData(TxData);
    	HAL_Delay(1000);//repeating every one second CHANGE LATER

    	//returns battery level
    	//use for health status
    	//TODO include battery level of sensor as well
    	//GetBatteryLevel();

    }

}

// System Clock Configuration
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    // Configure LSE Drive Capability
    HAL_PWR_EnableBkUpAccess();
    __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);

    // Configure the main internal regulator output voltage
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    // Initializes the CPU, AHB and APB busses clocks
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_LSE|RCC_OSCILLATORTYPE_MSI;
    RCC_OscInitStruct.LSEState = RCC_LSE_ON;
    RCC_OscInitStruct.MSIState = RCC_MSI_ON;
    RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_11;
    RCC_OscInitStruct.LSIDiv = RCC_LSI_DIV1;
    RCC_OscInitStruct.LSIState = RCC_LSI_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }

    // Configure the SYSCLKSource, HCLK, PCLK1 and PCLK2 clocks dividers
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK3|RCC_CLOCKTYPE_HCLK
                                  |RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_PCLK1
                                  |RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.AHBCLK3Divider = RCC_SYSCLK_DIV1;
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
        Error_Handler();
    }

    // Ensure that MSI is wake-up system clock
    __HAL_RCC_WAKEUPSTOP_CLK_CONFIG(RCC_STOP_WAKEUPCLOCK_MSI);

}

// Initialize for GPIO
void MX_GPIO_Init(void)
{

    // Enable all GPIO clocks
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();

    // Default all pins to analog except SWD pins.  (This has a hard-wired
    // assumption that SWDIO_GPIO_Port and SWCLK_GPIO_Port are GPIOA.)
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Pin = GPIO_PIN_All & (~(SWDIO_Pin|SWCLK_Pin));
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = GPIO_PIN_All;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
    HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);


    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(CHANDLER_TX_EN_GPIO_Port, CHANDLER_TX_EN_Pin, GPIO_PIN_SET);

    /*Configure GPIO pin : CHANDLER_TX_EN_Pin */
    GPIO_InitStruct.Pin = CHANDLER_TX_EN_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(CHANDLER_TX_EN_GPIO_Port, &GPIO_InitStruct);
}

// Enable DMA controller clock
void MX_DMA_Init(void)
{

    // DMA controller clock enable
    __HAL_RCC_DMAMUX1_CLK_ENABLE();
    __HAL_RCC_DMA1_CLK_ENABLE();
    __HAL_RCC_DMA2_CLK_ENABLE();

}

// Init I2C2
void MX_I2C2_Init(void)
{

    // Enable DMA interrupts
    HAL_NVIC_SetPriority(I2C2_RX_DMA_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(I2C2_RX_DMA_IRQn);
    HAL_NVIC_SetPriority(I2C2_TX_DMA_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(I2C2_TX_DMA_IRQn);

    // Configure I2C
    hi2c2.Instance = I2C2;
    hi2c2.Init.Timing = 0x307075B1;     // Same as notecard timing
    hi2c2.Init.OwnAddress1 = 0;
    hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c2.Init.OwnAddress2 = 0;
    hi2c2.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
    hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    if (HAL_I2C_Init(&hi2c2) != HAL_OK) {
        Error_Handler();
    }

    // Configure Analogue filter
    if (HAL_I2CEx_ConfigAnalogFilter(&hi2c2, I2C_ANALOGFILTER_ENABLE) != HAL_OK) {
        Error_Handler();
    }

    // Configure Digital filter
    if (HAL_I2CEx_ConfigDigitalFilter(&hi2c2, 0) != HAL_OK) {
        Error_Handler();
    }

    // Enabled
    peripherals |= PERIPHERAL_I2C2;

}

// DeInit I2C2
void MX_I2C2_DeInit(void)
{
    peripherals &= ~PERIPHERAL_I2C2;
    HAL_NVIC_DisableIRQ(I2C2_RX_DMA_IRQn);
    HAL_NVIC_DisableIRQ(I2C2_TX_DMA_IRQn);
    HAL_I2C_DeInit(&hi2c2);
}

// I2C1 DMA completion events
void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    if (hi2c == &hi2c2) {
        i2c2IOCompletions++;
    }
}
void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    if (hi2c == &hi2c2) {
        i2c2IOCompletions++;
    }
}
void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    if (hi2c == &hi2c2) {
        i2c2IOCompletions++;
    }
}
void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    if (hi2c == &hi2c2) {
        i2c2IOCompletions++;
    }
}

// Register a completion callback
void MX_UART_TxCpltCallback(UART_HandleTypeDef *huart, void (*cb)(void *))
{
    if (huart == &huart1) {
        TxCpltCallback_USART1 = cb;
    }
    if (huart == &huart2) {
        TxCpltCallback_USART2 = cb;
    }
    if (huart == &hlpuart1) {
        TxCpltCallback_LPUART1 = cb;
    }
}

// Transmit complete callback for serial ports
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart == &huart1 && TxCpltCallback_USART1 != NULL) {
        TxCpltCallback_USART1(huart);
    }
    if (huart == &huart2 && TxCpltCallback_USART2 != NULL) {
        TxCpltCallback_USART2(huart);
    }
    if (huart == &hlpuart1 && TxCpltCallback_LPUART1 != NULL) {
        TxCpltCallback_LPUART1(huart);
    }
}

// Receive from a register, and return true for success or false for failure
bool MY_I2C2_ReadRegister(uint16_t i2cAddress, uint8_t Reg, void *data, uint16_t maxdatalen, uint32_t timeoutMs)
{
    uint32_t ioCount = i2c2IOCompletions;
    uint32_t status = HAL_I2C_Mem_Read_DMA(&hi2c2, ((uint16_t)i2cAddress) << 1, (uint16_t)Reg, I2C_MEMADD_SIZE_8BIT, data, maxdatalen);
    if (status != HAL_OK) {
        return false;
    }
    uint32_t waitedMs = 0;
    uint32_t waitGranularityMs = 1;
    bool success = true;
    while (success && ioCount == i2c2IOCompletions) {
        HAL_Delay(waitGranularityMs);
        waitedMs += waitGranularityMs;
        if (timeoutMs != 0 && waitedMs > timeoutMs) {
            success = false;
        }
    }
    return success;
}

// Write a register, and return true for success or false for failure
bool MY_I2C2_WriteRegister(uint16_t i2cAddress, uint8_t Reg, void *data, uint16_t datalen, uint32_t timeoutMs)
{
    uint32_t ioCount = i2c2IOCompletions;
    uint32_t status = HAL_I2C_Mem_Write_DMA(&hi2c2, ((uint16_t)i2cAddress) << 1, (uint16_t)Reg, I2C_MEMADD_SIZE_8BIT, data, datalen);
    if (status != HAL_OK) {
        return false;
    }
    uint32_t waitedMs = 0;
    uint32_t waitGranularityMs = 1;
    bool success = true;
    while (success && ioCount == i2c2IOCompletions) {
        HAL_Delay(waitGranularityMs);
        waitedMs += waitGranularityMs;
        if (timeoutMs != 0 && waitedMs > timeoutMs) {
            success = false;
        }
    }
    return success;
}

// Transmit, and return true for success or false for failure
bool MY_I2C2_Transmit(uint16_t i2cAddress, void *data, uint16_t datalen, uint32_t timeoutMs)
{
    uint32_t ioCount = i2c2IOCompletions;
    uint32_t status = HAL_I2C_Master_Transmit_DMA(&hi2c2, ((uint16_t)i2cAddress) << 1, data, datalen);
    if (status != HAL_OK) {
        return false;
    }
    uint32_t waitedMs = 0;
    bool success = true;
    while (success && ioCount == i2c2IOCompletions) {
        HAL_Delay(1);
        waitedMs++;
        if (timeoutMs != 0 && waitedMs > timeoutMs) {
            success = false;
        }
    }
    return success;
}

// Receive, and return true for success or false for failure
bool MY_I2C2_Receive(uint16_t i2cAddress, void *data, uint16_t maxdatalen, uint32_t timeoutMs)
{
    uint32_t ioCount = i2c2IOCompletions;
    uint32_t status = HAL_I2C_Master_Receive_DMA(&hi2c2, ((uint16_t)i2cAddress) << 1, data, maxdatalen);
    if (status != HAL_OK) {
        return false;
    }
    uint32_t waitedMs = 0;
    uint32_t waitGranularityMs = 1;
    bool success = true;
    while (success && ioCount == i2c2IOCompletions) {
        HAL_Delay(waitGranularityMs);
        waitedMs += waitGranularityMs;
        if (timeoutMs != 0 && waitedMs > timeoutMs) {
            success = false;
        }
    }
    return success;
}

// SPI1 Initialization
void MX_SPI1_Init(void)
{

    // Enable DMA interrupts
    HAL_NVIC_SetPriority(SPI1_RX_DMA_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(SPI1_RX_DMA_IRQn);
    HAL_NVIC_SetPriority(SPI1_TX_DMA_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(SPI1_TX_DMA_IRQn);

    // SPI1 parameter configuration
    hspi1.Instance = SPI1;
    hspi1.Init.Mode = SPI_MODE_MASTER;
    hspi1.Init.Direction = SPI_DIRECTION_2LINES;
    hspi1.Init.DataSize = SPI_DATASIZE_4BIT;
    hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
    hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
    hspi1.Init.NSS = SPI_NSS_SOFT;
    hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
    hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
    hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    hspi1.Init.CRCPolynomial = 7;
    hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
    hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
    if (HAL_SPI_Init(&hspi1) != HAL_OK) {
        Error_Handler();
    }

    peripherals |= PERIPHERAL_SPI1;

}

// SPI1 Deinitialization
void MX_SPI1_DeInit(void)
{
    peripherals &= ~PERIPHERAL_SPI1;
    HAL_NVIC_DisableIRQ(SPI1_RX_DMA_IRQn);
    HAL_NVIC_DisableIRQ(SPI1_TX_DMA_IRQn);
    HAL_SPI_DeInit(&hspi1);
}

// USART1 init function
void MX_USART1_UART_Init(void)
{

    // Enable DMA interrupts
    HAL_NVIC_SetPriority(USART1_RX_DMA_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(USART1_RX_DMA_IRQn);
    HAL_NVIC_SetPriority(USART1_TX_DMA_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(USART1_TX_DMA_IRQn);

    // Initialize BAUDRATE
    //change UART1 baudrate, stop bits, start bits, parity for RS485
    huart1.Instance = USART1;
    huart1.Init.BaudRate = USART1_BAUDRATE; //115200
    huart1.Init.WordLength = UART_WORDLENGTH_8B; //8bit long
    huart1.Init.StopBits = UART_STOPBITS_1; //1 stop bit
    huart1.Init.Parity = UART_PARITY_NONE; //no parity
    huart1.Init.Mode = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;
    huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
    huart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
    huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
    if (HAL_UART_Init(&huart1) != HAL_OK) {
        Error_Handler();
    }

    // Enable FIFO
    if (HAL_UARTEx_SetTxFifoThreshold(&huart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK) {
        Error_Handler();
    }
    if (HAL_UARTEx_SetRxFifoThreshold(&huart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK) {
        Error_Handler();
    }
    if (HAL_UARTEx_EnableFifoMode(&huart1) != HAL_OK) {
        Error_Handler();
    }

    peripherals |= PERIPHERAL_USART1;

}

// Transmit to USART1
void MX_USART1_UART_Transmit(uint8_t *buf, uint32_t len, uint32_t timeoutMs)
{

    // Transmit
    HAL_UART_Transmit_DMA(&huart1, buf, len);

    // Wait, so that the caller won't mess with the buffer while the HAL is using it
    for (uint32_t i=0; i<timeoutMs; i++) {
        HAL_UART_StateTypeDef state = HAL_UART_GetState(&huart1);
        if ((state & HAL_UART_STATE_BUSY_TX) != HAL_UART_STATE_BUSY_TX) {
            break;
        }
        HAL_Delay(1);
    }

}

// USART1 Deinitialization
void MX_USART1_UART_DeInit(void)
{

    // Deinitialized
    peripherals &= ~PERIPHERAL_USART1;

    // Stop any pending DMA, if any
    HAL_UART_DMAStop(&huart1);

    // Reset peripheral
    __HAL_RCC_USART1_FORCE_RESET();
    __HAL_RCC_USART1_RELEASE_RESET();

    // Disable IDLE interrupt
    __HAL_UART_DISABLE_IT(&huart1, UART_IT_IDLE);

    // Deinit
    HAL_UART_DeInit(&huart1);

    // Deinit DMA interrupts
    HAL_NVIC_DisableIRQ(USART1_RX_DMA_IRQn);
    HAL_NVIC_DisableIRQ(USART1_TX_DMA_IRQn);

}

// USART2 init function
void MX_USART2_UART_Init(void)
{

    // Enable DMA interrupts
#ifdef USE_USART2_RX_DMA
    HAL_NVIC_SetPriority(USART2_RX_DMA_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(USART2_RX_DMA_IRQn);
#endif
    HAL_NVIC_SetPriority(USART2_TX_DMA_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(USART2_TX_DMA_IRQn);

    // Initialize
    huart2.Instance = USART2;
    huart2.Init.BaudRate = USART2_BAUDRATE;
    huart2.Init.WordLength = UART_WORDLENGTH_8B;
    huart2.Init.StopBits = UART_STOPBITS_1;
    huart2.Init.Parity = UART_PARITY_NONE;
    huart2.Init.Mode = UART_MODE_TX_RX;
    huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart2.Init.OverSampling = UART_OVERSAMPLING_16;
    huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
    huart2.Init.ClockPrescaler = UART_PRESCALER_DIV1;
    huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
    if (HAL_UART_Init(&huart2) != HAL_OK) {
        Error_Handler();
    }

    // Enable FIFO
    if (HAL_UARTEx_SetTxFifoThreshold(&huart2, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK) {
        Error_Handler();
    }
    if (HAL_UARTEx_SetRxFifoThreshold(&huart2, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK) {
        Error_Handler();
    }
    if (HAL_UARTEx_EnableFifoMode(&huart2) != HAL_OK) {
        Error_Handler();
    }

    // Enabled
    peripherals |= PERIPHERAL_USART2;

}

// USART2 suspend function
void MX_USART2_UART_Suspend(void)
{

    // Enable wakeup interrupt from STOP2 (RM0453 Tbl 93)
    // Note that this is the moral equivalent of doing
    // LL_LPUART_EnableIT_WKUP(USART2), however it works
    // on the dual-core processor to say "enable wakeup
    // on either core - whichever is available".
    LL_EXTI_EnableIT_0_31(LL_EXTI_LINE_27);

}

// USART2 resume function
void MX_USART2_UART_Resume(void)
{
    if (HAL_UART_Init(&huart2) != HAL_OK) {
        Error_Handler();
    }
    if (HAL_DMA_Init(&hdma_usart2_tx) != HAL_OK) {
        Error_Handler();
    }
}

// Transmit to USART2
void MX_USART2_UART_Transmit(uint8_t *buf, uint32_t len, uint32_t timeoutMs)
{

    // Transmit
    HAL_UART_Transmit_DMA(&huart2, buf, len);

    // Wait, so that the caller won't mess with the buffer while the HAL is using it
    for (uint32_t i=0; i<timeoutMs; i++) {
        HAL_UART_StateTypeDef state = HAL_UART_GetState(&huart2);
        if ((state & HAL_UART_STATE_BUSY_TX) != HAL_UART_STATE_BUSY_TX) {
            break;
        }
        HAL_Delay(1);
    }

}

// USART2 Deinitialization
void MX_USART2_UART_DeInit(void)
{

    // Deinitialized
    peripherals &= ~PERIPHERAL_USART2;

    // Stop any pending DMA, if any
    HAL_UART_DMAStop(&huart2);

    // Reset peripheral
    __HAL_RCC_USART2_FORCE_RESET();
    __HAL_RCC_USART2_RELEASE_RESET();

    // Disable IDLE interrupt
    __HAL_UART_DISABLE_IT(&huart2, UART_IT_IDLE);

    // Deinit
    HAL_UART_DeInit(&huart2);

    // Deinit DMA interrupts
#ifdef USE_USART2_RX_DMA
    HAL_NVIC_DisableIRQ(USART2_RX_DMA_IRQn);
#endif
    HAL_NVIC_DisableIRQ(USART2_TX_DMA_IRQn);

}

// LPUART1 Initialization Function
void MX_LPUART1_UART_Init(void)
{

    hlpuart1.Instance = LPUART1;
    hlpuart1.Init.BaudRate = LPUART1_BAUDRATE;
    hlpuart1.Init.WordLength = UART_WORDLENGTH_8B;
    hlpuart1.Init.StopBits = UART_STOPBITS_1;
    hlpuart1.Init.Parity = UART_PARITY_NONE;
    hlpuart1.Init.Mode = UART_MODE_TX_RX;
    hlpuart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    hlpuart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
    hlpuart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
    hlpuart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
    hlpuart1.FifoMode = UART_FIFOMODE_DISABLE;
    if (HAL_UART_Init(&hlpuart1) != HAL_OK) {
        Error_Handler();
    }
    if (HAL_UARTEx_SetTxFifoThreshold(&hlpuart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK) {
        Error_Handler();
    }
    if (HAL_UARTEx_SetRxFifoThreshold(&hlpuart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK) {
        Error_Handler();
    }
    if (HAL_UARTEx_DisableFifoMode(&hlpuart1) != HAL_OK) {
        Error_Handler();
    }

    peripherals |= PERIPHERAL_LPUART1;

}

// LPUART1 suspend function
void MX_LPUART1_UART_Suspend(void)
{

    // Clear OVERRUN flag
    LL_LPUART_ClearFlag_ORE(LPUART1);

    // Make sure that no LPUART transfer is on-going
    while (LL_LPUART_IsActiveFlag_BUSY(LPUART1) == 1) {};

    // Make sure that LPUART is ready to receive
    while (LL_LPUART_IsActiveFlag_REACK(LPUART1) == 0) {};

    // Configure LPUART1 transfer interrupts by clearing the WUF
    // flag and enabling the UART Wake Up from stop mode interrupt
    LL_LPUART_ClearFlag_WKUP(LPUART1);
    LL_LPUART_EnableIT_WKUP(LPUART1);

    // Enable Wake Up From Stop
    LL_LPUART_EnableInStopMode(LPUART1);

    // Unmask wakeup with Interrupt request from LPUART1
    LL_EXTI_EnableIT_0_31(LL_EXTI_LINE_28);

}

// LPUART1 resume function
void MX_LPUART1_UART_Resume(void)
{
}

// Transmit to LPUART1
void MX_LPUART1_UART_Transmit(uint8_t *buf, uint32_t len, uint32_t timeoutMs)
{

    // Transmit
    HAL_UART_Transmit_IT(&hlpuart1, buf, len);

    // Wait, so that the caller won't mess with the buffer while the HAL is using it
    for (uint32_t i=0; i<timeoutMs; i++) {
        HAL_UART_StateTypeDef state = HAL_UART_GetState(&hlpuart1);
        if ((state & HAL_UART_STATE_BUSY_TX) != HAL_UART_STATE_BUSY_TX) {
            break;
        }
        HAL_Delay(1);
    }

}

// LPUART1 De-Initialization Function
void MX_LPUART1_UART_DeInit(void)
{
    peripherals &= ~PERIPHERAL_LPUART1;
    HAL_UART_DeInit(&hlpuart1);
}

// Get the currently active peripherals
void MY_ActivePeripherals(char *buf, uint32_t buflen)
{
    *buf = '\0';
    if ((peripherals & PERIPHERAL_LPUART1) != 0) {
        strlcat(buf, "LPUART1 ", buflen);
    }
    if ((peripherals & PERIPHERAL_USART1) != 0) {
        strlcat(buf, "USART1 ", buflen);
    }
    if ((peripherals & PERIPHERAL_USART2) != 0) {
        strlcat(buf, "USART2 ", buflen);
    }
    if ((peripherals & PERIPHERAL_I2C2) != 0) {
        strlcat(buf, "I2C2 ", buflen);
    }
    if ((peripherals & PERIPHERAL_SPI1) != 0) {
        strlcat(buf, "SPI1 ", buflen);
    }
    if ((peripherals & PERIPHERAL_SPI1DMA) != 0) {
        strlcat(buf, "SPI1DMA ", buflen);
    }

}
