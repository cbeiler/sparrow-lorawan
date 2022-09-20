// Copyright 2021 Blues Inc.  All rights reserved.
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.

// End-device commissioning parameters
#pragma once

// ******************************************************************************
// ********************************** WARNING ***********************************
// ******************************************************************************
//
// The LoRaWAN AES128 keys should ideally be stored and provisioned on secure-elements.
//
// This project provides a software emulated secure-element, whose
// LoRaWAN AES128 keys are located in the se-identity.h file.
//
// ******************************************************************************
// ******************************************************************************
// ******************************************************************************
#include "se-identity.h"

// When using ABP activation the MAC layer must know in advance to which server
// version it will be connected.
#define ABP_ACTIVATION_LRWAN_VERSION_V10x                  0x01000300 /* 1.0.3.0 */
#define ABP_ACTIVATION_LRWAN_VERSION                       ABP_ACTIVATION_LRWAN_VERSION_V10x

// Indicates if the end-device support the operation with repeaters
#define LORAWAN_REPEATER_SUPPORT                           false

// Indicates if the end-device is to be connected to a private or public network
#define LORAWAN_PUBLIC_NETWORK                             true

// Current network ID
#define LORAWAN_NETWORK_ID                                 ( uint32_t )0

