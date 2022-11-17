#include <string>
#include "HalfNetworkType.h"
#include "LogCallback.h"
#include <ace/Log_Record.h>
#include <ace/OS_NS_stdio.h>
#include <ace/OS_NS_string.h>

namespace HalfNetwork
{
	/////////////////////////
	// CustomLogCallback
	/////////////////////////
	void CustomLogCallback::log(ACE_Log_Record& log_record)
	{
		Log((ACE_TCHAR*)log_record.msg_data());
	}

	/////////////////////////
	// ConsoleLogCallback
	/////////////////////////
	ConsoleLogCallback::ConsoleLogCallback() : CustomLogCallback()
	{
	}

	ConsoleLogCallback::~ConsoleLogCallback()
	{
	}


	void ConsoleLogCallback::Log(const ACE_TCHAR* msg)
	{
#ifdef ACE_USES_WCHAR
		wprintf( ACE_TEXT("%s\n"), msg);
#else
		printf( ACE_TEXT("%s\n"), msg);
#endif
	}

	/////////////////////////
	// FileLogCallback
	/////////////////////////
	FileLogCallback::FileLogCallback(const ACE_TCHAR* filename) : 
		_unicodeBomCheck(false), 
		_append(true)
	{
		size_t strLength = ACE_OS::strlen(filename)+2;
		_filename = new ACE_TCHAR[strLength];
		ACE_OS::sprintf(_filename, ACE_TEXT("%s"), filename);

		if (false == _append)
			remove(ACE_TEXT_ALWAYS_CHAR(filename));
	}

	FileLogCallback::~FileLogCallback()
	{
		delete [] _filename;
		_filename = NULL;
	}

	void FileLogCallback::_CheckUnicodeBOM()
	{
#ifndef ACE_USES_WCHAR
		return;
#endif
		if (_unicodeBomCheck)
			return;

		FILE* fp = ACE_OS::fopen(_filename, ACE_TEXT("r"));
		if (NULL == fp)
		{
			ACE_TCHAR bom = (ACE_TCHAR)0xFEFF;
			fp = ACE_OS::fopen(_filename, ACE_TEXT("wb"));
			fwrite(&bom, sizeof(ACE_TCHAR), 1, fp);
		}
		fclose(fp);
		fp = NULL;
		_unicodeBomCheck = true;
	}

	void FileLogCallback::Log(const ACE_TCHAR* msg)
	{
		_CheckUnicodeBOM();

		FILE* fp = ACE_OS::fopen(_filename, ACE_TEXT("ab"));
		if (NULL == fp)
			return;

		fwrite(msg, sizeof(ACE_TCHAR), ACE_OS::strlen(msg), fp);
		const ACE_TCHAR* lineFeed = ACE_TEXT("\r\n");
		fwrite(lineFeed, sizeof(ACE_TCHAR), ACE_OS::strlen(lineFeed), fp);
		fclose(fp);
		fp = NULL;
	}

	/////////////////////////
	// DebugLogCallback
	/////////////////////////
	void DebugLogCallback::Log(const ACE_TCHAR* msg)
	{
#ifdef WIN32
		//OutputDebugString(msg);
#endif
	}


} // namespace HalfNetwork