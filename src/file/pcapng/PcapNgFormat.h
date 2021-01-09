#pragma once

#include "../src/core/StdTypes.h"

#pragma pack (push,1)
typedef struct {
    uint16_t optionCode;
    uint16_t optionLength;
} PcapNgOption;

typedef struct {
    uint32_t    blockType;
    uint32_t    blockTotalLength;
    // Options
    // Block Total Length
} PcapNgBlock;

typedef struct {
    PcapNgBlock     block;
    uint32_t        magicNumber;
    uint16_t        versionMajor;
    uint16_t        versionMinor;
    uint64_t        sectionLength;
    // Options
    // Block Total Length
} PcapNgSectionHeaderBlock;

typedef struct {
    PcapNgBlock     block;
    uint16_t        linkType;
    uint16_t        reserved;
    uint32_t        snapLen;
    // Options
    // Block Total Length
} PcapNgInterfaceDescriptionBlock;

typedef struct {
    PcapNgBlock     block;
    uint32_t        interfaceId;
    uint32_t        timestampHigh;
    uint32_t        timestampLow;
    uint32_t        capturedLen;
    uint32_t        packetLen;
    // Packet Data
    // Options
    // Block Total Length
} PcapNgEnhancedPacketBlock;

typedef struct {
    PcapNgBlock     block;
    uint32_t        packetLen;
    // Packet Data
    // Options
    // Block Total Length
} PcapNgSimplePacketBlock;

typedef struct {
    PcapNgBlock     block;
    uint16_t        interfaceId;
    uint16_t        dropsCount;
    uint32_t        timestampHigh;
    uint32_t        timestampLow;
    uint32_t        capturedLen;
    uint32_t        packetLen;
    // Packet Data
    // Options
    // Block Total Length
} PcapNgPacketBlock;

#pragma pack(pop)

typedef enum {
    opt_endofopt = 0,
    opt_comment = 1,
} PcapNgOptionCodes;

typedef enum {
    shb_hardware = 2,
    shb_os = 3,
    shb_userappl = 4
} PcapNgShbOptionCodes;

typedef enum {
    if_name = 2,
    if_description = 3,
    if_IPv4addr = 4,
    if_IPv6addr = 5,
    if_MACaddr = 6,
    if_EUIaddr = 7,
    if_speed = 8,
    if_tsresol = 9,
    if_tzone = 10,
    if_filter = 11,
    if_os = 12,
    if_fcslen = 13,
    if_tsoffset = 14
} PcapNgIfOptionCodes;

typedef enum {
    epb_flags = 2,
    epb_hash = 3,
    epb_dropcount = 4
} PcapNgEpbOptionCodes;

typedef enum {
    pack_flags = 2,
    pack_hash = 3
} PcapNgPackOptionCodes;

typedef enum {
    sectionHeader = 0x0A0D0D0A,
    interfaceDescription = 0x00000001,
    enhancedPacket = 0x00000006,
    simplePacket = 0x00000003,
    packet = 0x00000002
} PcapNgBlockTypes;