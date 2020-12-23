#include "PacketProcess.h"
#include "User.h"
#include "Room.h"
#include "PacketDef.h"

namespace OmokServerLib
{
	ERROR_CODE PacketProcess::GameReady(PacketInfo packetInfo)
	{
		OmokServerLib::PktGameReadyRes resPkt;

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
