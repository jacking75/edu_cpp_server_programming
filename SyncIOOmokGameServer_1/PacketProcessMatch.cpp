#include "PacketProcess.h"
#include "PacketDef.h"


namespace ChatServerLib
{
	ERROR_CODE PacketProcess::MatchUser(PacketInfo packetInfo)
	{
		NCommon::PktMatchRes resPkt;
		NCommon::PktRoomEnterRes roomResPkt;
		auto userInfo = m_pRefUserMgr->GetUser(packetInfo.SessionIndex);

		auto errorCode = userInfo.first;
		auto pUser = userInfo.second;

		if (errorCode != ERROR_CODE::NONE) 
		{
			resPkt.SetError(errorCode);
			m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)NCommon::PACKET_ID::MATCH_USER_RES, sizeof(resPkt), (char*)&resPkt);
			return errorCode;
		}

		auto userIndex = pUser->GetIndex();

		auto pRoom = m_pRefRoomMgr->FindProperRoom();
		if (pRoom == nullptr) 
		{
			roomResPkt.SetError(ERROR_CODE::MATCHING_FAIL);
			m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)NCommon::PACKET_ID::ROOM_ENTER_RES, sizeof(roomResPkt), (char*)&roomResPkt);
			return errorCode;
		}

		pRoom->EnterUser(pUser);
		pUser->EnterRoom(pRoom->GetIndex());
		pRoom->NotifyEnterUserInfo(packetInfo.SessionIndex, pUser->GetID().c_str());

		//TODO :  검은돌 랜덤 선정
		pRoom->m_BlackStoneUserIndex = packetInfo.SessionIndex;
		pRoom->m_TurnIndex = packetInfo.SessionIndex;
		pRoom->OmokGame->init();

 		m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)NCommon::PACKET_ID::MATCH_USER_RES, sizeof(resPkt), (char*)&resPkt);
		//pRoom->SendToAllUser((short)NCommon::PACKET_ID::MATCH_USER_RES, sizeof(resPkt), (char*)&resPkt);
		
		return ERROR_CODE::NONE;

	}
}