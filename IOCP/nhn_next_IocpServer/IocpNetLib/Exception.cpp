#include "Exception.h"
#include <fstream>
#include <DbgHelp.h>
#include <TlHelp32.h>
#include <strsafe.h>
#include "StackWalker.h"

#define MAX_BUFF_SIZE 1024

void MakeDump(EXCEPTION_POINTERS* e)
{
	wchar_t tszFileName[MAX_BUFF_SIZE] = { 0 };
	SYSTEMTIME stTime = { 0 };
	GetSystemTime(&stTime);
	StringCbPrintf(tszFileName,
		_countof(tszFileName),
		L"%s_%4d%02d%02d_%02d%02d%02d.dmp",
		L"EduServerDump",
		stTime.wYear,
		stTime.wMonth,
		stTime.wDay,
		stTime.wHour,
		stTime.wMinute,
		stTime.wSecond);

	HANDLE hFile = CreateFile(tszFileName, GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile == INVALID_HANDLE_VALUE)
		return;

	MINIDUMP_EXCEPTION_INFORMATION exceptionInfo;
	exceptionInfo.ThreadId = GetCurrentThreadId();
	exceptionInfo.ExceptionPointers = e;
	exceptionInfo.ClientPointers = FALSE;

	MiniDumpWriteDump(
		GetCurrentProcess(),
		GetCurrentProcessId(),
		hFile,
		MINIDUMP_TYPE(MiniDumpWithIndirectlyReferencedMemory | MiniDumpScanMemory | MiniDumpWithFullMemory),
		e ? &exceptionInfo : NULL,
		NULL,
		NULL);

	if (hFile)
	{
		CloseHandle(hFile);
		hFile = NULL;
	}
	
}


LONG WINAPI ExceptionFilter(EXCEPTION_POINTERS* exceptionInfo)
{
	if ( IsDebuggerPresent() )
		return EXCEPTION_CONTINUE_SEARCH ;

	
	THREADENTRY32 te32;
	DWORD myThreadId = GetCurrentThreadId();
	DWORD myProcessId = GetCurrentProcessId();

	HANDLE hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (hThreadSnap != INVALID_HANDLE_VALUE)
	{
		te32.dwSize = sizeof(THREADENTRY32);

		if (Thread32First(hThreadSnap, &te32))
		{
			do
			{
				/// 내 프로세스 내의 스레드중 나 자신 스레드만 빼고 멈추게..
				if (te32.th32OwnerProcessID == myProcessId && te32.th32ThreadID != myThreadId)
				{
					HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, te32.th32ThreadID);
					if (hThread)
					{
						SuspendThread(hThread);
					}
				}

			} while (Thread32Next(hThreadSnap, &te32));

		}

		CloseHandle(hThreadSnap);
	}
		
	
	std::ofstream historyOut("EduServer_exception.txt", std::ofstream::out);
	
	/// 콜히스토리 남기고
	/*historyOut << "========== WorkerThread Call History ==========" << std::endl << std::endl;
	for (int i = 0; i < MAX_WORKER_THREAD; ++i)
	{
		if (GThreadCallHistory[i])
		{
			GThreadCallHistory[i]->DumpOut(historyOut);
		}
	}*/

	/// 콜성능 남기고
	/*historyOut << "========== WorkerThread Call Performance ==========" << std::endl << std::endl;
	for (int i = 0; i < MAX_WORKER_THREAD; ++i)
	{
		if (GThreadCallElapsedRecord[i])
		{
			GThreadCallElapsedRecord[i]->DumpOut(historyOut);
		}
	}*/

	/// 콜스택도 남기고
	historyOut << "========== Exception Call Stack ==========" << std::endl << std::endl;
	StackWalker stackWalker;
	stackWalker.SetOutputStream(&historyOut);
	stackWalker.ShowCallstack();
	
	/// 이벤트 로그 남기고
	//LoggerUtil::EventLogDumpOut(historyOut);

	historyOut.flush();
	historyOut.close();

	/// 마지막으로 dump file 남기자.
	MakeDump(exceptionInfo);


	ExitProcess(1);
	/// 여기서 쫑

	return EXCEPTION_EXECUTE_HANDLER;
	
}