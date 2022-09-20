// Copyright 2021 Blues Inc.  All rights reserved.
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.

// Header for LoRaWAN middleware instances
#pragma once

// Region
// Support for the region listed here will be linked in the MW code,
// and the application shall just dynamically use one region at a time
#define REGION_AS923
#define REGION_AU915
#define REGION_CN470
#define REGION_CN779
#define REGION_EU433
#define REGION_EU868
#define REGION_KR9
#define REGION_IN865
#define REGION_US915
#define REGION_RU864

// Limits the number usable channels by default for AU915, CN470 and US915 regions
// Note the default channel mask with this option activates the first 8 channels.
//this default mask can be modified in the RegionXXXXXInitDefaults function associated with the active region.
#define HYBRID_ENABLED                                  0

// Define the read access of the keys in memory
// Note: this value should be disabled after the development process
#define KEY_EXTRACTABLE                                 1

// Enables/Disables the context storage management storage.
// Must be enabled for LoRaWAN 1.0.4 or later.
#define CONTEXT_MANAGEMENT_ENABLED                      0

// Class B
#define LORAMAC_CLASSB_ENABLED  0

#if ( LORAMAC_CLASSB_ENABLED == 1 )
// CLASS B LSE crystal calibration
// Temperature coefficient of the clock source
#define RTC_TEMP_COEFFICIENT                            ( -0.035 )

// Temperature coefficient deviation of the clock source
#define RTC_TEMP_DEV_COEFFICIENT                        ( 0.0035 )

// Turnover temperature of the clock source
#define RTC_TEMP_TURNOVER                               ( 25.0 )

// Turnover temperature deviation of the clock source
#define RTC_TEMP_DEV_TURNOVER                           ( 5.0 )
#endif /* LORAMAC_CLASSB_ENABLED == 1 */

// Exported macros
#ifndef CRITICAL_SECTION_BEGIN
#define CRITICAL_SECTION_BEGIN( )      UTILS_ENTER_CRITICAL_SECTION( )
#endif /* !CRITICAL_SECTION_BEGIN */
#ifndef CRITICAL_SECTION_END
#define CRITICAL_SECTION_END( )        UTILS_EXIT_CRITICAL_SECTION( )
#endif /* !CRITICAL_SECTION_END */

