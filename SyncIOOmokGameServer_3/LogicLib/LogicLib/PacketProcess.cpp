#include <iostream>
#include "PacketProcess.h"

namespace ChatServerLib
{	
	void PacketProcess::Init(NServerNetLib::TcpNetwork* pNetwork, UserManager* pUserMgr, RoomManager* pRoomMgr,NServerNetLib::ServerConfig pConfig)
	{		
		m_pRefUserMgr = pUserMgr;
		m_pRefRoomMgr = pRoomMgr;

		using netLibPacketId = NServerNetLib::PACKET_ID;
		using commonPacketId = NCommon::PACKET_ID;

		for (int i = 0; i < (int)commonPacketId::MAX; ++i)
		{
			PacketFuncArray[i] = nullptr;
		}

		PacketFuncArray[(int)netLibPacketId::NTF_SYS_CONNECT_SESSION] = &PacketProcess::NtfSysConnectSession;
		PacketFuncArray[(int)netLibPacketId::NTF_SYS_CLOSE_SESSION] = &PacketProcess::NtfSysCloseSession;
		PacketFuncArray[(int)commonPacketId::ROOM_ENTER_REQ] = &PacketProcess::RoomEnter;
		PacketFuncArray[(int)commonPacketId::LOGIN_IN_REQ] = &PacketProcess::Login;
		PacketFuncArray[(int)commonPacketId::ROOM_LEAVE_REQ] = &PacketProcess::RoomLeave;
		PacketFuncArray[(int)commonPacketId::ROOM_CHAT_REQ] = &PacketProcess::RoomChat;
		PacketFuncArray[(int)commonPacketId::MATCH_USER_REQ] = &PacketProcess::MatchUser;
		PacketFuncArray[(int)commonPacketId::PUT_STONE_REQ] = &PacketProcess::GamePut;
		PacketFuncArray[(int)commonPacketId::GAME_START_REQ] = &PacketProcess::GameReady;

	}
	
	void PacketProcess::Process(PacketInfo packetInfo)
	{
		auto packetId = packetInfo.PacketId;

		if (packetId < 0 || packetId > (int)NCommon::PACKET_ID::MAX)
		{
			return;
		}

		if (PacketFuncArray[packetId] == nullptr)
		{
			return;
		}

		(this->*PacketFuncArray[packetId])(packetInfo);

	}

	ChatServerLib::ERROR_CODE PacketProcess::NtfSysConnectSession(PacketInfo packetInfo)
	{
		std::cout << "Client Connect [ " << packetInfo.SessionIndex << " ]" << std::endl;

		return ERROR_CODE::NONE;
	}
	
	ERROR_CODE PacketProcess::NtfSysCloseSession(PacketInfo packetInfo)
	{
		std::cout << "Close Session [ " << packetInfo.SessionIndex << " ]" << std::endl;
		auto pUser = std::get<1>(m_pRefUserMgr->GetUser(packetInfo.SessionIndex));

		if (pUser) 
		{
			auto pRoom = m_pRefRoomMgr->FindRoom(pUser->GetRoomIndex());
			if (pRoom.has_value() == true)
			{
				pRoom.value()->LeaveUser(pUser->GetIndex(), pUser->GetSessioIndex(), pUser->GetID().c_str());
			}
			m_pRefUserMgr->RemoveUser(packetInfo.SessionIndex);
		}

		return ERROR_CODE::NONE;
	}

}