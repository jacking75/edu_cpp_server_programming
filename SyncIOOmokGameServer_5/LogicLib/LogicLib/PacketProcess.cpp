#include <iostream>
#include "PacketProcess.h"
#include "PacketDef.h"

namespace OmokServerLib
{	
	void PacketProcess::Init(NServerNetLib::TcpNetwork* pNetwork, UserManager* pUserMgr, RoomManager* pRoomMgr, RedisManager* pRedisMgr, NServerNetLib::Logger* pLogger, ConnectedUserManager* m_pConUserMgr, NServerNetLib::ServerConfig pConfig)
	{		
		m_pRefUserMgr = pUserMgr;
		m_pRefRoomMgr = pRoomMgr;
		m_pRefRedisMgr = pRedisMgr;
		m_pRefLogger = pLogger;
		m_pRefConUserMgr = m_pConUserMgr;

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

	void PacketProcess::StateCheck()
	{
		m_pRefConUserMgr->LoginCheck();
		m_pRefRoomMgr->CheckRoomGameTime();
	}

	ERROR_CODE PacketProcess::NtfSysConnectSession(PacketInfo packetInfo)
	{
		m_pRefConUserMgr->SetConnectSession(packetInfo.SessionIndex);
		return ERROR_CODE::NONE;
	}
	
	ERROR_CODE PacketProcess::NtfSysCloseSession(PacketInfo packetInfo)
	{
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

		m_pRefConUserMgr->SetDisConnectSession(packetInfo.SessionIndex);
		m_pRefLogger->info("NtfSysCloseSession | Close Session [{}]", packetInfo.SessionIndex);

		return ERROR_CODE::NONE;
	}

	void PacketProcess::SendPacketSetError(int sessionIndex, NCommon::PACKET_ID packetID, ERROR_CODE errorCode)
	{
		if (packetID == NCommon::PACKET_ID::PUT_STONE_RES)
		{
			NCommon::PktPutStoneRes resPkt;

			resPkt.SetError(errorCode);
			SendPacketFunc(sessionIndex, (short)packetID, sizeof(resPkt), (char*)&resPkt);
			return;
		}
		else if (packetID == NCommon::PACKET_ID::GAME_START_RES)
		{
			NCommon::PktGameReadyRes resPkt;

			resPkt.SetError(errorCode);
			SendPacketFunc(sessionIndex, (short)packetID, sizeof(resPkt), (char*)&resPkt);
			return;
		}
		else if (packetID == NCommon::PACKET_ID::MATCH_USER_RES)
		{
			NCommon::PktMatchRes resPkt;

			resPkt.SetError(errorCode);
			SendPacketFunc(sessionIndex, (short)packetID, sizeof(resPkt), (char*)&resPkt);
			return;

		}
		else if (packetID == NCommon::PACKET_ID::ROOM_ENTER_RES)
		{
			NCommon::PktRoomEnterRes resPkt;

			resPkt.SetError(errorCode);
			SendPacketFunc(sessionIndex, (short)packetID, sizeof(resPkt), (char*)&resPkt);
			return;
		}
		else if (packetID == NCommon::PACKET_ID::ROOM_LEAVE_RES)
		{
			NCommon::PktRoomLeaveRes resPkt;

			resPkt.SetError(errorCode);
			SendPacketFunc(sessionIndex, (short)packetID, sizeof(resPkt), (char*)&resPkt);
			return;
		}
		else if (packetID == NCommon::PACKET_ID::ROOM_CHAT_RES)
		{
			NCommon::PktRoomChatRes resPkt;

			resPkt.SetError(errorCode);
			SendPacketFunc(sessionIndex, (short)packetID, sizeof(resPkt), (char*)&resPkt);
			return;
		}

	}

}