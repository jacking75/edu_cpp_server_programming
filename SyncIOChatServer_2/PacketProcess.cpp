#include <cstring>
#include <iostream>
#include "PacketProcess.h"

namespace ChatServerLib
{	
	PacketProcess::PacketProcess() {}
	PacketProcess::~PacketProcess() {}

	void PacketProcess::Init(NServerNetLib::TcpNetwork* pNetwork)
	{		
		
	}
	
	void PacketProcess::Process(PacketInfo packetInfo)
	{
		auto packetId = packetInfo.PacketId;

		if (packetId == (short)NServerNetLib::PACKET_ID::NTF_SYS_CONNECT_SESSION) {
			std::cout << "Session Index [ " << packetInfo.SessionIndex << " ] " << "Client Connect" << std::endl;
		}
		else if (packetId == (short)NServerNetLib::PACKET_ID::NTF_SYS_RECV_SESSION) {
			std::cout << "RECEIVE Session Index [ " << packetInfo.SessionIndex << " ] [ " << packetInfo.PacketBodySize << " ] Bytes" << '\n';
		}
		else if (packetId == (short)NServerNetLib::PACKET_ID::NTF_SYS_CLOSE_SESSION) {
			std::cout << "Close Session Index [ " << packetInfo.SessionIndex << " ] " << "Close Session" << std::endl;
		}
		

	}

}