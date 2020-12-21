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
	

	void Room::SendToAllUser(const short packetId, const short dataSize, char* pData, const int passUserindex)
	{		
		for (auto pUser : m_UserList)
		{
			if (pUser->GetSessioIndex() == passUserindex)
			{
				continue;
			}

			SendPacketFunc(pUser->GetSessioIndex(), packetId, dataSize, pData);
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

	void Room::NotifyEnterUserInfo(const int sessionIndex, const char* pszUserID)
	{
		OmokServerLib::PktRoomEnterUserInfoNtf pkt;
		pkt.UserUniqueId = sessionIndex;
		strncpy_s(pkt.UserID, (OmokServerLib::MAX_USER_ID_SIZE + 1), pszUserID, OmokServerLib::MAX_USER_ID_SIZE);
		SendToAllUser((short)OmokServerLib::PACKET_ID::ROOM_ENTER_NEW_USER_NTF, sizeof(pkt), (char*)&pkt, sessionIndex);
	}

	void Room::NotifyLeaveUserInfo(const int sessionIndex, const char* pszUserID)
	{
		OmokServerLib::PktRoomLeaveUserInfoNtf pkt;
		pkt.UserUniqueId = sessionIndex;

		SendToAllUser((short)OmokServerLib::PACKET_ID::ROOM_LEAVE_USER_NTF, sizeof(pkt), (char*)&pkt , sessionIndex);
	}

	void Room::NotifyChat(const int sessionIndex, const char* pszUserID, const char* pszMsg)
	{
		OmokServerLib::PktRoomChatNtf pkt;

		strncpy_s(pkt.UserID, (OmokServerLib::MAX_USER_ID_SIZE + 1), pszUserID, OmokServerLib::MAX_USER_ID_SIZE);
		strncpy_s(pkt.Msg, OmokServerLib::MAX_ROOM_CHAT_MSG_SIZE + 1, pszMsg, OmokServerLib::MAX_ROOM_CHAT_MSG_SIZE);

		SendToAllUser((short)OmokServerLib::PACKET_ID::ROOM_CHAT_NTF, sizeof(pkt), (char*)&pkt);
	}

	void Room::NotifyPutStoneInfo(const int sessionIndex, const char* pszUserID)
	{
		OmokServerLib::PktPutStoneRes pkt;

		strncpy_s(pkt.UserID, (OmokServerLib::MAX_USER_ID_SIZE + 1), pszUserID, OmokServerLib::MAX_USER_ID_SIZE);

		SendToAllUser((short)OmokServerLib::PACKET_ID::PUT_STONE_RES ,sizeof(pkt), (char*)&pkt, sessionIndex);
	}

	void Room::NotifyGameResult(const int sessionIndex, const char* pszUserID)
	{
		OmokServerLib::PktGameResultNtf pkt;

		strncpy_s(pkt.UserID, (OmokServerLib::MAX_USER_ID_SIZE + 1), pszUserID, OmokServerLib::MAX_USER_ID_SIZE);

		SendToAllUser((short)OmokServerLib::PACKET_ID::GAME_END_RESULT, sizeof(pkt), (char*)&pkt, sessionIndex);
	}

	void Room::NotifyGameStart(const int sessionIndex, const char* pszUserID)
	{
		OmokServerLib::PktGameReadyRes pkt;

		strncpy_s(pkt.UserID, (OmokServerLib::MAX_USER_ID_SIZE + 1), pszUserID, OmokServerLib::MAX_USER_ID_SIZE);

		SendToAllUser((short)OmokServerLib::PACKET_ID::GAME_START_RES, sizeof(pkt), (char*)&pkt, sessionIndex);
	}

	void Room::NotifyTimeOutTurnChange(const int sessionIndex, const char* pszUserID)
	{
		OmokServerLib::PktTimeOutTurnChange pkt;

		strncpy_s(pkt.UserID, (OmokServerLib::MAX_USER_ID_SIZE + 1), pszUserID, OmokServerLib::MAX_USER_ID_SIZE);

		SendToAllUser((short)OmokServerLib::PACKET_ID::TIME_OUT_TURN_CHANGE, sizeof(pkt), (char*)&pkt, -1);
	}

}