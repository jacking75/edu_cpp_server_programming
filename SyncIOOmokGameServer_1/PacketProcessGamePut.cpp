#include <tuple>
#include <iostream>
#include "User.h"
#include "UserManager.h"
#include "PacketProcess.h"
#include "PacketDef.h"

namespace ChatServerLib
{
	NServerNetLib::ERROR_CODE PacketProcess::GamePut(PacketInfo packetInfo)
	{
		auto reqPkt = (NCommon::PktPutStoneReq*)packetInfo.pRefData;
		NCommon::PktPutStoneRes resPkt;
		NCommon::PktGameResultNtf gameResPkt;

		auto userInfo = m_pRefUserMgr->GetUser(packetInfo.SessionIndex);

		auto errorCode = userInfo.first;
		auto pUser = userInfo.second;
		
		if (errorCode != NServerNetLib::ERROR_CODE::NONE) 
		{
			resPkt.SetError(errorCode);
			m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)NCommon::PACKET_ID::PUT_STONE_RES, sizeof(resPkt), (char*)&resPkt);
			return errorCode;
		}

		auto userIndex = pUser->GetIndex();
		auto room = m_pRefRoomMgr->FindRoom(pUser->GetRoomIndex());

		if (userIndex != room->m_TurnIndex)
		{
			resPkt.SetError(NServerNetLib::ERROR_CODE::NOT_YOUR_TURN);
			m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)NCommon::PACKET_ID::PUT_STONE_RES, sizeof(resPkt), (char*)&resPkt);
			return NServerNetLib::ERROR_CODE::NOT_YOUR_TURN;
		}

		auto result = room->OmokGame->GamePutStone(userIndex, reqPkt->x, reqPkt->y);

		if (result != NServerNetLib::ERROR_CODE::NONE)
		{
			m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)NCommon::PACKET_ID::PUT_STONE_RES, sizeof(resPkt), (char*)&resPkt);
			return errorCode;
		}

		room->NotifyPutStoneInfo(userIndex, pUser->GetID().c_str(), reqPkt->x, reqPkt->y);

		auto endResult = room->OmokGame->CheckGameEnd(reqPkt->x, reqPkt->y);

		if (endResult == NServerNetLib::ERROR_CODE::NONE) //게임이 끝나지 않았다면 종료
		{			
			auto nextTurnIndex = room->m_UserList[0]->GetIndex() == pUser->GetIndex() ? room->m_UserList[1]->GetIndex() : room->m_UserList[0]->GetIndex();
			room->m_TurnIndex = nextTurnIndex;
			strncpy_s(resPkt.UserID, (NCommon::MAX_USER_ID_SIZE + 1), m_pRefUserMgr->GetUser(nextTurnIndex).second->GetID().c_str(), NCommon::MAX_USER_ID_SIZE);
			m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)NCommon::PACKET_ID::PUT_STONE_RES, sizeof(resPkt), (char*)&resPkt);
			return NServerNetLib::ERROR_CODE::NONE;
		}


		auto blackUserID = m_pRefUserMgr->GetUser(room->m_BlackStoneUserIndex).second->GetID().c_str();
		auto whiteUserID = room->m_UserList[0]->GetID().c_str() == blackUserID ? room->m_UserList[1]->GetID().c_str() : room->m_UserList[0]->GetID().c_str();

		if (endResult == NServerNetLib::ERROR_CODE::GAME_RESULT_BLACK_WIN)
		{
			strncpy_s(gameResPkt.UserID, (NCommon::MAX_USER_ID_SIZE + 1), blackUserID, NCommon::MAX_USER_ID_SIZE);
			m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)NCommon::PACKET_ID::GAME_END_RESULT, sizeof(gameResPkt), (char*)&gameResPkt);
			room->NotifyGameResult(userIndex, blackUserID);
			room->Clear();
		}
		else
		{
			strncpy_s(gameResPkt.UserID, (NCommon::MAX_USER_ID_SIZE + 1), whiteUserID, NCommon::MAX_USER_ID_SIZE);
			m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)NCommon::PACKET_ID::GAME_END_RESULT, sizeof(gameResPkt), (char*)&gameResPkt);
			room->NotifyGameResult(userIndex, whiteUserID);
			room->Clear();
		}
	}
}