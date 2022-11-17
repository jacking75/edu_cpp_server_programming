#ifndef __logger_h__
#define __logger_h__

#pragma once

class ACE_Log_Msg_Callback;
class ACE_Log_Msg;

namespace HalfNetwork
{

	class Logger
	{
	public:
		Logger();
		virtual ~Logger();

	public:
		void	SetConsoleLog();
		void	SetFileLog(const ACE_TCHAR* filename);
		void	SetDisable(bool flag);

	public:
		ACE_Log_Msg_Callback* RedirectTo(ACE_Log_Msg_Callback* callback);
		void	Log(const ACE_TCHAR* formatStr, ...);

	public:
		void	AppendTime();
		void	AppendDate();
		void	AppendThreadId();
		void	AppendProcessId();

	private:
		ACE_Log_Msg*						_logMsg;
		ACE_Log_Msg_Callback*		_logCallback;
		ACE_TCHAR*							_format;
		bool										_disable;
	};

} // namespace HalfNetwork

#ifdef  NO_HALF_LOG
#define	HALF_LOG(LOGGER, MSG,...) do {} while (0)
#else
#define	HALF_LOG(LOGGER, MSG,...) \
	LOGGER->Log(MSG, __VA_ARGS__) 
#endif

#endif // __logger_h__