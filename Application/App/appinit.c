// Copyright 2021 Blues Inc.  All rights reserved.
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.

#include "app.h"
#include "Region.h"
#include "sys_app.h"
#include "stm32_seq.h"

// Statics
bool buttonHeldAtBoot = false;
uint32_t radioBand = 0;

// Region
LoRaMacRegion_t RegionSwitchSelection = LORAMAC_REGION_US915;

// Forwards
extern void App_Init(LoRaMacRegion_t selectedRegion);
void ioInit(void);
int tristate(uint16_t pin, GPIO_TypeDef *port);

// Main app processing loop
void MX_AppMain(void)
{

    // Initialize GPIOs
    ioInit();

    // Initialize peripherals
    SystemApp_Init();
    App_Init(RegionSwitchSelection);

    // Loop, processing LoRaWAN stats
    while (1) {
        UTIL_SEQ_Run(UTIL_SEQ_DEFAULT);
    }

}

// ISR for interrupts to be processed by the app
void MX_AppISR(uint16_t GPIO_Pin)
{
}

// Get the value of a tri-state GPIO
#define TRISTATE_FLOAT  0
#define TRISTATE_HIGH   1
#define TRISTATE_LOW    2
int tristate(uint16_t pin, GPIO_TypeDef *port)
{
    GPIO_InitTypeDef  gpio_init_structure = {0};
    gpio_init_structure.Mode = GPIO_MODE_INPUT;
    gpio_init_structure.Pull = GPIO_PULLUP;
    gpio_init_structure.Speed = GPIO_SPEED_FREQ_HIGH;
    gpio_init_structure.Pin = pin;
    HAL_GPIO_Init(port, &gpio_init_structure);
    bool pulledHigh = (GPIO_PIN_SET == HAL_GPIO_ReadPin(port, pin));
    gpio_init_structure.Mode = GPIO_MODE_INPUT;
    gpio_init_structure.Pull = GPIO_PULLDOWN;
    gpio_init_structure.Speed = GPIO_SPEED_FREQ_HIGH;
    gpio_init_structure.Pin = pin;
    HAL_GPIO_Init(port, &gpio_init_structure);
    bool pulledLow = (GPIO_PIN_RESET == HAL_GPIO_ReadPin(port, pin));
    gpio_init_structure.Mode = GPIO_MODE_INPUT;
    gpio_init_structure.Pull = GPIO_NOPULL;
    gpio_init_structure.Speed = GPIO_SPEED_FREQ_HIGH;
    gpio_init_structure.Pin = pin;
    HAL_GPIO_Init(port, &gpio_init_structure);
    bool high = (GPIO_PIN_SET == HAL_GPIO_ReadPin(port, pin));
    gpio_init_structure.Mode = GPIO_MODE_ANALOG;
    gpio_init_structure.Pull = GPIO_NOPULL;
    gpio_init_structure.Speed = GPIO_SPEED_FREQ_LOW;
    gpio_init_structure.Pin = pin;
    HAL_GPIO_Init(port, &gpio_init_structure);
    if (pulledHigh && pulledLow) {
        return TRISTATE_FLOAT;
    }
    return (high ? TRISTATE_HIGH : TRISTATE_LOW);
}

// Initialize app hardware I/O
void ioInit(void)
{
    GPIO_InitTypeDef  gpio_init_structure = {0};

    // Compute the RF frequency based on the region switch settings.  Note that
    // we power these pins with LED_RED so that even if the user happens to select
    // an invalid switch combination they aren't a constant current draw on the system.
    // This switch design methodology allows for a selection of any of 9 unique
    // frequency plans based on the switches.  We have chosen what we view to be the
    // the most common plans globally, but the developer can feel free to reassign
    // these as is appropriate for their product or market.
    RegionSwitchSelection = LORAMAC_REGION_US915;
#if (CURRENT_BOARD != BOARD_NUCLEO)
    gpio_init_structure.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_init_structure.Pull = GPIO_NOPULL;
    gpio_init_structure.Speed = GPIO_SPEED_FREQ_LOW;
    gpio_init_structure.Pin = LED_RED_Pin;
    HAL_GPIO_Init(LED_RED_GPIO_Port, &gpio_init_structure);
    HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, GPIO_PIN_SET);
    int gpio0 = tristate(RFSEL_1_Pin, RFSEL_1_GPIO_Port);
    int gpio1 = tristate(RFSEL_0_Pin, RFSEL_0_GPIO_Port);
    if (gpio0 == TRISTATE_FLOAT && gpio1 == TRISTATE_FLOAT) {
        // 0 OFF OFF OFF OFF
        RegionSwitchSelection = LORAMAC_REGION_US915;
    } else if (gpio0 == TRISTATE_HIGH && gpio1 == TRISTATE_FLOAT) {
        // 1  ON OFF OFF OFF
        RegionSwitchSelection = LORAMAC_REGION_AS923;
    } else if (gpio0 == TRISTATE_LOW && gpio1 == TRISTATE_FLOAT) {
        // 2 OFF  ON OFF OFF
        RegionSwitchSelection = LORAMAC_REGION_KR920;
    } else if (gpio0 == TRISTATE_FLOAT && gpio1 == TRISTATE_HIGH) {
        // 3 OFF OFF  ON OFF
        RegionSwitchSelection = LORAMAC_REGION_IN865;
    } else if (gpio0 == TRISTATE_HIGH && gpio1 == TRISTATE_HIGH) {
        // 4 ON OFF  ON OFF
        RegionSwitchSelection = LORAMAC_REGION_EU868;
    } else if (gpio0 == TRISTATE_LOW && gpio1 == TRISTATE_HIGH) {
        // 5 OFF  ON  ON OFF
        RegionSwitchSelection = LORAMAC_REGION_RU864;
    } else if (gpio0 == TRISTATE_FLOAT && gpio1 == TRISTATE_LOW) {
        // 6 OFF OFF OFF  ON
        RegionSwitchSelection = LORAMAC_REGION_AU915;
    } else if (gpio0 == TRISTATE_HIGH && gpio1 == TRISTATE_LOW) {
        // 7 ON OFF OFF  ON
        RegionSwitchSelection = LORAMAC_REGION_CN470;
//        RegionSwitchSelection = LORAMAC_REGION_CN779;
    } else if (gpio0 == TRISTATE_LOW && gpio1 == TRISTATE_LOW) {
        // 8 OFF  ON OFF  ON
        RegionSwitchSelection = LORAMAC_REGION_EU433;
    }
#endif

    // Init LEDs
    gpio_init_structure.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_init_structure.Pull = GPIO_NOPULL;
    gpio_init_structure.Speed = GPIO_SPEED_FREQ_LOW;
    gpio_init_structure.Pin = LED_BLUE_Pin;
    HAL_GPIO_Init(LED_BLUE_GPIO_Port, &gpio_init_structure);
    HAL_GPIO_WritePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin, GPIO_PIN_RESET);
    gpio_init_structure.Pin = LED_GREEN_Pin;
    HAL_GPIO_Init(LED_GREEN_GPIO_Port, &gpio_init_structure);
    HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, GPIO_PIN_RESET);
    gpio_init_structure.Pin = LED_RED_Pin;
    HAL_GPIO_Init(LED_RED_GPIO_Port, &gpio_init_structure);
    HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, GPIO_PIN_RESET);

    // Init button, and determine whether or not it was held down at boot
    gpio_init_structure.Pin = BUTTON1_Pin;
    gpio_init_structure.Mode = GPIO_MODE_IT_RISING_FALLING;
    gpio_init_structure.Speed = GPIO_SPEED_FREQ_HIGH;
    gpio_init_structure.Pull = BUTTON1_ACTIVE_HIGH ? GPIO_PULLDOWN : GPIO_PULLUP;
    HAL_GPIO_Init(BUTTON1_GPIO_Port, &gpio_init_structure);
    for (int i=0; i<250; i++) {
        HAL_Delay(1);
        buttonHeldAtBoot = (HAL_GPIO_ReadPin(BUTTON1_GPIO_Port, BUTTON1_Pin) == (BUTTON1_ACTIVE_HIGH ? GPIO_PIN_SET : GPIO_PIN_RESET));
        if (!buttonHeldAtBoot) {
            break;
        }
    }
    HAL_NVIC_SetPriority(BUTTON1_EXTI_IRQn, BUTTONx_IT_PRIORITY, 0x00);
    HAL_NVIC_EnableIRQ(BUTTON1_EXTI_IRQn);

}
