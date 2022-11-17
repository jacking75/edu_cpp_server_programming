#ifndef __logback_h__
#define __logback_h__

#pragma once

#include <ace/Log_Msg_Callback.h>

namespace HalfNetwork
{
	//////////////////////////////////////////////////////////////////////////
	class CustomLogCallback : public ACE_Log_Msg_Callback
	{
	public:
		CustomLogCallback() {}

	public:
		void log(ACE_Log_Record& log_record);
		virtual void Log(const ACE_TCHAR* msg) = 0;
	};

	//////////////////////////////////////////////////////////////////////////
	class ConsoleLogCallback : public CustomLogCallback
	{
	public:
		ConsoleLogCallback();
		virtual ~ConsoleLogCallback();

	public:
		void Log(const ACE_TCHAR* msg);
	};

	//////////////////////////////////////////////////////////////////////////
	class FileLogCallback : public CustomLogCallback
	{
	public:
		FileLogCallback(const ACE_TCHAR* filename);
		~FileLogCallback();

	public:
		void Log(const ACE_TCHAR* msg);

	protected:
		void _CheckUnicodeBOM();

	private:
		ACE_TCHAR*	_filename;
		bool				_unicodeBomCheck;
		bool				_append;
	};


	//////////////////////////////////////////////////////////////////////////
	class DebugLogCallback : public CustomLogCallback
	{
	public:
		void Log(const ACE_TCHAR* msg);
	};


} // namespace HalfNetwork


#endif // __logback_h__