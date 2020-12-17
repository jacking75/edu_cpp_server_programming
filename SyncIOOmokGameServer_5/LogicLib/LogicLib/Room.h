#pragma once
#include <memory>
#include "User.h"
#include "Omok.h"
#include "../../ServerNetLib/ServerNetLib/TcpNetwork.h"
#include <functional>

namespace OmokServerLib
{
	class Room
	{
	public:
		enum class Room_State
		{
			None = 0,
			Game = 1
		};

		Room() = default;
		~Room() = default;

		void Init(const short index, const short maxUserCount, NServerNetLib::TcpNetwork* pNetwork);

		void Clear();

		void EndGame();

		short GetIndex() { return m_Index; }

		bool IsUsed() { return m_IsUsed; }

		short MaxUserCount() { return m_MaxUserCount; }

		ERROR_CODE EnterUser(User* pUser);

		ERROR_CODE LeaveUser(const short userIndex, const int sessionIndex, const char* pszUserID);

		void SendToAllUser(const short packetId, const short dataSize, char* pData, const int passUserindex = -1);

		void NotifyEnterUserInfo(const int sessionIndex, const char* pszUserID);

		void NotifyLeaveUserInfo(const int sessionIndex, const char* pszUserID);

		void NotifyChat(const int sessionIndex, const char* pszUserID, const char* pszMsg);

		void NotifyPutStoneInfo(const int sessionIndex, const char* pszUserID);

		void NotifyGameResult(const int sessionIndex, const char* pszUserID);

		void NotifyGameStart(const int sessionIndex, const char* pszUserID);

		void NotifyTimeOutTurnChange(const int sessionIndex, const char* pszUserID);

		void SetRoomStateGame() { m_CurDomainState = Room_State::Game; };

		bool IsCurDomainInGame() { return m_CurDomainState == Room_State::Game ? true : false; };

		void CheckTurnTimeOut();

		std::unique_ptr<Omok> m_OmokGame = std::make_unique<Omok>();

		std::vector<User*> m_UserList;

		std::function<void(const int, const short, const short, char*)> SendPacketFunc;

	private:

		NServerNetLib::TcpNetwork* m_pRefNetwork;

		Room_State m_CurDomainState = Room_State::None;

		short m_Index = -1;

		short m_MaxUserCount = -1;

		bool m_IsUsed = false;

	};
}