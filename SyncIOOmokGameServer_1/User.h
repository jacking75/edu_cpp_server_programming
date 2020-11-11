#pragma once
#include <string>

namespace ChatServerLib
{
	class User
	{
	public:
		enum class DOMAIN_STATE {
			None = 0,
			Login = 1,
			Lobby = 2,
			Room = 3,
			Ready = 4,
			Game = 5
		};

	public:
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
			m_CurDomainState = DOMAIN_STATE::None;
			m_RoomIndex = -1;
		}

		void Set(const int sessionIndex, const char* pszID)
		{
			m_CurDomainState = DOMAIN_STATE::Login;
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
			m_CurDomainState = DOMAIN_STATE::Room;
		}

		void LeaveRoom()
		{
			m_CurDomainState = DOMAIN_STATE::Login;
		}

		void SetReady()
		{
			m_CurDomainState = DOMAIN_STATE::Ready;
		}

		void SetGame()
		{
			m_CurDomainState = DOMAIN_STATE::Game;
		}

		bool IsCurDomainInLogIn() 
		{
			return m_CurDomainState == DOMAIN_STATE::Login ? true : false;
		}
		bool IsCurDomainInRoom() 
		{
			return m_CurDomainState == DOMAIN_STATE::Room ? true : false;
		}
		bool IsCurDomainInReady() 
		{
			return m_CurDomainState == DOMAIN_STATE::Ready ? true : false;
		}
		bool IsCurDomainInGame() 
		{
			return m_CurDomainState == DOMAIN_STATE::Game ? true : false;
		}


	protected:
		short m_Index = -1;

		int m_SessionIndex = -1;

		std::string m_ID;

		DOMAIN_STATE m_CurDomainState = DOMAIN_STATE::None;

		short m_RoomIndex = -1;

		
	};
}