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

	struct CommandRequest
	{
		char* m_CommandBody = nullptr;
		int m_CommandBodySize = 0;
	};

	struct CommandResponse
	{
		ERROR_CODE m_ErrorCode = ERROR_CODE::NONE;
		std::string m_Result;
	};

	struct Get
	{
		char m_Key[KEY_MAX_SIZE] = { 0, };
	};

}