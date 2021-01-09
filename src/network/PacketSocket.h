#pragma once

#include <string>
#include <memory>
#include <vector>
#include "../src/core/Packet.h"

namespace network {

    class PacketSocket {

    public:
        static const size_t MaxPacketSize = 65536;

    // Constructors
    public:
        PacketSocket(const std::string& intf);
        PacketSocket(const PacketSocket&) = delete;
        PacketSocket(PacketSocket&& other) noexcept :
            intfHandle(other.intfHandle), packetNumber(other.packetNumber) {
            other.intfHandle = nullptr;
        }
        ~PacketSocket();

    // Assignments:
    public:
        PacketSocket& operator=(const PacketSocket&) = delete;
        PacketSocket& operator=(PacketSocket&& other) noexcept {
            this->intfHandle = other.intfHandle;
            other.intfHandle = nullptr;
            this->packetNumber = other.packetNumber;
            return *this;
        }

    // Methods:
    public:
        std::shared_ptr<core::Packet> Receive();
        void Send(std::shared_ptr < core::Packet > packet);
        uint64_t GetLastPacketNumber() const {
            return packetNumber;
        }

        static std::vector<std::string> ListNetworkInterfaces();

    private:
        void* intfHandle;
        uint64_t packetNumber;
    };
}