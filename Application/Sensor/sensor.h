// Copyright 2021 Blues Inc.  All rights reserved.
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.

#pragma once

// bin.c definitions
#include "app.h"

// sensor.c
bool sensorGetData(uint8_t *buf, uint8_t bufmaxlen, uint8_t *buflen, uint32_t *secsSuccess, uint32_t *secsFailure);

