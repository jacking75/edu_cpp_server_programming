#pragma once

#ifdef WIN32

#include <Windows.h>
#include <dbghelp.h>


namespace hbServerEngine
{
#define DUMP_FILE ".\\WindowsP.dmp"

	void CreateDumpFile(LPCSTR lpstrDumpFilePathName, EXCEPTION_POINTERS* pException) noexcept 
	{
		HANDLE hDumpFile = CreateFile(lpstrDumpFilePathName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hDumpFile == INVALID_HANDLE_VALUE)
		{
			return;
		}

		MINIDUMP_EXCEPTION_INFORMATION dumpInfo;
		dumpInfo.ExceptionPointers = pException;
		dumpInfo.ThreadId = GetCurrentThreadId();
		dumpInfo.ClientPointers = true;

		MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile, MiniDumpNormal, &dumpInfo, NULL, NULL);

		CloseHandle(hDumpFile);

		return;
	}

	LONG ApplicationCrashHandler(EXCEPTION_POINTERS* pException) noexcept 
	{
		CreateDumpFile(DUMP_FILE, pException);

		FatalAppExit(-1, "*** Unhandled Exception! ***");

		return EXCEPTION_EXECUTE_HANDLER;
	}


}

#endif
