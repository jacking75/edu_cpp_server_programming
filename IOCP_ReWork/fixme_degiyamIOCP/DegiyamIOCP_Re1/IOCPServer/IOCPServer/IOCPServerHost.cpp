// IOCPServer.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "ServiceManager.h"

const BOOL PreProcessParameter(const int argc, _TCHAR* argv[]);
BOOL WINAPI ConsoleCtrlHandler(const DWORD dwCtrlType);	// 디버그 콘솔 모드시

ServiceManager theServer;

int _tmain(int argc, _TCHAR* argv[])
{
	if ( PreProcessParameter(argc, argv) != TRUE )
		return FALSE;

	return TRUE;
}

const BOOL PreProcessParameter(const int argc, _TCHAR* argv[])
{
	BOOL	bIsSuccess = TRUE;
	TCHAR	szServiceDir[MAX_STRING] = _T("");

	if ( GetModuleFileName(NULL, szServiceDir, MAX_STRING) == 0 )
	{
		_tprintf(_T("PreProcessParameter::GetModuleFileName Error\n"));
		bIsSuccess = FALSE;
	}

	_tprintf(_T("Start Console Mode\n"));

	if ( SetConsoleCtrlHandler(ConsoleCtrlHandler, TRUE) != TRUE )
		return FALSE;

	bIsSuccess = theServer.StartServer(argv);

	return bIsSuccess;
}

BOOL WINAPI ConsoleCtrlHandler(const DWORD dwCtrlType)
{
	BOOL bIsSuccess = TRUE;

	switch ( dwCtrlType )
	{
	case CTRL_C_EVENT		:							break;
	case CTRL_BREAK_EVENT	:							break;
	case CTRL_LOGOFF_EVENT	:							break;
	case CTRL_SHUTDOWN_EVENT:							break;
	default					: bIsSuccess = FALSE;
	}

	return bIsSuccess;
}

