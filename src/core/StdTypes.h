#pragma once

#include <cstdint>
#include <chrono>

uint16_t    swap16Bit(uint16_t value);
int16_t     swap16Bit(int16_t value);
void        swap16Bit(void* value);
void        swap16Bit(uint8_t* value);

uint32_t    swap32Bit(uint32_t value);
int32_t     swap32Bit(int32_t value);
void        swap32Bit(void* value);
void        swap32Bit(uint8_t* value);

uint64_t    swap64Bit(uint64_t value);
int64_t     swap64Bit(int64_t value);
void        swap64Bit(void* value);
void        swap64Bit(uint8_t* value);

uint16_t    AsUInt16(const uint8_t* value, bool swapBytes = false);
uint32_t    AsUInt32(const uint8_t* value, bool swapBytes = false);
uint64_t    AsUInt64(const uint8_t* value, bool swapBytes = false);

int16_t     AsSInt16(const uint8_t* value, bool swapBytes = false);
int32_t     AsSInt32(const uint8_t* value, bool swapBytes = false);
int64_t     AsSInt64(const uint8_t* value, bool swapBytes = false);

using Time = std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds>;