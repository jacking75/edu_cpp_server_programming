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

	//TODO 최흥배
	// 네이밍 일관선이 없습니다. 첫 시작을 대문자로 시작하지 않네요
	// -> 해결
	// -> 멤버 변수가 대문자로 시작하지 않고 소문자로 시작하면 안 되는 것 아닌가요???
	struct CommandRequest
	{
		int sessionIndex = -1;
		int redisTaskID = 0;
		char* commandBody = nullptr;
	};

	enum class RedisTaskID : short
	{
		confirmLogin = 1

	};
}