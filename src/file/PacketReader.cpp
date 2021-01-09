#include "PacketReader.h"
#include <cassert>

using namespace file;

PacketReader::PacketReader(const std::string& fileName) : 
    packetNumber(0) {
    this->ifStream.open(fileName, std::ifstream::binary);
}

PacketReader::~PacketReader() {
    ifStream.close();
}

std::shared_ptr<core::Packet> PacketReader::Read() {
    assert(false);
    return std::shared_ptr<core::Packet>(nullptr);
}

