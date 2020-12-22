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

		//TODO 최흥배
		// VC++의 Concurrent_Queue 라는 스레드세이프한 큐 컨테이너를 사용하도록 하죠
		// 여기 이외에도 lock이 필요한 컨테이너는 스레드 세이프한 컨테이너로 바꾸기 바랍니다.
		// https://vsts2010.tistory.com/171?category=79917
		std::deque<CommandRequest> m_RedisRequestQueue;

		bool m_IsRun = false;

		UserManager* m_pRefUserMgr;

		ConnectedUserManager* m_pRefConUserMgr;

		RedisCpp::CRedisConn* con;
	};

}
