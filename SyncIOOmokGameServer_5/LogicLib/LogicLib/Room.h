#pragma once
#include <memory>
#include "User.h"
#include "Omok.h"
#include "../../ServerNetLib/ServerNetLib/TcpNetwork.h"
#include <functional>

//TODO 최흥배
// 코드 전체적으로 주석이 필요한 곳은 주석을 남겨주세요. 다른 사람이 봤을 때 주석 없이 이 코드들을 이해할 수 있다고 생각하면 주석 안달아도 되지만 그런 자신이 없다면 주석 달아주세요
// 물론 함수 및 변수 이름으로 충분히 뜻을 알 수 있다면 주석은 달지 않아도 됩니다
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

		ERROR_CODE UserSetGame(User* pUser, int sessionIndex);

	private:

		NServerNetLib::TcpNetwork* m_pRefNetwork;

		Room_State m_CurDomainState = Room_State::None;

		short m_Index = -1;

		short m_MaxUserCount = -1;

		bool m_IsUsed = false;

	};
}