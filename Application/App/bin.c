// Copyright 2021 Blues Inc.  All rights reserved.
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.

#include "app.h"

// State
static uint8_t binMaxFlags;
static uint8_t binFlagsFound;
static uint64_t binFlags;
static uint8_t *binRecord;
static uint8_t binRecordAllocLen;
static uint8_t binRecordLen;
static bool binSuccess;

// Forwards
uint32_t as_uint32(float x);
float as_real32(uint32_t x);
uint16_t real32_to_real16(float x);
float real16_to_real32(uint16_t x);

// Begin, with 0-4 flag bytes
void binBegin(uint8_t *buf, uint8_t len, uint8_t flagbytes)
{
    binMaxFlags = flagbytes*8;
    binRecord = buf;
    binRecordAllocLen = len;
    binRecordLen = 0;
    binFlags = 0;
    binSuccess = true;
}

// Done, returning the number of bytes written (0 if failure)
uint8_t binEnd()
{
    if (binMaxFlags == 64) {
        binAppendInt64(binFlags);
    } else if (binMaxFlags == 32) {
        binAppendInt32((int32_t)binFlags);
    } else if (binMaxFlags == 16) {
        binAppendInt16((int16_t)binFlags);
    } else if (binMaxFlags == 8) {
        binAppendInt8((int8_t)binFlags);
    }
    return binSuccess ? binRecordLen : 0;
}

// Append methods
void binAppendBool(bool value)
{
    if (binFlagsFound >= binMaxFlags) {
        binSuccess = false;
        return;
    }
    // The flags are stored least significant bit to highest
    if (value) {
        binFlags |= 1 << binFlagsFound;
    }
    binFlagsFound++;
}
void binAppendByte(uint8_t databyte)
{
    if ((binRecordLen + sizeof(uint8_t)) > binRecordAllocLen) {
        binSuccess = false;
        return;
    }
    binRecord[binRecordLen++] = databyte;
}
void binAppendInt8(int8_t value)
{
    binAppendByte(value & 0x0ff);
}
void binAppendInt16(int16_t value)
{
    binAppendByte(value & 0x0ff);
    value = value >> 8;
    binAppendByte(value & 0x0ff);
}
void binAppendInt24(int32_t value)
{
    binAppendByte(value & 0x0ff);
    value = value >> 8;
    binAppendByte(value & 0x0ff);
    value = value >> 8;
    binAppendByte(value & 0x0ff);
}
void binAppendInt32(int32_t value)
{
    binAppendByte(value & 0x0ff);
    value = value >> 8;
    binAppendByte(value & 0x0ff);
    value = value >> 8;
    binAppendByte(value & 0x0ff);
    value = value >> 8;
    binAppendByte(value & 0x0ff);
}
void binAppendInt64(int64_t value)
{
    binAppendByte(value & 0x0ff);
    value = value >> 8;
    binAppendByte(value & 0x0ff);
    value = value >> 8;
    binAppendByte(value & 0x0ff);
    value = value >> 8;
    binAppendByte(value & 0x0ff);
    value = value >> 8;
    binAppendByte(value & 0x0ff);
    value = value >> 8;
    binAppendByte(value & 0x0ff);
    value = value >> 8;
    binAppendByte(value & 0x0ff);
    value = value >> 8;
    binAppendByte(value & 0x0ff);
}
void binAppendString(char *p, uint32_t len)
{
    uint32_t actualLen = strlen(p);
    for (int i=0; i<len; i++) {
        if (i < actualLen) {
            binAppendByte((uint8_t)p[i]);
        } else {
            binAppendByte(0);
        }
    }
}
void binAppendBytes(uint8_t *data, uint32_t datalen, uint32_t templateLen)
{
    for (int i=0; i<templateLen; i++) {
        if (i < datalen) {
            binAppendByte((uint8_t)data[i]);
        } else {
            binAppendByte(0);
        }
    }
}
void binAppendReal16(float number)
{
    if (sizeof(float) != 4) {
        binSuccess = false;
        return;
    }
    uint16_t value = real32_to_real16(number);
    binAppendByte(value & 0x0ff);
    value = value >> 8;
    binAppendByte(value & 0x0ff);
}
void binAppendReal32(float number)
{
    if (sizeof(float) != 4) {
        binSuccess = false;
        return;
    }
    // Detect little-endian or big-endian
    uint32_t test_var = 1;
    uint8_t *test_endian = (uint8_t *) &test_var;
    bool little_endian = test_endian[0] == 1;
    // Send them in little-endian order
    uint8_t realBytes[4];
    memcpy(realBytes, &number, 4);
    if (little_endian) {
        binAppendByte(realBytes[0]);
        binAppendByte(realBytes[1]);
        binAppendByte(realBytes[2]);
        binAppendByte(realBytes[3]);
    } else {
        binAppendByte(realBytes[3]);
        binAppendByte(realBytes[2]);
        binAppendByte(realBytes[1]);
        binAppendByte(realBytes[0]);
    }
}
void binAppendReal64(double number)
{
    if (sizeof(double) != 8) {
        binSuccess = false;
        return;
    }
    // Detect little-endian or big-endian
    uint32_t test_var = 1;
    uint8_t *test_endian = (uint8_t *) &test_var;
    bool little_endian = test_endian[0] == 1;
    // Send them in little-endian order
    uint8_t realBytes[8];
    memcpy(realBytes, &number, 8);
    if (little_endian) {
        binAppendByte(realBytes[0]);
        binAppendByte(realBytes[1]);
        binAppendByte(realBytes[2]);
        binAppendByte(realBytes[3]);
        binAppendByte(realBytes[4]);
        binAppendByte(realBytes[5]);
        binAppendByte(realBytes[6]);
        binAppendByte(realBytes[7]);
    } else {
        binAppendByte(realBytes[7]);
        binAppendByte(realBytes[6]);
        binAppendByte(realBytes[5]);
        binAppendByte(realBytes[4]);
        binAppendByte(realBytes[3]);
        binAppendByte(realBytes[2]);
        binAppendByte(realBytes[1]);
        binAppendByte(realBytes[0]);
    }
}

// Methods to convert back and forth between real32 and IEEE-754 FP16 format which we refer to as real16
uint32_t as_uint32(float x)
{
    uint32_t a;
    memcpy(&a, &x, sizeof(a));
    return a;
}
float as_real32(uint32_t x)
{
    float a;
    memcpy(&a, &x, sizeof(a));
    return a;
}

// IEEE-754 16-bit floating-point format (without infinity):
// 1-5-10, exp-15, +-131008.0, +-6.1035156E-5, +-5.9604645E-8, 3.311 digits
uint16_t real32_to_real16(float x)
{
    // round-to-nearest-even: add last bit after truncated mantissa
    uint32_t b = as_uint32(x)+0x00001000;
    // exponent
    uint32_t e = (b&0x7F800000)>>23;
    // mantissa; in line below: 0x007FF000 = 0x00800000-0x00001000 = decimal indicator flag - initial rounding
    uint32_t m = b&0x007FFFFF;
    // sign : normalized : denormalized : saturate
    return (b&0x80000000)>>16 | (e>112)*((((e-112)<<10)&0x7C00)|m>>13) | ((e<113)&(e>101))*((((0x007FF000+m)>>(125-e))+1)>>1) | (e>143)*0x7FFF;
}

// IEEE-754 16-bit floating-point format (without infinity):
// 1-5-10, exp-15, +-131008.0, +-6.1035156E-5, +-5.9604645E-8, 3.311 digits
float real16_to_real32(uint16_t x)
{
    // exponent
    uint32_t e = (x&0x7C00)>>10;
    // mantissa
    uint32_t m = (x&0x03FF)<<13;
    // evil log2 bit hack to count leading zeros in denormalized format
    uint32_t v = as_uint32((float)m)>>23;
    // sign : normalized : denormalized
    return as_real32((x&0x8000)<<16 | (e!=0)*((e+112)<<23|m) | ((e==0)&(m!=0))*((v-37)<<23|((m<<(150-v))&0x007FE000)));
}
