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

		auto pRoom = m_pRefRoomMgr->FindProperRoom();

		if (pRoom.has_value() == false) 
		{
			roomResPkt.SetError(ERROR_CODE::MATCHING_FAIL);
			m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)NCommon::PACKET_ID::ROOM_ENTER_RES, sizeof(roomResPkt), (char*)&roomResPkt);
			return errorCode;
		}

		pRoom.value()->EnterUser(pUser);
		pUser->EnterRoom(pRoom.value()->GetIndex());
		pRoom.value()->NotifyEnterUserInfo(packetInfo.SessionIndex, pUser->GetID().c_str());

 		m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)NCommon::PACKET_ID::MATCH_USER_RES, sizeof(resPkt), (char*)&resPkt);
		
		return ERROR_CODE::NONE;

	}
}