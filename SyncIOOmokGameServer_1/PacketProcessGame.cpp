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

		if (errorCode != NServerNetLib::ERROR_CODE::NONE) {
			resPkt.SetError(errorCode);
			m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)NCommon::PACKET_ID::PUT_STONE_RES, sizeof(resPkt), (char*)&resPkt);
			return errorCode;
		}

		auto userIndex = pUser->GetIndex();
		auto room = m_pRefRoomMgr->FindRoom(pUser->GetRoomIndex());

		auto result = room->OmokGame->GamePutStone(userIndex, reqPkt->x, reqPkt->y);

		if (result != NServerNetLib::ERROR_CODE::NONE)
		{	
			m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)NCommon::PACKET_ID::PUT_STONE_RES, sizeof(resPkt), (char*)&resPkt);
			return errorCode;
		}

		room->NotifyPutStoneInfo(userIndex, pUser->GetID().c_str(), reqPkt->x, reqPkt->y);

		auto endResult = room->OmokGame->CheckGameEnd(reqPkt->x, reqPkt->y);


		if (endResult == NServerNetLib::ERROR_CODE::GAME_RESULT_BLACK_WIN)
		{
			if (room->m_BlackStoneUserIndex == userIndex) 
			{
				strncpy_s(gameResPkt.UserID, (NCommon::MAX_USER_ID_SIZE + 1), pUser->GetID().c_str(), NCommon::MAX_USER_ID_SIZE);
				m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)NCommon::PACKET_ID::GAME_END_RESULT, sizeof(gameResPkt), (char*)&gameResPkt);
				room->NotifyGameResult(userIndex, pUser->GetID().c_str());
				
			}
			else
			{
				for (auto iter : room->m_UserList)
				{
					if (iter->GetIndex() != userIndex)
					{
						strncpy_s(gameResPkt.UserID, (NCommon::MAX_USER_ID_SIZE + 1), iter->GetID().c_str(), NCommon::MAX_USER_ID_SIZE);
					}
				}
				m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)NCommon::PACKET_ID::GAME_END_RESULT, sizeof(gameResPkt), (char*)&gameResPkt);
				room->NotifyGameResult(userIndex, gameResPkt.UserID);
			}
			room->Clear();
		}
		else
		{
			if (room->m_BlackStoneUserIndex != userIndex)
			{
				strncpy_s(gameResPkt.UserID, (NCommon::MAX_USER_ID_SIZE + 1), pUser->GetID().c_str(), NCommon::MAX_USER_ID_SIZE);
				m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)NCommon::PACKET_ID::GAME_END_RESULT, sizeof(gameResPkt), (char*)&gameResPkt);
				room->NotifyGameResult(userIndex, pUser->GetID().c_str());

			}
			else
			{
				for (auto iter : room->m_UserList)
				{
					if (iter->GetIndex() != userIndex)
					{
						strncpy_s(gameResPkt.UserID, (NCommon::MAX_USER_ID_SIZE + 1), iter->GetID().c_str(), NCommon::MAX_USER_ID_SIZE);
					}
				}
				m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)NCommon::PACKET_ID::GAME_END_RESULT, sizeof(gameResPkt), (char*)&gameResPkt);
				room->NotifyGameResult(userIndex, gameResPkt.UserID);
			}
			room->Clear();
		}	
	}
}