#pragma once

#include <toml++/toml.h>
#include "ErrorCode.h"

namespace OmokServerLib
{
	class Config
	{
	public:
		ERROR_CODE Load();

		std::optional<std::string> redisAddress;
		int port;
		int maxClientCount;
		int extraClientCount;
		int backLogCount;
		int maxClientRecvBufferSize;
		int maxClientSendBufferSize;
		int maxRoomUserCount;
		int maxRoomCountByLobby;
		int redisPortNum;
	};
}

