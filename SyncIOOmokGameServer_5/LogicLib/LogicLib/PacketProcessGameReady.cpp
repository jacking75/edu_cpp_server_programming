#include "PacketProcess.h"
#include "PacketDef.h"

//TODO 최흥배
// 패킷 핸들러 함수에 있는 코드들이 Room으로 옮길 수 있으면 그쪽으로 옮겨주세요
// 패킷 핸들어에 코드가 많이 있는 것 별로 좋지 않습니다.
//-> 아래의 UserSetGame 함수를 Room 으로 옮기면 Room이 RoomManager를 가지고있어야하는데 이과정에서 오류발생.. 다시고민..
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

		auto setUserStateResult = UserSetGame(pUser, packetInfo.SessionIndex);

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
