// Copyright 2021 Blues Inc.  All rights reserved.
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.

#pragma once

// Exported constants
#define __APP_VERSION_MAIN   (0x01U) /*!< [31:24] main version */
#define __APP_VERSION_SUB1   (0x01U) /*!< [23:16] sub1 version */
#define __APP_VERSION_SUB2   (0x00U) /*!< [15:8]  sub2 version */
#define __APP_VERSION_RC     (0x00U) /*!< [7:0]  release candidate */

#define __APP_VERSION_MAIN_SHIFT 24  /*!< main byte shift */
#define __APP_VERSION_SUB1_SHIFT 16  /*!< sub1 byte shift */
#define __APP_VERSION_SUB2_SHIFT 8   /*!< sub2 byte shift */
#define __APP_VERSION_RC_SHIFT   0   /*!< release candidate byte shift */

// Exported macros

// Application version
#define __APP_VERSION         ((__APP_VERSION_MAIN  << __APP_VERSION_MAIN_SHIFT)\
                               |(__APP_VERSION_SUB1 << __APP_VERSION_SUB1_SHIFT)\
                               |(__APP_VERSION_SUB2 << __APP_VERSION_SUB2_SHIFT)\
                               |(__APP_VERSION_RC   << __APP_VERSION_RC_SHIFT))

// LoRaWAN application version
#define __LORA_APP_VERSION            __APP_VERSION

