#pragma once
#include "ErrorCode.h"
#include <optional>
#include <mutex>
#include <queue>
#include <basetsd.h>
#include "RedisProtocol.h"
#include <functional>
#include "UserManager.h"
#include "ConnectedUserManager.h"


namespace RedisCpp
{
	class CRedisConn;
}

namespace OmokServerLib
{	
	class RedisManager
	{
		using RedisRequestInfo = CommandRequest;
		typedef ERROR_CODE(RedisManager::* PacketFunc)(RedisRequestInfo);
		PacketFunc PacketFuncArray[REDIS_TASK_ID_MAX];

	public:
		RedisManager() = default;
		~RedisManager();

		ERROR_CODE Connect(const char* ipAddress, const int portNum);

		void Disconnect();

		void Stop();

		void RedisProcessThread();

		void Process(RedisRequestInfo redisRequestInfo);

		void InsertRedisRequestQueue(RedisRequestInfo redisRequestInfo);

		std::function<void(const int, const short, const short, char*)> SendPacketFunc;

		void Init(UserManager* pUserMgr, ConnectedUserManager* pConUserMgr);

		ERROR_CODE ConfirmLogin(RedisRequestInfo redisRequestInfo);

		std::optional <CommandRequest> GetCommandResult();


	private:

		std::unique_ptr<std::thread> m_RedisThread = nullptr;

		std::mutex m_Mutex;;

		std::deque<CommandRequest> m_RedisRequestQueue;

		bool m_IsRun = false;

		UserManager* m_pRefUserMgr;

		ConnectedUserManager* m_pRefConUserMgr;

		RedisCpp::CRedisConn* con;
	};

}
