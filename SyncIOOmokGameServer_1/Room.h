#pragma once

#include <vector>
#include <string>
#include <memory>
#include "User.h"
#include "Omok.h"
#include "TcpNetwork.h"


namespace ChatServerLib
{
	class Room
	{
	public:
		enum class ROOM_STATE {
			NONE = 0,
			GAME = 1
		};

		Room();
		~Room();

		void Init(const short index, const short maxUserCount, NServerNetLib::TcpNetwork* pNetwork);

		void SetNetwork(NServerNetLib::TcpNetwork* pNetwork);

		void Clear();

		short GetIndex() { return m_Index; }

		bool IsUsed() { return m_IsUsed; }

		short MaxUserCount() { return m_MaxUserCount; }

		NServerNetLib::ERROR_CODE EnterUser(User* pUser);

		NServerNetLib::ERROR_CODE LeaveUser(const short userIndex);

		void SendToAllUser(const short packetId, const short dataSize, char* pData, const int passUserindex = -1);

		void NotifyEnterUserInfo(const int userIndex, const char* pszUserID);

		void NotifyLeaveUserInfo(const int userIndex, const char* pszUserID);

		void NotifyChat(const int sessionIndex, const char* pszUserID, const char* pszMsg);
		void NotifyPutStoneInfo(const int userIndex, const char* pszUserID);
		void NotifyGameResult(const int userIndex, const char* pszUserID);
		void NotifyGameStart(const int userIndex, const char* pszUserID);
		bool IsCurDomainInGame() { return m_CurDomainState == ROOM_STATE::GAME ? true : false; };


		Omok* OmokGame  = new Omok();
		std::vector<User*> m_UserList;

		int m_BlackStoneUserIndex = -1;
		int m_TurnIndex = -1;
	private:

		NServerNetLib::TcpNetwork* m_pRefNetwork;

		ROOM_STATE m_CurDomainState = ROOM_STATE::NONE;

		short m_Index = -1;
		short m_MaxUserCount = -1;
	
		bool m_IsUsed = false;
		std::wstring m_Title;
	
	};
}