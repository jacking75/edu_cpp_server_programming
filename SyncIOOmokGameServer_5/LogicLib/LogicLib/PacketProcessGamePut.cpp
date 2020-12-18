#include "PacketProcess.h"
#include "PacketDef.h"

namespace OmokServerLib
{
	ERROR_CODE PacketProcess::GamePut(PacketInfo packetInfo)
	{
		auto reqPkt = (NCommon::PktPutStoneReq*)packetInfo.pRefData;
		NCommon::PktPutStoneRes resPkt;
		NCommon::PktGameResultNtf gameResPkt;

		//TODO 최흥배
		//중복 코드
		//
		auto userInfo = m_pRefUserMgr->GetUser(packetInfo.SessionIndex);

		auto errorCode = userInfo.first;
		auto pUser = userInfo.second;
		
		if (errorCode != ERROR_CODE::NONE) 
		{
			SendPacketSetError(packetInfo.SessionIndex, NCommon::PACKET_ID::PUT_STONE_RES, errorCode);
			return errorCode;
		}

		if (pUser->IsCurDomainInGame() == false)
		{
			SendPacketSetError(packetInfo.SessionIndex, NCommon::PACKET_ID::PUT_STONE_RES, ERROR_CODE::USER_STATE_NOT_GAME);
			return errorCode;
		}

		auto pRoom = m_pRefRoomMgr->FindRoom(pUser->GetRoomIndex());
		//

		if (packetInfo.SessionIndex != pRoom.value()->m_OmokGame->m_TurnIndex)
		{
			SendPacketSetError(packetInfo.SessionIndex, NCommon::PACKET_ID::PUT_STONE_RES, ERROR_CODE::NOT_YOUR_TURN);
			return ERROR_CODE::NOT_YOUR_TURN;
		}
		
		auto putStoneResult = PutStone(pRoom.value(), reqPkt->x, reqPkt->y , packetInfo.SessionIndex);
		
		if (putStoneResult.first != ERROR_CODE::NONE)
		{
			SendPacketSetError(packetInfo.SessionIndex, NCommon::PACKET_ID::PUT_STONE_RES, putStoneResult.first);
			return putStoneResult.first;
		}

		auto nextTurnUserID = putStoneResult.second.c_str();

		auto endResult = pRoom.value()->m_OmokGame->CheckGameEnd(reqPkt->x, reqPkt->y);

		if (endResult == ERROR_CODE::NONE) //게임이 끝나지 않았다면 종료
		{
			strncpy_s(resPkt.UserID, (NCommon::MAX_USER_ID_SIZE + 1), nextTurnUserID , NCommon::MAX_USER_ID_SIZE);
			SendPacketFunc(packetInfo.SessionIndex, (short)NCommon::PACKET_ID::PUT_STONE_RES, sizeof(resPkt), (char*)&resPkt);
			pRoom.value()->m_OmokGame->SetUserTurnTime();
			return ERROR_CODE::NONE;
		}

		auto winUserID = SetWinUserID(pRoom.value(), endResult);

		strncpy_s(gameResPkt.UserID, (NCommon::MAX_USER_ID_SIZE + 1), winUserID.c_str(), NCommon::MAX_USER_ID_SIZE);
		SendPacketFunc(packetInfo.SessionIndex, (short)NCommon::PACKET_ID::GAME_END_RESULT, sizeof(gameResPkt), (char*)&gameResPkt);
		pRoom.value()->NotifyGameResult(packetInfo.SessionIndex, winUserID.c_str());
		pRoom.value()->EndGame();
		pRoom.value()->Clear();

		return ERROR_CODE::NONE;
	}

	std::pair<ERROR_CODE,std::string> PacketProcess::PutStone(Room* pRoom, int x , int y , int sessionIndex)
	{
		auto result = pRoom->m_OmokGame->GamePutStone(x,y);

		if (result != ERROR_CODE::NONE)
		{
			return { result,"" };
		}

		auto pNextTurnUser = sessionIndex == pRoom->m_UserList[0]->GetSessioIndex() ? pRoom->m_UserList[1] : pRoom->m_UserList[0];
		pRoom->m_OmokGame->m_TurnIndex = pNextTurnUser->GetSessioIndex();

		auto nextTurnUserID = pNextTurnUser->GetID().c_str();
		pRoom->NotifyPutStoneInfo(sessionIndex, nextTurnUserID);

		return { ERROR_CODE::NONE ,nextTurnUserID };
		
	}

	std::string PacketProcess::SetWinUserID(Room* pRoom , ERROR_CODE endResult)
	{
		auto blackUserID = pRoom->m_UserList[pRoom->m_OmokGame->m_BlackStoneUserIndex]->GetID();
		auto whiteUserID = pRoom->m_UserList[pRoom->m_OmokGame->m_WhiteStoneUserIndex]->GetID();

		auto winUserID = blackUserID;

		if (endResult == ERROR_CODE::GAME_RESULT_WHITE_WIN)
		{
			winUserID = whiteUserID;
		}

		return winUserID;
	}
}