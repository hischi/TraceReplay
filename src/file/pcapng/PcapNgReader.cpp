#include "PcapNgReader.h"
#include "PcapNgFormat.h"

using namespace file::pcapng;
using namespace core;

PcapNgReader::PcapNgReader(std::string& fileName) : 
    PacketReader(fileName) {
    
    // Parse section header block
    if (!ParseSectionHeaderBlock())
        throw std::runtime_error("Invalid PCAPNG file (error in section header block)");

    // Parse interface description block
    if (!ParseInterfaceDescriptionBlock())
        throw std::runtime_error("Invalid PCAPNG file (error in interface description block)");
}

std::shared_ptr<core::Packet> PcapNgReader::Read() {

    PcapNgBlock block;
    ifStream.read((char*)&block, sizeof(block));
    if (!ifStream || ifStream.eof())
        return std::shared_ptr<core::Packet>(nullptr);

    // Swap Bytes of the block header if necessary
    if (swapBytes) {
        swap32Bit(&block.blockType);
        swap32Bit(&block.blockTotalLength);
    }

    switch (block.blockType)
    {
    case PcapNgBlockTypes::sectionHeader:
    case PcapNgBlockTypes::interfaceDescription:
        throw std::runtime_error("Invalid PCAPNG file (unexpected header instead of packet)");

    case PcapNgBlockTypes::enhancedPacket:
        return ParseEnhancedPacketBlock(block.blockTotalLength);
    case PcapNgBlockTypes::simplePacket:
        return ParseSimplePacketBlock(block.blockTotalLength);
    case PcapNgBlockTypes::packet:
        return ParsePacketBlock(block.blockTotalLength);

    default:
        throw std::runtime_error("Invalid PCAPNG file (unknown block type)");
    }
}

bool PcapNgReader::ParseSectionHeaderBlock() {
    PcapNgSectionHeaderBlock sectionHeaderBlock;
    memset(&sectionHeaderBlock, 0, sizeof(sectionHeaderBlock));
    ifStream.read((char*)&sectionHeaderBlock, sizeof(sectionHeaderBlock));

    // Check for PcapNg sectionHeader block type
    if (sectionHeaderBlock.block.blockType != PcapNgBlockTypes::sectionHeader) {
        return false;
    }

    // Check based on magic number if we need to swap bytes in headers
    switch (sectionHeaderBlock.magicNumber)
    {
    case 0x1A2B3C4D:
        swapBytes = false;
        break;

    case 0x4D3C2B1A:
        swapBytes = true;
        swap32Bit(&sectionHeaderBlock.block.blockTotalLength);
        break;
    default:
        return false;
    }

    // Read till end of block
    ifStream.seekg(sectionHeaderBlock.block.blockTotalLength - sizeof(sectionHeaderBlock), std::ios::cur);

    return (bool)(ifStream);
}

bool PcapNgReader::ParseInterfaceDescriptionBlock() {
    PcapNgInterfaceDescriptionBlock ifDescriptionBlock;
    memset(&ifDescriptionBlock, 0, sizeof(ifDescriptionBlock));
    ifStream.read((char*)&ifDescriptionBlock, sizeof(ifDescriptionBlock));

    // Swap bytes if necessary
    if (swapBytes) {
        swap32Bit(&ifDescriptionBlock.block.blockType);
        swap32Bit(&ifDescriptionBlock.block.blockTotalLength);
        swap16Bit(&ifDescriptionBlock.linkType);
        swap32Bit(&ifDescriptionBlock.snapLen);
    }

    // Check for PcapNg interface description block type
    if (ifDescriptionBlock.block.blockType != PcapNgBlockTypes::interfaceDescription) {
        return false;
    }

    // Check for Ethernet type
    if (ifDescriptionBlock.linkType != 1) {
        return false;
    }

    maxSnapLength = ifDescriptionBlock.snapLen;
    timeResolution = 6; // Default if no tsResol option is available

    // Iterate over options
    uint32_t remOptionsLen = ifDescriptionBlock.block.blockTotalLength - sizeof(ifDescriptionBlock) - 4;
    while (remOptionsLen > 0) {
        PcapNgOption option;
        ifStream.read((char*)&option, sizeof(option));
        remOptionsLen -= sizeof(option);

        if (swapBytes) {
            swap16Bit(&option.optionCode);
            swap16Bit(&option.optionLength);
        }

        switch (option.optionCode) {
        case PcapNgIfOptionCodes::if_tsresol:
            uint8_t tsresol;
            ifStream.read((char*)&tsresol, sizeof(tsresol));
            remOptionsLen -= sizeof(tsresol);

            if (tsresol & 0x80) {
                // Not supported 
                return false;
            }
            else {
                timeResolution = tsresol;
            }
            break;

        // Timezones could be realized here but are not defined by PcapNg format in a useable manner

        default:
            if (option.optionLength & 0x0003) {
                option.optionLength = (option.optionLength & 0xFFFC) + 4;
            }
            ifStream.seekg(option.optionLength, std::ios::cur);
        }
    }

    // Read till end of block
    ifStream.seekg(ifDescriptionBlock.block.blockTotalLength - sizeof(ifDescriptionBlock), std::ios::cur);
}

std::shared_ptr<core::Packet> PcapNgReader::ParseEnhancedPacketBlock(uint32_t blockLen) {
    PcapNgEnhancedPacketBlock packetBlock;
    ifStream.read((char*)&packetBlock + sizeof(PcapNgBlock), sizeof(packetBlock) - sizeof(PcapNgBlock));
    if (!ifStream || ifStream.eof())
        return std::shared_ptr<core::Packet>(nullptr);

    if (swapBytes) {
        swap32Bit(&packetBlock.capturedLen);
        swap32Bit(&packetBlock.timestampHigh);
        swap32Bit(&packetBlock.timestampLow);
    }

    // Sanity check
    if (packetBlock.capturedLen > maxSnapLength) {
        throw std::runtime_error("PCAPNG packet longer than max. snap length");
    }

    // Prepare timestamp
    uint64_t timestamp = (((uint64_t)packetBlock.timestampHigh) << 32) | packetBlock.timestampLow;
    Time time(std::chrono::seconds(0));
    if (timeResolution > 9) {
        time += std::chrono::nanoseconds((uint64_t)(timestamp / pow(10, timeResolution - 9)));
    }
    else if (timeResolution == 9) {
        time += std::chrono::nanoseconds(timestamp);
    }
    else if (timeResolution == 6) {
        time += std::chrono::microseconds(timestamp);
    }
    else {
        time += std::chrono::nanoseconds((uint64_t)(timestamp * pow(10, 9 - timeResolution)));
    }

    // Put everything in the new packet
    packetNumber++;

    std::shared_ptr<core::Packet> packet(new Packet(packetBlock.capturedLen, packetBlock.capturedLen, time));

    // Now we can put some data into the packet
    ifStream.read((char*)packet->GetData(), packet->GetLen());

    // Skip rest of block
    ifStream.seekg(blockLen - sizeof(PcapNgEnhancedPacketBlock) - packet->GetLen(), std::ios::cur);
    return packet;
}

std::shared_ptr<core::Packet> PcapNgReader::ParseSimplePacketBlock(uint32_t blockLen) {
    PcapNgSimplePacketBlock packetBlock;
    ifStream.read((char*)&packetBlock + sizeof(PcapNgBlock), sizeof(packetBlock) - sizeof(PcapNgBlock));
    if (!ifStream || ifStream.eof())
        return std::shared_ptr<core::Packet>(nullptr);

    uint32_t capturedLen = blockLen - sizeof(PcapNgSimplePacketBlock) - 4;

    // Sanity check
    if (capturedLen > maxSnapLength) {
        throw std::runtime_error("PCAPNG packet longer than max. snap length");
    }

    // Prepare timestamp (simple packets don't come with time so set it to 0)    
    Time time(std::chrono::seconds(0));

    // Put everything in the new packet
    packetNumber++;
    std::shared_ptr<core::Packet> packet(new Packet(capturedLen, capturedLen, time));

    // Now we can put some data into the packet
    ifStream.read((char*)packet->GetData(), packet->GetLen());

    // Skip rest of block
    ifStream.seekg(blockLen - sizeof(PcapNgEnhancedPacketBlock) - packet->GetLen(), std::ios::cur);
    return packet;
}

std::shared_ptr<core::Packet> PcapNgReader::ParsePacketBlock(uint32_t blockLen) {
    PcapNgPacketBlock packetBlock;
    ifStream.read((char*)&packetBlock + sizeof(PcapNgBlock), sizeof(packetBlock) - sizeof(PcapNgBlock));
    if (!ifStream || ifStream.eof())
        return std::shared_ptr<core::Packet>(nullptr);

    if (swapBytes) {
        swap32Bit(&packetBlock.capturedLen);
        swap32Bit(&packetBlock.timestampHigh);
        swap32Bit(&packetBlock.timestampLow);
    }

    // Sanity check
    if (packetBlock.capturedLen > maxSnapLength) {
        throw std::runtime_error("PCAPNG packet longer than max. snap length");
    }

    // Prepare timestamp
    uint64_t timestamp = (((uint64_t)packetBlock.timestampHigh) << 32) | packetBlock.timestampLow;
    Time time(std::chrono::seconds(0));
    if (timeResolution > 9) {
        time += std::chrono::nanoseconds((uint64_t)(timestamp / pow(10, timeResolution - 9)));
    }
    else if (timeResolution == 9) {
        time += std::chrono::nanoseconds(timestamp);
    }
    else if (timeResolution == 6) {
        time += std::chrono::microseconds(timestamp);
    }
    else {
        time += std::chrono::nanoseconds((uint64_t)(timestamp * pow(10, 9 - timeResolution)));
    }

    // Put everything in the new packet
    packetNumber++;
    std::shared_ptr<core::Packet> packet(new Packet(packetBlock.capturedLen, packetBlock.capturedLen, time));

    // Now we can put some data into the packet
    ifStream.read((char*)packet->GetData(), packet->GetLen());

    // Skip rest of block
    ifStream.seekg(blockLen - sizeof(PcapNgEnhancedPacketBlock) - packet->GetLen(), std::ios::cur);
    return packet;
}