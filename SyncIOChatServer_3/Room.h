#pragma once

#include <vector>
#include <string>
#include <memory>

#include "User.h"
#include "TcpNetwork.h"


namespace ChatServerLib
{
	class Room
	{
	public:
		Room();
		virtual ~Room();

		void Init(const short index, const short maxUserCount);

		void SetNetwork(NServerNetLib::TcpNetwork* pNetwork);

		void Clear();

		short GetIndex() { return m_Index; }

		bool IsUsed() { return m_IsUsed; }

		short MaxUserCount() { return m_MaxUserCount; }

		short GetUserCount() { return (short)m_UserList.size(); }

		NServerNetLib::ERROR_CODE EnterUser(User* pUser);

		NServerNetLib::ERROR_CODE LeaveUser(const short userIndex);

		void SendToAllUser(const short packetId, const short dataSize, char* pData, const int passUserindex = -1);

		void NotifyEnterUserInfo(const int userIndex, const char* pszUserID);

		void NotifyLeaveUserInfo(const char* pszUserID);

		void NotifyChat(const int sessionIndex, const char* pszUserID, const wchar_t* pszMsg);

	private:

		NServerNetLib::TcpNetwork* m_pRefNetwork;

		short m_Index = -1;
		short m_MaxUserCount;

		bool m_IsUsed = false;
		std::wstring m_Title;
		std::vector<User*> m_UserList;

	};
}