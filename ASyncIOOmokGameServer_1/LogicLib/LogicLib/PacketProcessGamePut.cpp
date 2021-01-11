#include "PacketProcess.h"
#include "User.h"
#include "Room.h"
#include "PacketDef.h"

namespace OmokServerLib
{
	ERROR_CODE PacketProcess::GamePut(PacketInfo packetInfo)
	{
		auto reqPkt = (OmokServerLib::PktPutStoneReq*)packetInfo.pRefData;

		OmokServerLib::PktPutStoneRes resPkt;
		resPkt.Id = (short)OmokServerLib::PACKET_ID::PUT_STONE_RES;
		resPkt.TotalSize = sizeof(OmokServerLib::PktPutStoneRes);

		OmokServerLib::PktGameResultNtf gameResPkt;
		resPkt.Id = (short)OmokServerLib::PACKET_ID::GAME_END_RESULT;
		resPkt.TotalSize = sizeof(OmokServerLib::PktGameResultNtf);

		auto findResult = FindUserAndRoom(packetInfo.SessionIndex, OmokServerLib::PACKET_ID::PUT_STONE_RES, ERROR_CODE::USER_STATE_NOT_ROOM);

		if (findResult.has_value() == false)
		{
			return ERROR_CODE::USER_ROOM_FIND_ERROR;
		}

		auto pUser = findResult.value().first;
		auto pRoom = findResult.value().second;

		if (pUser->IsCurDomainInGame() == false)
		{
			SendPacketSetError(packetInfo.SessionIndex, OmokServerLib::PACKET_ID::PUT_STONE_RES, ERROR_CODE::USER_STATE_NOT_GAME);
			return ERROR_CODE::USER_STATE_NOT_GAME;
		}

		if (packetInfo.SessionIndex != pRoom->m_OmokGame->m_TurnIndex)
		{
			SendPacketSetError(packetInfo.SessionIndex, OmokServerLib::PACKET_ID::PUT_STONE_RES, ERROR_CODE::NOT_YOUR_TURN);
			return ERROR_CODE::NOT_YOUR_TURN;
		}
		
		auto putStoneResult = PutStone(pRoom, reqPkt->x, reqPkt->y , packetInfo.SessionIndex);
		
		if (putStoneResult.first != ERROR_CODE::NONE)
		{
			SendPacketSetError(packetInfo.SessionIndex, OmokServerLib::PACKET_ID::PUT_STONE_RES, putStoneResult.first);
			return putStoneResult.first;
		}

		auto nextTurnUserID = putStoneResult.second.c_str();

		auto endResult = pRoom->m_OmokGame->CheckGameEnd(reqPkt->x, reqPkt->y);

		if (endResult == ERROR_CODE::NONE) //게임이 끝나지 않았다면 종료
		{
			strncpy_s(resPkt.UserID, (OmokServerLib::MAX_USER_ID_SIZE + 1), nextTurnUserID , OmokServerLib::MAX_USER_ID_SIZE);
			SendPacketFunc(packetInfo.SessionIndex,sizeof(resPkt), (char*)&resPkt);
			pRoom->m_OmokGame->SetUserTurnTime();
			return ERROR_CODE::NONE;
		}

		auto winUserID = SetWinUserID(pRoom, endResult);

		strncpy_s(gameResPkt.UserID, (OmokServerLib::MAX_USER_ID_SIZE + 1), winUserID.c_str(), OmokServerLib::MAX_USER_ID_SIZE);
		SendPacketFunc(packetInfo.SessionIndex, sizeof(gameResPkt), (char*)&gameResPkt);
		pRoom->NotifyGameResult(packetInfo.SessionIndex, winUserID.c_str());
		pRoom->EndGame();
		pRoom->Clear();

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