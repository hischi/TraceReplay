#include "PacketSocket.h"
#include <stdexcept>
#include <cassert>
#include "wpcap/pcap.h"

using namespace core;

network::PacketSocket::PacketSocket(const std::string& intf) :
	intfHandle(nullptr), packetNumber(0)
{
	char errorBuffer[PCAP_ERRBUF_SIZE];
	if ((intfHandle = pcap_open_live(intf.c_str(), MaxPacketSize, 1, 1000, errorBuffer)) == NULL)
	{
		throw std::invalid_argument("No network-interface found to open packet-socket");
	}
}

network::PacketSocket::~PacketSocket()
{
	if(intfHandle != nullptr)
		pcap_close((pcap_t*) intfHandle);
}

std::shared_ptr<Packet> network::PacketSocket::Receive()
{
	assert(intfHandle != nullptr);

	pcap_pkthdr hdr;
	const uint8_t* data = pcap_next((pcap_t*) intfHandle, &hdr);

	if (data == NULL) {
		return std::shared_ptr<core::Packet>(nullptr);
	}
	else {
		Time timeStamp(std::chrono::seconds(0));
		timeStamp += std::chrono::seconds(hdr.ts.tv_sec) + std::chrono::microseconds(hdr.ts.tv_usec);

		packetNumber++;
		return std::shared_ptr<core::Packet>(new core::Packet(hdr.caplen, hdr.caplen, data, timeStamp));
	}
}

void network::PacketSocket::Send(std::shared_ptr<Packet> packet)
{
	assert(intfHandle != nullptr);
	if (packet->GetLen() == 0)
		return;

	if (pcap_sendpacket((pcap_t*)intfHandle, packet->GetData(), packet->GetLen()) != 0)
		throw std::runtime_error("Can't send on network-interface");
}

std::vector<std::string> network::PacketSocket::ListNetworkInterfaces()
{
	std::vector<std::string> results;
	char errbuf[PCAP_ERRBUF_SIZE];

	// Get all interfaces
	pcap_if_t* allIntfs;
	if (pcap_findalldevs(&allIntfs, errbuf) == -1)
	{
		throw std::runtime_error("Error in pcap_findalldevs");
	}

	for (pcap_if_t* intf = allIntfs; intf; intf = intf->next)
	{
		results.push_back(std::string(intf->name));
	}

	// Clean
	pcap_freealldevs(allIntfs);
	return results;
}
