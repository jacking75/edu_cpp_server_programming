#pragma once
#include "ErrorCode.h"
#include <string>
#include <basetsd.h>

namespace OmokServerLib
{
	const int KEY_MAX_SIZE = 1024;
	const int VALUE_MAX_SIZE = 4096;
	const int REQUEST_MAX_SIZE = 4096;
	const int RESULT_MAX_SIZE = 4096;
	const int REDIS_TASK_ID_MAX = 100;

	struct CommandRequest
	{
		int SessionIndex = -1;
		int RedisTaskID = 0;
		char* CommandBody = nullptr;
	};

	enum class RedisTaskID : short
	{
		confirmLogin = 1

	};
}