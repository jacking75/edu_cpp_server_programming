#pragma once

#include <Windows.h>


class AsyncIOException
{
	DWORD		errorcode;
	char		msg[128];
public:

	AsyncIOException(DWORD error, char* amsg) : errorcode(error)
	{
		ZeroMemory(msg, sizeof(msg));
		if (amsg)
		{
			size_t len = strlen(amsg);
			if (sizeof(msg) > len)
			{
				strncpy_s(msg, amsg, len);
			}
		}

		// 발생자동로깅
		//	발생시 특정파일에 로그를 남기고,
		//	콜스택을 일정수준까지 출력해 준다.
	}

	virtual ~AsyncIOException()
	{
	}

	const char* toMsg()
	{
		return msg;
	}

	DWORD toError()
	{
		return errorcode;
	}
};
