#pragma once

#include <memory>
#include "TcpNetwork.h"
#include "Define.h"
#include "ErrorCode.h"


namespace ChatServerLib
{		
	class PacketProcess
	{
		using PacketInfo = NServerNetLib::RecvPacketInfo;


	public:
		PacketProcess();
		~PacketProcess();


		void Init(NServerNetLib::TcpNetwork* pNetwork);

		void Process(PacketInfo packetInfo);


	private:
		NServerNetLib::TcpNetwork* m_pRefNetwork;

	};
}