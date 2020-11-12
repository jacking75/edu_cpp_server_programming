#pragma once


#include "TcpNetwork.h"
#include "Define.h"
#include "PacketID.h"
#include "UserManager.h"
#include "RoomManager.h"


namespace ChatServerLib
{		
	class PacketProcess
	{
		using PacketInfo = NServerNetLib::RecvPacketInfo;
		typedef ERROR_CODE(PacketProcess::* PacketFunc)(PacketInfo);
		PacketFunc PacketFuncArray[(int)NCommon::PACKET_ID::MAX];

	public:
		PacketProcess() = default;
		~PacketProcess() = default;

		ERROR_CODE NtfSysConnectSession(PacketInfo packetInfo);

		ERROR_CODE NtfSysCloseSession(PacketInfo packetInfo);

		void Init(NServerNetLib::TcpNetwork* pNetwork, UserManager* pUserMgr, RoomManager* pRoomMgr, NServerNetLib::ServerConfig pConfig);

		void Process(PacketInfo packetInfo);

		ERROR_CODE Login(PacketInfo packetInfo);

		ERROR_CODE RoomEnter(PacketInfo packetInfo);

		ERROR_CODE RoomLeave(PacketInfo packetInfo);

		ERROR_CODE RoomChat(PacketInfo packetInfo);

		ERROR_CODE MatchUser(PacketInfo packetInfo);

		ERROR_CODE GamePut(PacketInfo packetInfo);

		ERROR_CODE GameReady(PacketInfo packetInfo);

	private:

		NServerNetLib::TcpNetwork* m_pRefNetwork;

		UserManager* m_pRefUserMgr;

		RoomManager* m_pRefRoomMgr;
	};
}