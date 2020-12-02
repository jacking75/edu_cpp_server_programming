#pragma once
#include "ErrorCode.h"
#include <hiredis.h>
#include <mutex>
#include <queue>
#include <basetsd.h>
#include "RedisProtocol.h"
#include "RedisHandler.h"

namespace OmokServerLib
{
	class RedisManager
	{
		using PacketInfo = CommandRequest;
		typedef ERROR_CODE(RedisManager::* PacketFunc)(PacketInfo);
		PacketFunc PacketFuncArray[REDIS_TASK_ID_MAX];

	public:

		ERROR_CODE Connect(const char* ipAddress, const int portNum);

		void Disconnect();

		void Stop();

		void RedisProcessThread();

		void Init();

		void Process(PacketInfo packetInfo);

	public:

		ERROR_CODE ConfirmLogin(PacketInfo packinfo);

		bool GetCommandResult();

	private:
		redisContext* m_Connection = nullptr;

		std::unique_ptr<std::thread> m_RedisThread = nullptr;
		std::mutex m_Mutex;;

		std::queue<CommandRequest> m_RequestQueue;
		std::queue<CommandResponse> m_ResponseQueue;

		UINT32 m_SendCheckTick = 0;
		UINT32 m_ReceiveCheckTick = 0;
		UINT32 m_ReceiveCheckTimeOut = 0;

		bool m_IsRun = false;
	};

}
