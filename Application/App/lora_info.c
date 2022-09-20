// Copyright 2021 Blues Inc.  All rights reserved.
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.

#include "main.h"
#include "LoRaMac.h"
#include "lora_info.h"
#include "sys_app.h" // APP_PRINTF

// Private
static LoraInfo_t loraInfo = {0, 0};

// Init lora info structure
void LoraInfo_Init(void)
{
    loraInfo.ActivationMode = 0;
    loraInfo.Region = 0;
    loraInfo.ClassB = 0;
    loraInfo.Kms = 0;

#ifdef  REGION_AS923
    loraInfo.Region |= (1 << LORAMAC_REGION_AS923) ;
#endif
#ifdef  REGION_AU915
    loraInfo.Region |= (1 << LORAMAC_REGION_AU915) ;
#endif
#ifdef  REGION_CN470
    loraInfo.Region |= (1 << LORAMAC_REGION_CN470) ;
#endif
#ifdef  REGION_CN779
    loraInfo.Region |= (1 << LORAMAC_REGION_CN779) ;
#endif
#ifdef  REGION_EU433
    loraInfo.Region |= (1 << LORAMAC_REGION_EU433) ;
#endif
#ifdef  REGION_EU868
    loraInfo.Region |= (1 << LORAMAC_REGION_EU868) ;
#endif
#ifdef  REGION_KR920
    loraInfo.Region |= (1 << LORAMAC_REGION_KR920) ;
#endif
#ifdef  REGION_IN865
    loraInfo.Region |= (1 << LORAMAC_REGION_IN865) ;
#endif
#ifdef  REGION_US915
    loraInfo.Region |= (1 << LORAMAC_REGION_US915) ;
#endif
#ifdef  REGION_RU864
    loraInfo.Region |= (1 << LORAMAC_REGION_RU864) ;
#endif

    if (loraInfo.Region == 0) {
        Error_Handler();
    }

#if ( LORAMAC_CLASSB_ENABLED == 1 )
    loraInfo.ClassB = 1;
#elif !defined (LORAMAC_CLASSB_ENABLED)
#error LORAMAC_CLASSB_ENABLED not defined ( shall be <0 or 1> )
#endif // LORAMAC_CLASSB_ENABLED 

#if (!defined (LORAWAN_KMS) || (LORAWAN_KMS == 0))
    loraInfo.Kms = 0;
    loraInfo.ActivationMode = 3;
#else // LORAWAN_KMS == 1
    loraInfo.Kms = 1;
    loraInfo.ActivationMode = ACTIVATION_BY_PERSONALIZATION + (OVER_THE_AIR_ACTIVATION << 1);
#endif // LORAWAN_KMS 

}

// Get a pointer to the info structure
LoraInfo_t *LoraInfo_GetPtr(void)
{
    return &loraInfo;
}

