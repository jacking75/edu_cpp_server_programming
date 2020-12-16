#pragma once
#include <string>

namespace OmokServerLib
{
	class User
	{
	public:
		enum class Domain_State {
			None = 0,
			Login = 1,
			Lobby = 2,
			Room = 3,
			Ready = 4,
			Game = 5
		};

	public:
		//TODO 최흥배
		// default 사용이 좋지 않을까요?
		User() {}
		virtual ~User() {}

		void Init(const short index)
		{
			m_Index = index;
		}

		void Clear()
		{
			m_SessionIndex = 0;
			m_ID = "";
			m_CurDomainState = Domain_State::None;
			m_RoomIndex = -1;
		}

		void Set(const int sessionIndex, const char* pszID)
		{
			m_CurDomainState = Domain_State::Login;
			m_SessionIndex = sessionIndex;
			m_ID = pszID;
		}

		short GetIndex() { return m_Index; }

		int GetSessioIndex() { return m_SessionIndex; }

		std::string& GetID() { return m_ID; }

		short GetRoomIndex() { return m_RoomIndex; }

		void EnterRoom( const short roomIndex)
		{
			m_RoomIndex = roomIndex;
			m_CurDomainState = Domain_State::Room;
		}

		void LeaveRoom()
		{
			m_CurDomainState = Domain_State::Login;
		}

		void SetReady()
		{
			m_CurDomainState = Domain_State::Ready;
		}

		void SetGame()
		{
			m_CurDomainState = Domain_State::Game;
		}

		void SetLogin ()
		{
			m_CurDomainState = Domain_State::Login;
		}

		bool IsCurDomainInLogIn() 
		{
			return m_CurDomainState == Domain_State::Login ? true : false;
		}
		bool IsCurDomainInRoom() 
		{
			return m_CurDomainState == Domain_State::Room ? true : false;
		}
		bool IsCurDomainInReady() 
		{
			return m_CurDomainState == Domain_State::Ready ? true : false;
		}
		bool IsCurDomainInGame() 
		{
			return m_CurDomainState == Domain_State::Game ? true : false;
		}


		//TODO 최흥배
		// protected 사용하는 이유가 있나요?
	protected:

		short m_Index = -1;

		int m_SessionIndex = -1;

		std::string m_ID;

		Domain_State m_CurDomainState = Domain_State::None;

		short m_RoomIndex = -1;
		
	};
}