#include "PacketProcess.h"
#include "PacketDef.h"

namespace OmokServerLib
{
	ERROR_CODE PacketProcess::GameReady(PacketInfo packetInfo)
	{
		NCommon::PktGameReadyRes resPkt;

		auto userInfo = m_pRefUserMgr->GetUser(packetInfo.SessionIndex);
		auto errorCode = userInfo.first;
		auto pUser = userInfo.second;

		if (errorCode != ERROR_CODE::NONE)
		{
			SendPacketSetError(packetInfo.SessionIndex, NCommon::PACKET_ID::GAME_START_RES, errorCode);
			return errorCode;
		}
		auto checkReadyResult = m_pRefUserMgr->CheckReady(pUser);

		if (checkReadyResult != ERROR_CODE::NONE)
		{
			SendPacketSetError(packetInfo.SessionIndex, NCommon::PACKET_ID::GAME_START_RES, checkReadyResult);
			return errorCode;
		}

		pUser->SetReady();
		
		auto setUserStateResult = UserSetGame(pUser, packetInfo.SessionIndex);

		if (setUserStateResult == ERROR_CODE::NONE)
		{
			strncpy_s(resPkt.UserID, (NCommon::MAX_USER_ID_SIZE + 1), pUser->GetID().c_str(), NCommon::MAX_USER_ID_SIZE);
			SendPacketFunc(packetInfo.SessionIndex, (short)NCommon::PACKET_ID::GAME_START_RES, sizeof(resPkt), (char*)&resPkt);
		}
		else
		{
			SendPacketSetError(packetInfo.SessionIndex, NCommon::PACKET_ID::GAME_START_RES, ERROR_CODE::NOT_READY_EXIST);
		}

		return ERROR_CODE::NONE;
	}



	ERROR_CODE PacketProcess::UserSetGame(User* pUser, int sessionIndex)
	{
		auto pRoom = m_pRefRoomMgr->FindRoom(pUser->GetRoomIndex());
		auto pOpponentUser = sessionIndex == pRoom.value()->m_UserList[0]->GetSessioIndex() ? pRoom.value()->m_UserList[1] : pRoom.value()->m_UserList[0];
		auto roomUserIndex = sessionIndex == pRoom.value()->m_UserList[0]->GetSessioIndex() ? 1 : 0;

		if (pOpponentUser->IsCurDomainInReady() == true)
		{
			pOpponentUser->SetGame();
			pUser->SetGame();
			pRoom.value()->SetRoomStateGame();

			//검은돌 랜덤 선정
			pRoom.value()->m_OmokGame->m_BlackStoneUserIndex = std::abs(1 - roomUserIndex);
			pRoom.value()->m_OmokGame->m_WhiteStoneUserIndex = roomUserIndex;
			pRoom.value()->m_OmokGame->m_TurnIndex = sessionIndex;
			pRoom.value()->m_OmokGame->init();
			pRoom.value()->NotifyGameStart(sessionIndex, pUser->GetID().c_str());

			pRoom.value()->m_OmokGame->SetUserTurnTime();

			return ERROR_CODE::NONE;
			
		}
		else
		{
			return ERROR_CODE::NOT_READY_EXIST;
		}

		return ERROR_CODE::UNASSIGNED_ERROR;
	}
}
