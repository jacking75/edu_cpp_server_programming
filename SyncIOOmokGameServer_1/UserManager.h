#pragma once
#include <unordered_map>
#include <deque>
#include <string>
#include <vector>
#include "ErrorCode.h"


namespace ChatServerLib
{

	class User;

	class UserManager
	{
	public:
		UserManager() = default;
		~UserManager() = default;

		void Init(const int maxUserCount);

		NServerNetLib::ERROR_CODE AddUser(const int sessionIndex, const char* pszID);
		NServerNetLib::ERROR_CODE RemoveUser(const int sessionIndex);

		std::pair<NServerNetLib::ERROR_CODE, User*> GetUser(const int sessionIndex);


	private:
		User* AllocUserObjPoolIndex();
		void ReleaseUserObjPoolIndex(const int index);

		User* FindUser(const int sessionIndex);
		User* FindUser(const char* pszID);

	private:
		std::vector<User> m_UserObjPool;
		std::deque<int> m_UserObjPoolIndex;

		std::unordered_map<int, User*> m_UserSessionDic;
		std::unordered_map<const char*, User*> m_UserIDDic; 
	};
}
