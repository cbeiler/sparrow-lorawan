// Copyright 2021 Blues Inc.  All rights reserved.
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.

#include "sensor.h"

// Measure sensor values and retrieve data to be transmitted, returning true if success or false if failure.
bool sensorGetData(uint8_t *buf, uint8_t bufmaxlen, uint8_t *buflen, uint32_t *secsSuccess, uint32_t *secsFailure)
{
    // Set callback timings on either success or failure to send this sensor measurement
    *secsFailure = 10;
    *secsSuccess = 15;

    binBegin(buf, bufmaxlen, 0);
    binAppendInt8(1);               // "a"
    binAppendInt16(2);              // "b"
    static int messagesSent = 0;
    binAppendInt24(messagesSent++); // "c"
    binAppendReal16(-1.23);         // "temp"
    *buflen = binEnd();

    // Done
    APP_PRINTF("sensor retrieved %d bytes\r\n", *buflen);
    return true;

}
