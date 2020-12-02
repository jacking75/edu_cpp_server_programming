#pragma once
#include "ErrorCode.h"
#include <basetsd.h>
#include <queue>
#include <mutex>
#include "RedisProtocol.h"
#include <hiredis.h>
#include <functional>

namespace OmokServerLib
{
	class RedisHandler
	{
		using PacketInfo = CommandRequest;
	public:

		ERROR_CODE ConfirmLogin(PacketInfo packinfo);

		ERROR_CODE Connect(const char* ipAddress, const int portNum);

		void Disconnect();

		void ExecuteCommandAsync(const CommandRequest& request);

		CommandResponse GetCommandResult();

		std::function<void(const int, const short, const short, char*)> SendPacketFunc;

	private:

		void ExecuteCommandProcess();

		std::string CommandRequestToString(const CommandRequest& request);

	private:


	};

}

