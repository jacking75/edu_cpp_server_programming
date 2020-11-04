#pragma once

#include <memory>
#include "TcpNetwork.h"
#include "Define.h"
#include "ErrorCode.h"
#include "PacketID.h"
#include "UserManager.h"
#include "RoomManager.h"


namespace ChatServerLib
{		
	class PacketProcess
	{
		using PacketInfo = NServerNetLib::RecvPacketInfo;
		typedef NServerNetLib::ERROR_CODE(PacketProcess::* PacketFunc)(PacketInfo);
		PacketFunc PacketFuncArray[(int)NCommon::PACKET_ID::MAX];

	public:
		PacketProcess();
		~PacketProcess();

		NServerNetLib::ERROR_CODE NtfSysConnectSession(PacketInfo packetInfo);
		NServerNetLib::ERROR_CODE NtfSysCloseSession(PacketInfo packetInfo);

		void Init(NServerNetLib::TcpNetwork* pNetwork, UserManager* pUserMgr, RoomManager* pRoomMgr, NServerNetLib::ServerConfig pConfig);
		void Process(PacketInfo packetInfo);

		NServerNetLib::ERROR_CODE Login(PacketInfo packetInfo);
		NServerNetLib::ERROR_CODE RoomEnter(PacketInfo packetInfo);
		NServerNetLib::ERROR_CODE RoomLeave(PacketInfo packetInfo);
		NServerNetLib::ERROR_CODE RoomChat(PacketInfo packetInfo);
		NServerNetLib::ERROR_CODE MatchUser(PacketInfo packetInfo);
		NServerNetLib::ERROR_CODE GamePut(PacketInfo packetInfo);
		NServerNetLib::ERROR_CODE GameReady(PacketInfo packetInfo);

	private:
		NServerNetLib::TcpNetwork* m_pRefNetwork;
		UserManager* m_pRefUserMgr;
		RoomManager* m_pRefRoomMgr;
	};
}