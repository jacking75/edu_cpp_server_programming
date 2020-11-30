#pragma once
#include "ErrorCode.h"
#include <string>
#include <basetsd.h>

namespace OmokServerLib
{
	enum class CommandType : UINT16
	{
		NONE = 0,
		SET = 1,
		GET = 2,
	};

	const int KEY_MAX_SIZE = 1024;
	const int VALUE_MAX_SIZE = 4096;
	const int REQUEST_MAX_SIZE = 4096;
	const int RESULT_MAX_SIZE = 4096;

	struct CommandRequest
	{
		CommandType m_CommandType = CommandType::NONE;
		char* m_CommandBody = nullptr;
		int m_CommandBodySize = 0;
	};

	struct CommandResponse
	{
		CommandType m_CommandType = CommandType::NONE;
		ERROR_CODE m_ErrorCode = ERROR_CODE::NONE;
		std::string m_Result;
	};

	struct Set
	{
		char m_Key[KEY_MAX_SIZE] = { 0, };
		char m_Value[VALUE_MAX_SIZE] = { 0, };
		UINT64 m_ExpireTime = 0;
	};

	struct Get
	{
		char m_Key[KEY_MAX_SIZE] = { 0, };
	};

}