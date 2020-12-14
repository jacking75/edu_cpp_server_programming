#pragma once

#include "ErrorCode.h"
#include <optional>


#include <mutex>
#include <queue>
#include <basetsd.h>
#include "RedisProtocol.h"
#include <functional>
#include "UserManager.h"


struct redisContext;

namespace OmokServerLib
{
	class RedisManager
	{
		using RedisRequestInfo = CommandRequest;
		typedef ERROR_CODE(RedisManager::* PacketFunc)(RedisRequestInfo);
		PacketFunc PacketFuncArray[REDIS_TASK_ID_MAX];

	public:
		RedisManager();
		~RedisManager();

		ERROR_CODE Connect(const char* ipAddress, const int portNum);

		void Disconnect();

		void Stop();

		void RedisProcessThread();

		void Init();

		void Process(RedisRequestInfo redisRequestInfo);

		void InsertRedisRequestQueue(RedisRequestInfo redisRequestInfo);

		std::function<void(const int, const short, const short, char*)> SendPacketFunc;

		void Init(UserManager* pUserMgr);

	public:

		ERROR_CODE ConfirmLogin(RedisRequestInfo redisRequestInfo);

		std::optional <CommandRequest> GetCommandResult();


	private:

		redisContext* m_Connection = nullptr;

		std::unique_ptr<std::thread> m_RedisThread = nullptr;
		std::mutex m_Mutex;;

		std::deque<CommandRequest> m_RedisRequestQueue;

		bool m_IsRun = false;

		UserManager* m_pRefUserMgr;
	};

}
