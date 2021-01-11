#pragma once

#include "Room.h"
#include "PacketDef.h"

namespace OmokServerLib
{
	void Room::Init(const short index, const short maxUserCount, NServerNetLib::TcpNetwork* pNetwork)
	{
		m_Index = index;
		m_MaxUserCount = maxUserCount;
		m_pRefNetwork = pNetwork;
	}

	void Room::Clear()
	{
		m_UserList.clear();
		m_CurDomainState = Room_State::None;
	}

	void Room::EndGame()
	{
		m_UserList[0]->LeaveRoom();
		m_UserList[1]->LeaveRoom();
		m_CurDomainState = Room_State::None;
	}


	ERROR_CODE Room::EnterUser(User* pUser)
	{
		if (m_UserList.size() == m_MaxUserCount) 
		{
			return ERROR_CODE::ROOM_ENTER_MEMBER_FULL;
		}

		m_UserList.push_back(pUser);

		return ERROR_CODE::NONE;
	}

	ERROR_CODE Room::LeaveUser(const short userIndex , const int sessionIndex, const char* pszUserID)
	{
		auto iter = std::find_if(std::begin(m_UserList), std::end(m_UserList),
			[userIndex](auto pUser) { return pUser->GetIndex() == userIndex; });

		if (iter == std::end(m_UserList)) 
		{
			return OmokServerLib::ERROR_CODE::ROOM_LEAVE_NOT_MEMBER;
		}

		m_UserList.erase(iter);

		if (m_UserList.empty())
		{
			Clear();
			return ERROR_CODE::NONE;
		}

		NotifyLeaveUserInfo(sessionIndex, pszUserID);
		return ERROR_CODE::NONE;
	}
	

	void Room::SendToAllUser(const short dataSize, char* pData, const int passUserindex)
	{		
		for (auto pUser : m_UserList)
		{
			if (pUser->GetSessioIndex() == passUserindex)
			{
				continue;
			}

			SendPacketFunc(pUser->GetSessioIndex(), dataSize, pData);
		}
	}

	void Room::CheckTurnTimeOut()
	{
		if (m_CurDomainState == Room_State::Game)
		{
			if (m_OmokGame->CheckTimeOut())
			{
				auto curTurnIndex = m_OmokGame->m_TurnIndex;

				auto firstUserIndex = m_UserList[0]->GetSessioIndex();
				auto firstUserID = m_UserList[0]->GetID().c_str();

				auto secondUserIndex = m_UserList[1]->GetSessioIndex();
				auto secondUserID = m_UserList[1]->GetID().c_str();

				auto nextTurnUserIndex = curTurnIndex == firstUserIndex ? secondUserIndex : firstUserIndex;
				auto nextTurnUserID = curTurnIndex == firstUserIndex ? secondUserID : firstUserID;

				m_OmokGame->m_TurnIndex = nextTurnUserIndex;
				m_OmokGame->IsBlackTurn = !m_OmokGame->IsBlackTurn;
				NotifyTimeOutTurnChange(nextTurnUserIndex, nextTurnUserID);
				m_OmokGame->SetUserTurnTime();
			}
		}
	}
	
	ERROR_CODE Room::UserSetGame(User* pUser, int sessionIndex)
	{		
		auto pOpponentUser = sessionIndex == m_UserList[0]->GetSessioIndex() ? m_UserList[1] : m_UserList[0];
		auto roomUserIndex = sessionIndex == m_UserList[0]->GetSessioIndex() ? 1 : 0;

		if (pOpponentUser->IsCurDomainInReady() == true)
		{
			pOpponentUser->SetGame();
			pUser->SetGame();
			SetRoomStateGame();

			//검은돌 랜덤 선정
			m_OmokGame->m_BlackStoneUserIndex = std::abs(1 - roomUserIndex);
			m_OmokGame->m_WhiteStoneUserIndex = roomUserIndex;
			m_OmokGame->m_TurnIndex = sessionIndex;
			m_OmokGame->init();
			NotifyGameStart(sessionIndex, pUser->GetID().c_str());

			m_OmokGame->SetUserTurnTime();

			return ERROR_CODE::NONE;

		}
		else
		{
			return ERROR_CODE::NOT_READY_EXIST;
		}

		return ERROR_CODE::UNASSIGNED_ERROR;
	}

	void Room::NotifyEnterUserInfo(const int sessionIndex, const char* pszUserID)
	{
		OmokServerLib::PktRoomEnterUserInfoNtf pkt;
		pkt.Id = (short)OmokServerLib::PACKET_ID::ROOM_ENTER_NEW_USER_NTF;
		pkt.TotalSize = sizeof(OmokServerLib::PktRoomEnterUserInfoNtf);

		pkt.UserUniqueId = sessionIndex;
		strncpy_s(pkt.UserID, (OmokServerLib::MAX_USER_ID_SIZE + 1), pszUserID, OmokServerLib::MAX_USER_ID_SIZE);
		SendToAllUser(sizeof(pkt), (char*)&pkt, sessionIndex);
	}

	void Room::NotifyLeaveUserInfo(const int sessionIndex, const char* pszUserID)
	{
		OmokServerLib::PktRoomLeaveUserInfoNtf pkt;
		pkt.Id = (short)OmokServerLib::PACKET_ID::ROOM_LEAVE_USER_NTF;
		pkt.TotalSize = sizeof(OmokServerLib::PktRoomLeaveUserInfoNtf);
		pkt.UserUniqueId = sessionIndex;

		SendToAllUser(sizeof(pkt), (char*)&pkt , sessionIndex);
	}

	void Room::NotifyChat(const int sessionIndex, const char* pszUserID, const char* pszMsg)
	{
		OmokServerLib::PktRoomChatNtf pkt;
		pkt.Id = (short)OmokServerLib::PACKET_ID::ROOM_CHAT_NTF;
		pkt.TotalSize = sizeof(OmokServerLib::PktRoomChatNtf);

		strncpy_s(pkt.UserID, (OmokServerLib::MAX_USER_ID_SIZE + 1), pszUserID, OmokServerLib::MAX_USER_ID_SIZE);
		strncpy_s(pkt.Msg, OmokServerLib::MAX_ROOM_CHAT_MSG_SIZE + 1, pszMsg, OmokServerLib::MAX_ROOM_CHAT_MSG_SIZE);

		SendToAllUser(sizeof(pkt), (char*)&pkt);
	}

	void Room::NotifyPutStoneInfo(const int sessionIndex, const char* pszUserID)
	{
		OmokServerLib::PktPutStoneRes pkt;
		pkt.Id = (short)OmokServerLib::PACKET_ID::PUT_STONE_RES;
		pkt.TotalSize = sizeof(OmokServerLib::PktPutStoneRes);

		strncpy_s(pkt.UserID, (OmokServerLib::MAX_USER_ID_SIZE + 1), pszUserID, OmokServerLib::MAX_USER_ID_SIZE);

		SendToAllUser(sizeof(pkt), (char*)&pkt, sessionIndex);
	}

	void Room::NotifyGameResult(const int sessionIndex, const char* pszUserID)
	{
		OmokServerLib::PktGameResultNtf pkt;
		pkt.Id = (short)OmokServerLib::PACKET_ID::GAME_END_RESULT;
		pkt.TotalSize = sizeof(OmokServerLib::PktGameResultNtf);

		strncpy_s(pkt.UserID, (OmokServerLib::MAX_USER_ID_SIZE + 1), pszUserID, OmokServerLib::MAX_USER_ID_SIZE);

		SendToAllUser(sizeof(pkt), (char*)&pkt, sessionIndex);
	}

	void Room::NotifyGameStart(const int sessionIndex, const char* pszUserID)
	{
		OmokServerLib::PktGameReadyRes pkt;
		pkt.Id = (short)OmokServerLib::PACKET_ID::GAME_START_RES;
		pkt.TotalSize = sizeof(OmokServerLib::PktGameReadyRes);

		strncpy_s(pkt.UserID, (OmokServerLib::MAX_USER_ID_SIZE + 1), pszUserID, OmokServerLib::MAX_USER_ID_SIZE);

		SendToAllUser(sizeof(pkt), (char*)&pkt, sessionIndex);
	}

	void Room::NotifyTimeOutTurnChange(const int sessionIndex, const char* pszUserID)
	{
		OmokServerLib::PktTimeOutTurnChange pkt;
		pkt.Id = (short)OmokServerLib::PACKET_ID::TIME_OUT_TURN_CHANGE;
		pkt.TotalSize = sizeof(OmokServerLib::PktTimeOutTurnChange);

		strncpy_s(pkt.UserID, (OmokServerLib::MAX_USER_ID_SIZE + 1), pszUserID, OmokServerLib::MAX_USER_ID_SIZE);

		SendToAllUser(sizeof(pkt), (char*)&pkt, -1);
	}

}