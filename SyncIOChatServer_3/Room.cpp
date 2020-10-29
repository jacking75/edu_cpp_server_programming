#pragma once

#include <vector>
#include <string>
#include <memory>
#include <iostream>
#include "User.h"
#include "TcpNetwork.h"
#include "Room.h"
#include "PacketDef.h"

namespace ChatServerLib
{
	Room::Room() {}

	Room::~Room()
	{

	}

	void Room::Init(const short index, const short maxUserCount)
	{
		m_Index = index;
		m_MaxUserCount = maxUserCount;
	}

	void Room::SetNetwork(NServerNetLib::TcpNetwork* pNetwork)
	{
		m_pRefNetwork = pNetwork;
	}

	void Room::Clear()
	{
		m_UserList.clear();
	}

	NServerNetLib::ERROR_CODE Room::EnterUser(User* pUser)
	{

		if (m_UserList.size() == m_MaxUserCount) {
			return NServerNetLib::ERROR_CODE::ROOM_ENTER_MEMBER_FULL;
		}

		m_UserList.push_back(pUser);
		return NServerNetLib::ERROR_CODE::NONE;
	}

	NServerNetLib::ERROR_CODE Room::LeaveUser(const short userIndex)
	{
		auto iter = std::find_if(std::begin(m_UserList), std::end(m_UserList), [userIndex](auto pUser) { return pUser->GetIndex() == userIndex; });
		if (iter == std::end(m_UserList)) {
			return NServerNetLib::ERROR_CODE::ROOM_LEAVE_NOT_MEMBER;
		}

		m_UserList.erase(iter);

		if (m_UserList.empty())
		{
			Clear();
		}

		return NServerNetLib::ERROR_CODE::NONE;
	}
	
	void Room::SendToAllUser(const short packetId, const short dataSize, char* pData, const int passUserindex)
	{
		for (auto pUser : m_UserList)
		{
			if (pUser->GetIndex() == passUserindex) {
				continue;
			}

			m_pRefNetwork->SendData(pUser->GetSessioIndex(), packetId, dataSize, pData);
		}
	}

	void Room::NotifyEnterUserInfo(const int userIndex, const char* pszUserID)
	{
		NCommon::PktRoomEnterUserInfoNtf pkt;

		strncpy_s(pkt.UserID, (NCommon::MAX_USER_ID_SIZE + 1), pszUserID, NCommon::MAX_USER_ID_SIZE);


		SendToAllUser((short)NCommon::PACKET_ID::ROOM_ENTER_NEW_USER_NTF, sizeof(pkt), (char*)&pkt, userIndex);
	}

	void Room::NotifyLeaveUserInfo(const char* pszUserID)
	{

		NCommon::PktRoomLeaveUserInfoNtf pkt;
		strncpy_s(pkt.UserID, (NCommon::MAX_USER_ID_SIZE + 1), pszUserID, NCommon::MAX_USER_ID_SIZE);

		SendToAllUser((short)NCommon::PACKET_ID::ROOM_LEAVE_USER_NTF, sizeof(pkt), (char*)&pkt);
	}

	void Room::NotifyChat(const int sessionIndex, const char* pszUserID, const wchar_t* pszMsg)
	{
		NCommon::PktRoomChatNtf pkt;

		strncpy_s(pkt.UserID, (NCommon::MAX_USER_ID_SIZE + 1), pszUserID, NCommon::MAX_USER_ID_SIZE);
		wcsncpy_s(pkt.Msg, NCommon::MAX_ROOM_CHAT_MSG_SIZE + 1, pszMsg, NCommon::MAX_ROOM_CHAT_MSG_SIZE);

		SendToAllUser((short)NCommon::PACKET_ID::ROOM_CHAT_NTF, sizeof(pkt), (char*)&pkt, sessionIndex);
	}

}