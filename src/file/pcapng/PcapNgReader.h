#pragma once

#include "../src/file/PacketReader.h"

namespace file {
    namespace pcapng {

        class PcapNgReader : public PacketReader {

        public:
            PcapNgReader(std::string& fileName);
            virtual std::shared_ptr<core::Packet> Read();

        private:
            bool ParseSectionHeaderBlock();
            bool ParseInterfaceDescriptionBlock();

            std::shared_ptr<core::Packet> ParseEnhancedPacketBlock(uint32_t blockLen);
            std::shared_ptr<core::Packet> ParseSimplePacketBlock(uint32_t blockLen);
            std::shared_ptr<core::Packet> ParsePacketBlock(uint32_t blockLen);

        protected:
            bool swapBytes;
            uint32_t maxSnapLength;
            uint16_t timeResolution;

        };
    }

}