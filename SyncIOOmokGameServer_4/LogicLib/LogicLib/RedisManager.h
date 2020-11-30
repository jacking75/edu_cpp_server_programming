#pragma once
#include "ErrorCode.h"
#include <basetsd.h>
#include <queue>
#include <mutex>
#include "RedisProtocol.h"

namespace OmokServerLib
{
	class RedisManager
	{
	public:
		ERROR_CODE Connect(const char* ipAddress, const int portNum);

		void Disconnect();

		void ExecuteCommandAsync(const CommandRequest& request);

		CommandResponse GetCommandResult();

	private:

		void ExecuteCommandProcess();

		std::string CommandRequestToString(const CommandRequest& request);

	private:

		redisContext* m_Connection = nullptr;

		std::unique_ptr<std::thread> m_RedisThread = nullptr;
		std::mutex m_Mutex;;

		std::queue<CommandRequest> m_RequestQueue;
		std::queue<CommandResponse> m_ResponseQueue;

		UINT32 m_SendCheckTick = 0;
		UINT32 m_ReceiveCheckTick = 0;
		UINT32 m_ReceiveCheckTimeOut = 0;

	};

}

