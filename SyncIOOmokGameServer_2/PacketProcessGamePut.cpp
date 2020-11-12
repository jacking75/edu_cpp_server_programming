#include "PacketProcess.h"
#include "PacketDef.h"

namespace ChatServerLib
{
	ERROR_CODE PacketProcess::GamePut(PacketInfo packetInfo)
	{
		auto reqPkt = (NCommon::PktPutStoneReq*)packetInfo.pRefData;
		NCommon::PktPutStoneRes resPkt;
		NCommon::PktGameResultNtf gameResPkt;

		auto userInfo = m_pRefUserMgr->GetUser(packetInfo.SessionIndex);

		auto errorCode = userInfo.first;
		auto pUser = userInfo.second;
		
		if (errorCode != ERROR_CODE::NONE) 
		{
			resPkt.SetError(errorCode);
			m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)NCommon::PACKET_ID::PUT_STONE_RES, sizeof(resPkt), (char*)&resPkt);
			return errorCode;
		}

		if (pUser->IsCurDomainInGame() == false)
		{
			resPkt.SetError(ERROR_CODE::USER_STATE_NOT_GAME);
			m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)NCommon::PACKET_ID::PUT_STONE_RES, sizeof(resPkt), (char*)&resPkt);
			return errorCode;
		}

		auto pRoom = m_pRefRoomMgr->FindRoom(pUser->GetRoomIndex());

		if (packetInfo.SessionIndex != pRoom.value()->m_OmokGame->m_TurnIndex)
		{
			resPkt.SetError(ERROR_CODE::NOT_YOUR_TURN);
			m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)NCommon::PACKET_ID::PUT_STONE_RES, sizeof(resPkt), (char*)&resPkt);
			return ERROR_CODE::NOT_YOUR_TURN;
		}

		auto result = pRoom.value()->m_OmokGame->GamePutStone(reqPkt->x, reqPkt->y);
		pRoom.value()->m_OmokGame->printTest();

		if (result != ERROR_CODE::NONE)
		{
			resPkt.SetError(result);
			m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)NCommon::PACKET_ID::PUT_STONE_RES, sizeof(resPkt), (char*)&resPkt);
			return errorCode;
		}

		auto pNextTurnUser = packetInfo.SessionIndex == pRoom.value()->m_UserList[0]->GetSessioIndex() ? pRoom.value()->m_UserList[1] : pRoom.value()->m_UserList[0];
		pRoom.value()->m_OmokGame->m_TurnIndex = pNextTurnUser->GetSessioIndex();

		auto nextTurnUserID = pNextTurnUser->GetID().c_str();
		pRoom.value()->NotifyPutStoneInfo(packetInfo.SessionIndex, nextTurnUserID);

		auto endResult = pRoom.value()->m_OmokGame->CheckGameEnd(reqPkt->x, reqPkt->y);

		if (endResult == ERROR_CODE::NONE) //게임이 끝나지 않았다면 종료
		{			
			strncpy_s(resPkt.UserID, (NCommon::MAX_USER_ID_SIZE + 1), nextTurnUserID, NCommon::MAX_USER_ID_SIZE);
			m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)NCommon::PACKET_ID::PUT_STONE_RES, sizeof(resPkt), (char*)&resPkt);
			return ERROR_CODE::NONE;
		}

		auto blackUserID = m_pRefUserMgr->GetUser(pRoom.value()->m_OmokGame->m_BlackStoneUserIndex).second->GetID().c_str();
		auto whiteUserID = m_pRefUserMgr->GetUser(pRoom.value()->m_OmokGame->m_WhiteStoneUserIndex).second->GetID().c_str();

		auto winUserID = blackUserID;

		if (endResult == ERROR_CODE::GAME_RESULT_WHITE_WIN)
		{
			winUserID = whiteUserID;
		}

		strncpy_s(gameResPkt.UserID, (NCommon::MAX_USER_ID_SIZE + 1), winUserID, NCommon::MAX_USER_ID_SIZE);
		m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)NCommon::PACKET_ID::GAME_END_RESULT, sizeof(gameResPkt), (char*)&gameResPkt);
		pRoom.value()->NotifyGameResult(packetInfo.SessionIndex, winUserID);
		pRoom.value()->Clear();
	}
}