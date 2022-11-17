#include <string>
#include "HalfNetworkType.h"
#include "Logger.h"
#include "LogCallback.h"
#include <ace/OS_NS_string.h>
#include <ace/OS_NS_stdio.h>
#include <ace/Log_Msg.h>

namespace HalfNetwork
{
	const uint32	FormatStringLength = 32;

	Logger::Logger() : 
		_logMsg(new ACE_Log_Msg()), 
		_logCallback(NULL), 
		_disable(false)
	{
		_format = new ACE_TCHAR[FormatStringLength];
		memset(_format, 0, FormatStringLength);
	}

	Logger::~Logger()
	{
		SAFE_DELETE(_logMsg);
		delete [] _format;
		_format = NULL;
	}

	void Logger::SetConsoleLog()
	{
		RedirectTo(new ConsoleLogCallback);
	}

	void Logger::SetFileLog( const ACE_TCHAR* filename )
	{
		RedirectTo(new FileLogCallback(filename));
	}

	void Logger::Log( const ACE_TCHAR* formatStr, ... )
	{
		if (_disable)
			return;

		uint32 logBufferSize = 512;
		uint32 resultBufferLength = 0;
		ACE_TCHAR* buffer = NULL;
		do 
		{
			if (NULL != buffer)
			{
				delete[] buffer;
				buffer = NULL;
			}
			logBufferSize *= 2;
			buffer = new ACE_TCHAR[logBufferSize];
			memset(buffer, 0, logBufferSize);
			va_list argList;
			va_start(argList, formatStr);
			resultBufferLength = ACE_OS::vsnprintf(buffer, logBufferSize, formatStr, argList);
			va_end(argList);
		} while(resultBufferLength >= logBufferSize);

		ACE_TCHAR* fullSentence = new ACE_TCHAR[FormatStringLength+logBufferSize];
		ACE_OS::sprintf(fullSentence, ACE_TEXT("%s%s"), _format, buffer);

		_logMsg->log(LM_DEBUG, fullSentence);
		delete[] buffer;
		buffer = NULL;
		delete[] fullSentence;
		fullSentence = NULL;
	}

	ACE_Log_Msg_Callback* Logger::RedirectTo(ACE_Log_Msg_Callback* callback)
	{
		_logCallback = callback;
		_logMsg->clr_flags(ACE_Log_Msg::STDERR | ACE_Log_Msg::LOGGER);
		ACE_Log_Msg_Callback *previous = _logMsg->msg_callback(callback);
		if (callback == 0)
			_logMsg->clr_flags(ACE_Log_Msg::MSG_CALLBACK);
		else
			_logMsg->set_flags(ACE_Log_Msg::MSG_CALLBACK);

		return previous;
	}

	void Logger::AppendTime()
	{
		ACE_OS::strcat(_format, ACE_TEXT("%T "));
	}

	void Logger::AppendDate()
	{
		ACE_OS::strcat(_format, ACE_TEXT("%D "));
	}

	void Logger::AppendThreadId()
	{
		ACE_OS::strcat(_format, ACE_TEXT("[%t] "));
	}

	void Logger::AppendProcessId()
	{
		ACE_OS::strcat(_format, ACE_TEXT("[%P] "));
	}

	void Logger::SetDisable( bool flag )
	{
		_disable = flag;
	}
} // namespace HalfNetwork