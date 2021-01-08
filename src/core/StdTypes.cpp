#include "StdTypes.h"
#include <stdlib.h>

uint16_t    swap16Bit(uint16_t value) {
    return _byteswap_ushort(value);
}

int16_t     swap16Bit(int16_t value) {
    return _byteswap_ushort(value);
}

void        swap16Bit(void* value) {
    *((uint16_t*)value) = _byteswap_ushort(*((uint16_t*)value));
}

void        swap16Bit(uint8_t* value) {
    *((uint16_t*)value) = _byteswap_ushort(*((uint16_t*)value));
}

uint32_t    swap32Bit(uint32_t value) {
    return _byteswap_ulong(value);
}

int32_t     swap32Bit(int32_t value) {
    return _byteswap_ulong(value);
}

void        swap32Bit(void* value) {
    *((uint32_t*)value) = _byteswap_ulong(*((uint32_t*)value));
}

void        swap32Bit(uint8_t* value) {
    *((uint32_t*)value) = _byteswap_ulong(*((uint32_t*)value));
}

uint64_t    swap64Bit(uint64_t value) {
    return _byteswap_uint64(value);
}

int64_t     swap64Bit(int64_t value) {
    return _byteswap_uint64(value);
}

void        swap64Bit(void* value) {
    *((uint64_t*)value) = _byteswap_uint64(*((uint64_t*)value));
}

void        swap64Bit(uint8_t* value) {
    *((uint64_t*)value) = _byteswap_uint64(*((uint64_t*)value));
}

uint16_t    AsUInt16(const uint8_t* value, bool swapBytes) {
    if (swapBytes)
        return swap16Bit(*((uint16_t*)value));
    else
        return *((uint16_t*)value);
}

uint32_t    AsUInt32(const uint8_t* value, bool swapBytes) {
    if (swapBytes)
        return swap32Bit(*((uint32_t*)value));
    else
        return *((uint32_t*)value);
}

uint64_t    AsUInt64(const uint8_t* value, bool swapBytes) {
    if (swapBytes)
        return swap64Bit(*((uint64_t*)value));
    else
        return *((uint64_t*)value);
}

int16_t    AsSInt16(const uint8_t* value, bool swapBytes) {
    if (swapBytes)
        return swap16Bit(*((int16_t*)value));
    else
        return *((int16_t*)value);
}

int32_t    AsSInt32(const uint8_t* value, bool swapBytes) {
    if (swapBytes)
        return swap32Bit(*((int32_t*)value));
    else
        return *((int32_t*)value);
}

int64_t    AsSInt64(const uint8_t* value, bool swapBytes) {
    if (swapBytes)
        return swap64Bit(*((int64_t*)value));
    else
        return *((int64_t*)value);
}