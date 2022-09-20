// Copyright 2021 Blues Inc.  All rights reserved.
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.

#include "main.h"
#include "Region.h"     // Needed for LORAWAN_DEFAULT_DATA_RATE
#include "sys_app.h"
#include "lora_app.h"
#include "stm32_seq.h"
#include "stm32_timer.h"
#include "utilities_def.h"
#include "lora_app_version.h"
#include "lorawan_version.h"
#include "subghz_phy_version.h"
#include "lora_info.h"
#include "LmHandler.h"
#include "stm32_lpm.h"
#include "config_sys.h"
#include "app.h"
#include "adc_if.h"
#include "sensor.h"

// Sensor timers
static uint32_t sensorSecsSuccess = 10;
static uint32_t sensorSecsFailure = 10;

// Forwards
void App_Init(LoRaMacRegion_t selectedRegion);
static void SendTxData(void);
static void OnTxTimerEvent(void *context);
static void OnJoinRequest(LmHandlerJoinParams_t *joinParams);
static void OnTxData(LmHandlerTxParams_t *params);
static void OnRxData(LmHandlerAppData_t *appData, LmHandlerRxParams_t *params);
static void OnMacProcessNotify(void);
static void OnTxTimerLedEvent(void *context);
static void OnRxTimerLedEvent(void *context);
static void OnJoinTimerLedEvent(void *context);
void startSensorTimer(bool success);


// Private vars
static ActivationType_t ActivationType = LORAWAN_DEFAULT_ACTIVATION_TYPE;

// LoRaWAN handler Callbacks
static LmHandlerCallbacks_t LmHandlerCallbacks = {
    .GetBatteryLevel =           GetBatteryLevel,
    .GetTemperature =            GetTemperatureLevel,
    .GetUniqueId =               GetUniqueId,
    .GetDevAddr =                GetDevAddr,
    .OnMacProcess =              OnMacProcessNotify,
    .OnJoinRequest =             OnJoinRequest,
    .OnTxData =                  OnTxData,
    .OnRxData =                  OnRxData
};

// LoRaWAN handler parameters
static LmHandlerParams_t LmHandlerParams = {
    .ActiveRegion =             LORAMAC_REGION_US915,
    .DefaultClass =             LORAWAN_DEFAULT_CLASS,
    .AdrEnable =                LORAWAN_ADR_STATE,
    .TxDatarate =               LORAWAN_DEFAULT_DATA_RATE,
    .PingPeriodicity =          LORAWAN_DEFAULT_PING_SLOT_PERIODICITY
};

// Timer to handle the application Tx
static UTIL_TIMER_Object_t TxTimer;

// User application buffer
static uint8_t AppDataBuffer[LORAWAN_APP_DATA_BUFFER_MAX_SIZE];

// User application data structure
static LmHandlerAppData_t AppData = { 0, 0, AppDataBuffer };

// Timer to handle the application Tx Led to toggle
static UTIL_TIMER_Object_t TxLedTimer;

// Timer to handle the application Rx Led to toggle
static UTIL_TIMER_Object_t RxLedTimer;

// Timer to handle the application Join Led to toggle
static UTIL_TIMER_Object_t JoinLedTimer;

// Init lorawan
void App_Init(LoRaMacRegion_t selectedRegion)
{

    // Get LoRa APP version
    APP_LOG(TS_OFF, VLEVEL_M, "APP_VERSION:        V%X.%X.%X\r\n",
            (uint8_t)(__LORA_APP_VERSION >> __APP_VERSION_MAIN_SHIFT),
            (uint8_t)(__LORA_APP_VERSION >> __APP_VERSION_SUB1_SHIFT),
            (uint8_t)(__LORA_APP_VERSION >> __APP_VERSION_SUB2_SHIFT));

    // Get MW LoraWAN info
    APP_LOG(TS_OFF, VLEVEL_M, "MW_LORAWAN_VERSION: V%X.%X.%X\r\n",
            (uint8_t)(__LORAWAN_VERSION >> __APP_VERSION_MAIN_SHIFT),
            (uint8_t)(__LORAWAN_VERSION >> __APP_VERSION_SUB1_SHIFT),
            (uint8_t)(__LORAWAN_VERSION >> __APP_VERSION_SUB2_SHIFT));

    // Get MW SubGhz_Phy info
    APP_LOG(TS_OFF, VLEVEL_M, "MW_RADIO_VERSION:   V%X.%X.%X\r\n",
            (uint8_t)(__SUBGHZ_PHY_VERSION >> __APP_VERSION_MAIN_SHIFT),
            (uint8_t)(__SUBGHZ_PHY_VERSION >> __APP_VERSION_SUB1_SHIFT),
            (uint8_t)(__SUBGHZ_PHY_VERSION >> __APP_VERSION_SUB2_SHIFT));

    UTIL_TIMER_Create(&TxLedTimer, 0xFFFFFFFFU, UTIL_TIMER_ONESHOT, OnTxTimerLedEvent, NULL);
    UTIL_TIMER_Create(&RxLedTimer, 0xFFFFFFFFU, UTIL_TIMER_ONESHOT, OnRxTimerLedEvent, NULL);
    UTIL_TIMER_Create(&JoinLedTimer, 0xFFFFFFFFU, UTIL_TIMER_PERIODIC, OnJoinTimerLedEvent, NULL);
    UTIL_TIMER_SetPeriod(&TxLedTimer, 500);
    UTIL_TIMER_SetPeriod(&RxLedTimer, 500);
    UTIL_TIMER_SetPeriod(&JoinLedTimer, 500);

    UTIL_SEQ_RegTask((1 << CFG_SEQ_Task_LmHandlerProcess), UTIL_SEQ_RFU, LmHandlerProcess);
    UTIL_SEQ_RegTask((1 << CFG_SEQ_Task_LoRaSendOnTxTimer), UTIL_SEQ_RFU, SendTxData);

    // Init Info table used by LmHandler
    LoraInfo_Init();

    // Init the Lora Stack
    LmHandlerParams.ActiveRegion = selectedRegion;
    LmHandlerInit(&LmHandlerCallbacks);
    LmHandlerConfigure(&LmHandlerParams);

    UTIL_TIMER_Start(&JoinLedTimer);

    LmHandlerJoin(ActivationType);

    // Schedule the first sensor transmit
    UTIL_TIMER_Create(&TxTimer, 0xFFFFFFFFU, UTIL_TIMER_ONESHOT, OnTxTimerEvent, NULL);
    startSensorTimer(false);

}

// Schedule the next transmit timer wakeup
void startSensorTimer(bool success)
{
    uint32_t seconds = success ? sensorSecsSuccess : sensorSecsFailure;
    APP_PRINTF("sensor will next be sampled in %d seconds\r\n", seconds);
    UTIL_TIMER_Stop(&TxTimer);
    UTIL_TIMER_SetPeriod(&TxTimer, seconds * 1000);
    UTIL_TIMER_Start(&TxTimer);
}

// callback when LoRa application has received a frame
// appData data received in the last Rx
// params status of last Rx
static void OnRxData(LmHandlerAppData_t *appData, LmHandlerRxParams_t *params)
{

    if ((appData != NULL) || (params != NULL)) {
        ledIndicateReceiveInProgress(true);

        UTIL_TIMER_Start(&RxLedTimer);

        static const char *slotStrings[] = { "1", "2", "C", "C Multicast", "B Ping-Slot", "B Multicast Ping-Slot" };

        APP_LOG(TS_OFF, VLEVEL_M, "\r\n###### ========== MCPS-Indication ==========\r\n");
        APP_LOG(TS_OFF, VLEVEL_H, "###### D/L FRAME:%04d | SLOT:%s | PORT:%d | DR:%d | RSSI:%d | SNR:%d\r\n",
                params->DownlinkCounter, slotStrings[params->RxSlot], appData->Port, params->Datarate, params->Rssi, params->Snr);
        APP_LOG(TS_OFF, VLEVEL_M, "\r\n");
        switch (appData->Port) {
        case LORAWAN_SWITCH_CLASS_PORT:
            // this port switches the class
            if (appData->BufferSize == 1) {
                switch (appData->Buffer[0]) {
                case 0: {
                    LmHandlerRequestClass(CLASS_A);
                    break;
                }
                case 1: {
                    LmHandlerRequestClass(CLASS_B);
                    break;
                }
                case 2: {
                    LmHandlerRequestClass(CLASS_C);
                    break;
                }
                default:
                    break;
                }
            }
            break;
        case LORAWAN_USER_APP_PORT:
            break;

        default:

            break;
        }
    }

}

// LoRa End Node send request
static void SendTxData(void)
{

    // Get sensor data to be transmitted
    bool success = sensorGetData(AppDataBuffer, sizeof(AppDataBuffer), &AppData.BufferSize, &sensorSecsSuccess, &sensorSecsFailure);
    if (!success || AppData.BufferSize == 0) {
        startSensorTimer(false);
        return;
    }

    // Set the transmit port for the app data
    AppData.Port = LORAWAN_USER_APP_PORT;

    // Send the data
    UTIL_TIMER_Time_t nextTxIn = 0;
    if (LORAMAC_HANDLER_SUCCESS == LmHandlerSend(&AppData, LORAWAN_DEFAULT_CONFIRMED_MSG_STATE, &nextTxIn, false)) {
        APP_LOG(TS_ON, VLEVEL_L, "SEND REQUEST\r\n");
    } else if (nextTxIn > 0) {
        APP_LOG(TS_ON, VLEVEL_L, "Next Tx in  : ~%d second(s)\r\n", (nextTxIn / 1000));
    }

    // Start a timer for the next transmit, assuming success
    startSensorTimer(true);

}

// TX timer callback function
// context ptr of timer context
static void OnTxTimerEvent(void *context)
{
    UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_LoRaSendOnTxTimer), CFG_SEQ_Prio_0);
}

// LED Tx timer callback function
// context ptr of LED context
static void OnTxTimerLedEvent(void *context)
{
    ledIndicateTransmitInProgress(false);
}

// LED Rx timer callback function
// context ptr of LED context
static void OnRxTimerLedEvent(void *context)
{
    ledIndicateReceiveInProgress(false);
}

// LED Join timer callback function
// context ptr of LED context
static void OnJoinTimerLedEvent(void *context)
{
    ledToggleJoin();
}

// tx event callback function
// params status of last Tx
static void OnTxData(LmHandlerTxParams_t *params)
{
    if ((params != NULL)) {
        // Process Tx event only if its a mcps response to prevent some internal events (mlme)
        if (params->IsMcpsConfirm != 0) {
            ledIndicateTransmitInProgress(true);
            UTIL_TIMER_Start(&TxLedTimer);

            APP_LOG(TS_OFF, VLEVEL_M, "\r\n###### ========== MCPS-Confirm =============\r\n");
            APP_LOG(TS_OFF, VLEVEL_H, "###### U/L FRAME:%04d | PORT:%d | DR:%d | PWR:%d", params->UplinkCounter,
                    params->AppData.Port, params->Datarate, params->TxPower);

            APP_LOG(TS_OFF, VLEVEL_H, " | MSG TYPE:");
            if (params->MsgType == LORAMAC_HANDLER_CONFIRMED_MSG) {
                APP_LOG(TS_OFF, VLEVEL_H, "CONFIRMED [%s]\r\n", (params->AckReceived != 0) ? "ACK" : "NACK");
            } else {
                APP_LOG(TS_OFF, VLEVEL_H, "UNCONFIRMED\r\n");
            }

            APP_LOG(TS_OFF, VLEVEL_M, "\r\n");
        }
    }
}

// join event callback function
// joinParams status of join
static void OnJoinRequest(LmHandlerJoinParams_t *joinParams)
{
    if (joinParams != NULL) {
        if (joinParams->Status == LORAMAC_HANDLER_SUCCESS) {
            UTIL_TIMER_Stop(&JoinLedTimer);
            ledJoined();

            APP_LOG(TS_OFF, VLEVEL_M, "\r\n###### = JOINED = ");
            if (joinParams->Mode == ACTIVATION_TYPE_ABP) {
                APP_LOG(TS_OFF, VLEVEL_M, "ABP ======================\r\n");
            } else {
                APP_LOG(TS_OFF, VLEVEL_M, "OTAA =====================\r\n");
            }
        } else {
            APP_LOG(TS_OFF, VLEVEL_M, "\r\n###### = JOIN FAILED\r\n");
            startSensorTimer(false);
            APP_LOG(TS_OFF, VLEVEL_M, "\r\n");
        }
    }
}

// Will be called each time a Radio IRQ is handled by the MAC layer
static void OnMacProcessNotify(void)
{
    UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_LmHandlerProcess), CFG_SEQ_Prio_0);
}

