#pragma once

#include "../core/Packet.h"
#include <fstream>

namespace file {

    class PacketReader {

    public: 
        PacketReader(const std::string& fileName);
        virtual ~PacketReader();

        virtual std::shared_ptr<core::Packet> Read();
        uint64_t GetLastPacketNumber() const {
            return packetNumber;
        }

    protected:
        std::ifstream ifStream;
        uint64_t packetNumber;
    };
}