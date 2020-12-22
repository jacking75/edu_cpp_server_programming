#include "PacketProcess.h"
#include "PacketDef.h"

//TODO 최흥배
// 패킷 핸들러 함수에 있는 코드들이 Room으로 옮길 수 있으면 그쪽으로 옮겨주세요
// 패킷 핸들어에 코드가 많이 있는 것 별로 좋지 않습니다.
//-> 해결
namespace OmokServerLib
{
	ERROR_CODE PacketProcess::GameReady(PacketInfo packetInfo)
	{
		OmokServerLib::PktGameReadyRes resPkt;

		//TODO 최흥배
		// 이 유저가 지금 방에 있는 유저인지 확인해봐야 하지 않나요?
		// 이 단계에서 room 객체도 얻는 것이 좋습니다.
		//-> 해결
		auto findResult = FindUserAndRoom(packetInfo.SessionIndex, OmokServerLib::PACKET_ID::GAME_START_RES, ERROR_CODE::USER_STATE_NOT_ROOM);

		if (findResult.has_value() == false)
		{
			return ERROR_CODE::USER_ROOM_FIND_ERROR;
		}

		auto pUser = findResult.value().first;
		auto pRoom = findResult.value().second;

		if (pUser->IsCurDomainInRoom() == false)
		{
			SendPacketSetError(packetInfo.SessionIndex, OmokServerLib::PACKET_ID::GAME_START_RES, ERROR_CODE::USER_STATE_NOT_ROOM);
			return ERROR_CODE::USER_STATE_NOT_ROOM;
		}

		auto checkReadyResult = m_pRefUserMgr->CheckReady(pUser);

		if (checkReadyResult != ERROR_CODE::NONE)
		{
			SendPacketSetError(packetInfo.SessionIndex, OmokServerLib::PACKET_ID::GAME_START_RES, checkReadyResult);
			return checkReadyResult;
		}

		pUser->SetReady();

		auto setUserStateResult = pRoom->UserSetGame(pUser, packetInfo.SessionIndex);

		if (setUserStateResult == ERROR_CODE::NONE)
		{
			strncpy_s(resPkt.UserID, (OmokServerLib::MAX_USER_ID_SIZE + 1), pUser->GetID().c_str(), OmokServerLib::MAX_USER_ID_SIZE);
			SendPacketFunc(packetInfo.SessionIndex, (short)OmokServerLib::PACKET_ID::GAME_START_RES, sizeof(resPkt), (char*)&resPkt);
		}
		else
		{
			SendPacketSetError(packetInfo.SessionIndex, OmokServerLib::PACKET_ID::GAME_START_RES, ERROR_CODE::NOT_READY_EXIST);
		}

		return ERROR_CODE::NONE;
	}
}
