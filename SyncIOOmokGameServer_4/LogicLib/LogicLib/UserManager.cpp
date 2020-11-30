#include "UserManager.h"
#include "User.h"

namespace OmokServerLib
{

	void UserManager::Init(const int maxUserCount)
	{
		for (int i = 0; i < maxUserCount; ++i)
		{
			User user;
			user.Init((short)i);

			m_UserObjPool.push_back(user);
			m_UserObjPoolIndex.push_back(i);
		}
	}

	std::optional <User*> UserManager::AllocUserObjPoolIndex()
	{
		if (m_UserObjPoolIndex.empty())
		{
			return std::nullopt;
		}

		int index = m_UserObjPoolIndex.front();
		m_UserObjPoolIndex.pop_front();

		return &m_UserObjPool[index];
	}

	void UserManager::ReleaseUserObjPoolIndex(const int index)
	{
		m_UserObjPoolIndex.push_back(index);
		m_UserObjPool[index].Clear();
	}

	ERROR_CODE UserManager::AddUser(const int sessionIndex, const char* pszID)
	{
		if (FindUser(pszID).has_value() == true)
		{
			return ERROR_CODE::USER_MGR_ID_DUPLICATION;
		}

		auto pUser = AllocUserObjPoolIndex();

		if (pUser.has_value() == false)
		{
			return ERROR_CODE::USER_MGR_MAX_USER_COUNT;
		}

		pUser.value()->Set(sessionIndex, pszID);
		
		m_UserSessionDic.insert({ sessionIndex, pUser.value() });
		m_UserIDDic.insert({ pszID, pUser.value() });

		return ERROR_CODE::NONE;
	}

	ERROR_CODE UserManager::RemoveUser(const int sessionIndex)
	{
		auto pUser = FindUser(sessionIndex);

		if (pUser.has_value() == false) 
		{
			return ERROR_CODE::USER_MGR_REMOVE_INVALID_SESSION;
		}
		
		auto index = pUser.value()->GetIndex();
		auto pszID = pUser.value()->GetID();

		m_UserSessionDic.erase(sessionIndex);
		m_UserIDDic.erase(pszID.c_str());
		ReleaseUserObjPoolIndex(index);

		return ERROR_CODE::NONE;
	}
	
	std::pair<ERROR_CODE, User*> UserManager::GetUser(const int sessionIndex)
	{
		auto pUser = FindUser(sessionIndex);

		if (pUser.has_value() == false)
		{
			return { ERROR_CODE::USER_MGR_INVALID_SESSION_INDEX, nullptr };
		}

		return { ERROR_CODE::NONE, pUser.value() };
	}

	std::optional <User*> UserManager::FindUser(const int sessionIndex)
	{
		auto findIter = m_UserSessionDic.find(sessionIndex);

		if (findIter == m_UserSessionDic.end())
		{
			return std::nullopt;
		}

		return (User*)findIter->second;
	}
	
	std::optional <User*> UserManager::FindUser(const char* pszID)
	{
		auto findIter = m_UserIDDic.find(pszID);

		if (findIter == m_UserIDDic.end()) 
		{
			return std::nullopt;
		}

		return (User*)findIter->second;
	}

	ERROR_CODE UserManager::CheckReady(User* pUser)
	{
		if (pUser->IsCurDomainInGame() == true)
		{
			return ERROR_CODE::ALREADY_GAME_STATE;
		}

		if (pUser->IsCurDomainInLogIn() == true)
		{
			return ERROR_CODE::ROOM_ENTER_NOT_CREATED;
		}

		if (pUser->IsCurDomainInReady() == true)
		{
			return ERROR_CODE::ALREADY_READY_STATE;
		}

		return ERROR_CODE::NONE;
	}
}