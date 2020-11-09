#pragma once

#include "Room.h"
#include "PacketDef.h"

namespace ChatServerLib
{
	Room::Room() {}

	Room::~Room()
	{
		delete OmokGame;
	}

	void Room::Init(const short index, const short maxUserCount, NServerNetLib::TcpNetwork* pNetwork)
	{
		m_Index = index;
		m_MaxUserCount = maxUserCount;
		m_pRefNetwork = pNetwork;
	}

	void Room::SetNetwork(NServerNetLib::TcpNetwork* pNetwork)
	{
		m_pRefNetwork = pNetwork;
	}

	void Room::Clear()
	{
		m_UserList.clear();
		m_TurnIndex = -1;
		m_BlackStoneUserIndex = -1;
		OmokGame->initType();
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

	ERROR_CODE Room::LeaveUser(const short userIndex)
	{
		auto iter = std::find_if(std::begin(m_UserList), std::end(m_UserList), [userIndex](auto pUser) { return pUser->GetIndex() == userIndex; });
		if (iter == std::end(m_UserList)) 
		{
			return ChatServerLib::ERROR_CODE::ROOM_LEAVE_NOT_MEMBER;
		}

		m_UserList.erase(iter);

		if (m_UserList.empty())
		{
			Clear();
		}
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

			m_pRefNetwork->SendData(pUser->GetSessioIndex(), packetId, dataSize, pData);
		}
	}

	void Room::SendChatToAllUser(const short packetId, const short dataSize, char* pData, const int passUserindex)
	{
		for (auto pUser : m_UserList)
		{
			m_pRefNetwork->SendData(pUser->GetSessioIndex(), packetId, dataSize, pData);
		}
	}

	void Room::NotifyEnterUserInfo(const int userIndex, const char* pszUserID)
	{
		NCommon::PktRoomEnterUserInfoNtf pkt;
		pkt.UserUniqueId = userIndex;
		strncpy_s(pkt.UserID, (NCommon::MAX_USER_ID_SIZE + 1), pszUserID, NCommon::MAX_USER_ID_SIZE);
		SendToAllUser((short)NCommon::PACKET_ID::ROOM_ENTER_NEW_USER_NTF, sizeof(pkt), (char*)&pkt, userIndex);
	}

	void Room::NotifyLeaveUserInfo(const int userIndex, const char* pszUserID)
	{
		NCommon::PktRoomLeaveUserInfoNtf pkt;
		pkt.UserUniqueId = userIndex;

		SendToAllUser((short)NCommon::PACKET_ID::ROOM_LEAVE_USER_NTF, sizeof(pkt), (char*)&pkt , userIndex);
	}

	void Room::NotifyChat(const int sessionIndex, const char* pszUserID, const char* pszMsg)
	{
		NCommon::PktRoomChatNtf pkt;

		strncpy_s(pkt.UserID, (NCommon::MAX_USER_ID_SIZE + 1), pszUserID, NCommon::MAX_USER_ID_SIZE);
		strncpy_s(pkt.Msg, NCommon::MAX_ROOM_CHAT_MSG_SIZE + 1, pszMsg, NCommon::MAX_ROOM_CHAT_MSG_SIZE);

		SendChatToAllUser((short)NCommon::PACKET_ID::ROOM_CHAT_NTF, sizeof(pkt), (char*)&pkt, sessionIndex);
	}

	void Room::NotifyPutStoneInfo(const int userIndex, const char* pszUserID)
	{
		NCommon::PktPutStoneRes pkt;

		strncpy_s(pkt.UserID, (NCommon::MAX_USER_ID_SIZE + 1), pszUserID, NCommon::MAX_USER_ID_SIZE);

		SendToAllUser((short)NCommon::PACKET_ID::PUT_STONE_RES ,sizeof(pkt), (char*)&pkt, userIndex);
	}

	void Room::NotifyGameResult(const int userIndex, const char* pszUserID)
	{
		NCommon::PktGameResultNtf pkt;

		strncpy_s(pkt.UserID, (NCommon::MAX_USER_ID_SIZE + 1), pszUserID, NCommon::MAX_USER_ID_SIZE);

		SendToAllUser((short)NCommon::PACKET_ID::GAME_END_RESULT, sizeof(pkt), (char*)&pkt, userIndex);
	}

	void Room::NotifyGameStart(const int userIndex, const char* pszUserID)
	{
		NCommon::PktGameReadyRes pkt;

		strncpy_s(pkt.UserID, (NCommon::MAX_USER_ID_SIZE + 1), pszUserID, NCommon::MAX_USER_ID_SIZE);

		SendToAllUser((short)NCommon::PACKET_ID::GAME_START_RES, sizeof(pkt), (char*)&pkt, userIndex);
	}

}