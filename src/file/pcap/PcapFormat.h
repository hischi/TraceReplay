#pragma once

#include "../../core/StdTypes.h"

#pragma pack (push,1)
typedef struct {
    uint32_t    magicNumber;
    uint16_t    versionMajor;
    uint16_t    versionMinor;
    int32_t     timezone;
    uint32_t    timestampAccuracy;
    uint32_t    maxSnapLength;
    uint32_t    network;
} PcapHeader;

typedef struct {
    uint32_t    timestampSeconds;
    uint32_t    timestampMicroSeconds;
    uint32_t    packetLength;
    uint32_t    originalLength;
} PcapPacketHeader;
#pragma pack(pop)