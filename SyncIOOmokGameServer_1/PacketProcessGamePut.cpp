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

		auto room = m_pRefRoomMgr->FindRoom(pUser->GetRoomIndex());

		if (packetInfo.SessionIndex != room->m_TurnIndex)
		{
			resPkt.SetError(ERROR_CODE::NOT_YOUR_TURN);
			m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)NCommon::PACKET_ID::PUT_STONE_RES, sizeof(resPkt), (char*)&resPkt);
			return ERROR_CODE::NOT_YOUR_TURN;
		}

		auto result = room->OmokGame->GamePutStone(reqPkt->x, reqPkt->y);
		room->OmokGame->printTest();
		if (result != ERROR_CODE::NONE)
		{
			resPkt.SetError(result);
			m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)NCommon::PACKET_ID::PUT_STONE_RES, sizeof(resPkt), (char*)&resPkt);
			return errorCode;
		}
		auto nextTurnIndex = room->m_UserList[0]->GetSessioIndex() == packetInfo.SessionIndex ? room->m_UserList[1]->GetSessioIndex() : room->m_UserList[0]->GetSessioIndex();
		room->m_TurnIndex = nextTurnIndex;
		auto nextTurnUser = m_pRefUserMgr->GetUser(nextTurnIndex).second->GetID().c_str();
		room->NotifyPutStoneInfo(packetInfo.SessionIndex, nextTurnUser);

		auto endResult = room->OmokGame->CheckGameEnd(reqPkt->x, reqPkt->y);

		if (endResult == ERROR_CODE::NONE) //게임이 끝나지 않았다면 종료
		{			
			strncpy_s(resPkt.UserID, (NCommon::MAX_USER_ID_SIZE + 1), nextTurnUser, NCommon::MAX_USER_ID_SIZE);
			m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)NCommon::PACKET_ID::PUT_STONE_RES, sizeof(resPkt), (char*)&resPkt);
			return ERROR_CODE::NONE;
		}

		auto blackUserID = m_pRefUserMgr->GetUser(room->m_BlackStoneUserIndex).second->GetID().c_str();
		auto whiteUserID = room->m_UserList[0]->GetID().c_str() == blackUserID ? room->m_UserList[1]->GetID().c_str() : room->m_UserList[0]->GetID().c_str();

		if (endResult == ERROR_CODE::GAME_RESULT_BLACK_WIN)
		{
			strncpy_s(gameResPkt.UserID, (NCommon::MAX_USER_ID_SIZE + 1), blackUserID, NCommon::MAX_USER_ID_SIZE);
			m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)NCommon::PACKET_ID::GAME_END_RESULT, sizeof(gameResPkt), (char*)&gameResPkt);
			room->NotifyGameResult(packetInfo.SessionIndex, blackUserID);
			room->Clear();
		}
		else
		{
			strncpy_s(gameResPkt.UserID, (NCommon::MAX_USER_ID_SIZE + 1), whiteUserID, NCommon::MAX_USER_ID_SIZE);
			m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)NCommon::PACKET_ID::GAME_END_RESULT, sizeof(gameResPkt), (char*)&gameResPkt);
			room->NotifyGameResult(packetInfo.SessionIndex, whiteUserID);
			room->Clear();
		}
	}
}