#include "spdlog/spdlog.h"
#include "wpcap/pcap.h"
#include "core/Thread.h"
#include "core/Mutex.h"
#include "file/pcap/PcapReader.h"
#include "network/PacketSocket.h"
#include <iostream>

int main()
{
    spdlog::info("Welcome to TraceReplay");

    spdlog::info("Available network interface:");
    std::vector<std::string> intfs = network::PacketSocket::ListNetworkInterfaces();
    for (size_t i = 0; i < intfs.size(); i++) {
        spdlog::info("Interface {:2d}: {}", i, intfs[i]);
    }

    spdlog::info("Select network interface:");
    int i = -1;
    do {
        std::cin >> i;
        if (i < 0) {
            spdlog::info("You want to leave. Exit program"); 
            return 0;
        }
        else if ((size_t) i >= intfs.size()) {
            spdlog::error("Select a smaller index than {}", intfs.size());
        }
    } while ((size_t) i >= intfs.size());

    try
    {
        network::PacketSocket socket(intfs[i]);

        for (int i = 0; i < 100; i++) {
            std::shared_ptr<core::Packet> packet = socket.Receive();
            if (packet == nullptr) {
                spdlog::info("No packet received");
                continue;
            }

            spdlog::info("Packet #{:8d} with len {:8d}. USE-COUNT: {}", socket.GetLastPacketNumber(), packet->GetLen(), packet.use_count());
        }
    }
    catch (const std::exception& e)
    {
        spdlog::error("Exception occurred: {}", e.what());
    }

    /*


    try
    {
        std::unique_ptr<file::PacketReader> reader(new file::pcap::PcapReader("D:\\Projekte\\TraceReplay\\test_files\\someip.pcap"));
        std::shared_ptr<core::Packet> packet;
        while(reader != nullptr) {
            packet = reader->Read();
            if (packet == nullptr) {
                spdlog::info("End of file reached");
                break;
            }

            spdlog::info("Packet #{:8d} with len {:8d}. USE-COUNT: {}", reader->GetLastPacketNumber(), packet->GetLen(), packet.use_count());

        } 
    }
    catch (const std::exception& e)
    {
        spdlog::error("Exception occurred: {}", e.what());
    }
    */
    spdlog::info("Goodbye");
}