#pragma once

#include "../src/file/PacketReader.h"

namespace file {
    namespace pcap {

        class PcapReader : public PacketReader {

        public:
            PcapReader(const std::string& fileName);
            virtual std::shared_ptr<core::Packet> Read();

        protected:
            bool swapBytes;
            bool timeInMicros;
            int32_t timezone;
            uint32_t maxSnapLength;
        };


    }
}