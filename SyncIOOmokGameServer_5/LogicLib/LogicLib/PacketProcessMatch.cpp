#include "PacketProcess.h"
#include "PacketDef.h"


namespace OmokServerLib
{
	ERROR_CODE PacketProcess::MatchUser(PacketInfo packetInfo)
	{
		OmokServerLib::PktMatchRes resPkt;

		//TODO 최흥배
		// 다른 패킷 처리 함수에서 비슷한 코드가 있습니다. 중복을 제거해주세요
		//-> 25줄에서 방을 찾을때 다른 함수들과 달리 FindProperRoom 함수로 방을 찾아 다른 함수들과 달라 묶기에 무리가 있는 것 같습니다.
		auto userInfo = m_pRefUserMgr->GetUser(packetInfo.SessionIndex);

		auto errorCode = userInfo.first;
		auto pUser = userInfo.second;

		if (errorCode != ERROR_CODE::NONE) 
		{
			SendPacketSetError(packetInfo.SessionIndex, OmokServerLib::PACKET_ID::MATCH_USER_RES, errorCode);
			return errorCode;
		}

		auto pRoom = m_pRefRoomMgr->FindProperRoom();

		if (pRoom.has_value() == false) 
		{
			SendPacketSetError(packetInfo.SessionIndex, OmokServerLib::PACKET_ID::ROOM_ENTER_RES, ERROR_CODE::MATCHING_FAIL);
			return errorCode;
		}
		//

		pRoom.value()->EnterUser(pUser);
		pUser->EnterRoom(pRoom.value()->GetIndex());
		pRoom.value()->NotifyEnterUserInfo(packetInfo.SessionIndex, pUser->GetID().c_str());

		SendPacketFunc(packetInfo.SessionIndex, (short)OmokServerLib::PACKET_ID::MATCH_USER_RES, sizeof(resPkt), (char*)&resPkt);
		
		return ERROR_CODE::NONE;

	}
}