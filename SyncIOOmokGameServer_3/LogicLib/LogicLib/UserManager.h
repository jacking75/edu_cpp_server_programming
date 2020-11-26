#pragma once
#include <unordered_map>
#include <deque>
#include <string>
#include <vector>
#include "ErrorCode.h"
#include <optional>

namespace ChatServerLib
{
	class User;

	class UserManager
	{
	public:

		UserManager() = default;
		~UserManager() = default;

		void Init(const int maxUserCount);

		ERROR_CODE AddUser(const int sessionIndex, const char* pszID);

		ERROR_CODE RemoveUser(const int sessionIndex);

		std::pair<ERROR_CODE, User*> GetUser(const int sessionIndex);

		ERROR_CODE CheckReady(User* pUser);

	private:

		std::optional <User*> AllocUserObjPoolIndex();

		void ReleaseUserObjPoolIndex(const int index);

	    std::optional <User*> FindUser(const char* pszID);

		std::optional <User*> FindUser(const int sessionIndex);

	private:

		std::vector<User> m_UserObjPool;

		std::deque<int> m_UserObjPoolIndex;

		std::unordered_map<int, User*> m_UserSessionDic;

		std::unordered_map<const char*, User*> m_UserIDDic; 
	};
}
