#include "PcapReader.h"
#include "PcapFormat.h"
#include <exception>

using namespace file::pcap;
using namespace core;

PcapReader::PcapReader(const std::string& fileName) :
    PacketReader(fileName) {

    PcapHeader pcapHeader;
    memset(&pcapHeader, 0, sizeof(pcapHeader));
    ifStream.read((char*)&pcapHeader, sizeof(pcapHeader));

    // Check based on magic number if timestamp is in microseconds and 
    // if we need to swap pcapHeader and pcapPacketHeader
    switch (pcapHeader.magicNumber) {
    case 0xA1B2C3D4:
        swapBytes = false;
        timeInMicros = true;
        break;
    case 0xD4C3B2A1:
        swapBytes = true;
        timeInMicros = true;
        break;
    case 0xA1B23C4D:
        swapBytes = false;
        timeInMicros = false;
        break;
    case 0x4D3CB2A1:
        swapBytes = true;
        timeInMicros = false;
        break;
    default:
        throw std::runtime_error("Unknown magic-cookie in PCAP file");
    }

    if (swapBytes) {
        maxSnapLength = swap32Bit(pcapHeader.maxSnapLength);
        timezone = swap32Bit(pcapHeader.timezone);
    }
    else {
        maxSnapLength = pcapHeader.maxSnapLength;
        timezone = pcapHeader.timezone;
    }
}

std::shared_ptr<core::Packet> PcapReader::Read() {

    PcapPacketHeader packetHeader;
    ifStream.read((char*)&packetHeader, sizeof(packetHeader));
    if (!ifStream || ifStream.eof()) {
        return std::shared_ptr<core::Packet>(nullptr);
    }

    // Swap Bytes in the packet Header if necessary
    if (swapBytes) {
        swap32Bit(&packetHeader.packetLength);
        swap32Bit(&packetHeader.timestampSeconds);
        swap32Bit(&packetHeader.timestampMicroSeconds);
    }

    // Sanity check
    if (packetHeader.packetLength > maxSnapLength) {
        throw std::runtime_error("PCAP packet longer than max. snap length");
    }

    // Prepare Timestamp
    Time time(std::chrono::seconds(0));
    time += std::chrono::seconds(packetHeader.timestampSeconds);
    if (timeInMicros) {
        time += std::chrono::microseconds(packetHeader.timestampMicroSeconds);
    }
    else {
        time += std::chrono::nanoseconds(packetHeader.timestampMicroSeconds);
    }
    time += std::chrono::seconds(timezone);

    // Put everything in the new packet
    packetNumber++;

    std::shared_ptr<core::Packet> packet(new Packet(packetHeader.packetLength, packetHeader.packetLength, time));

    // Now we can put some data into the packet
    ifStream.read((char*) packet->GetData(), packet->GetLen());
    return packet;
}