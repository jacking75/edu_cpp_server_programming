// IOCPServer.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "ServiceManager.h"
#include "../../Common/LIB_SERVICECTRL.h"

const BOOL PreProcessParameter(const int argc, _TCHAR* argv[]);
BOOL WINAPI ConsoleCtrlHandler(const DWORD dwCtrlType);	// 디버그 콘솔 모드시
VOID WINAPI ServiceCtrlHandler(const DWORD Opcode);		// 릴리즈 서비스 모드시
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

#ifdef _NDEBUG
	if ( argc > 2 )
	{
		if ( !strcmp(argv[1], "-i") )
		{
			TCHAR	szServicePath[MAX_STRING] = _T("");

			wsprintf(szServicePath, "%s -s %s %s", szServiceDir, argv[2], argv[3]);

			COMMONLIB::LIB_SERVICECTRL::InstallServer(szServiceName, szServicePath);
		}
		else if ( !strcmp(argv[1], "-d") )
		{
			if ( COMMONLIB::LIB_SERVICECTRL::UnInstallServer(argv[2]) != TRUE )
				bIsSuccess = FALSE;
		}
		else if ( !strcmp(argv[1], "-s") )
		{
			SERVICE_TABLE_ENTRY DispatchTable[] = { {ServiceName, ServiceMain}, {NULL, NULL} };
			::StartServiceCtrlDispatcher(DispatchTable);
		}
	}
#else if _DEBUG
	_tprintf(_T("Start Console Mode\n"));

	if ( SetConsoleCtrlHandler(ConsoleCtrlHandler, TRUE) != TRUE )
		return FALSE;

	bIsSuccess = theServer.StartServer(argv);
#endif	// _NDEBUG & _DEBUG

	return bIsSuccess;
}

VOID WINAPI ServiceMain(DWORD argc, LPTSTR *argv)
{
	if ( COMMONLIB::LIB_SERVICECTRL::RegisterService(NAME_TESTSERVER, ServiceCtrlHandler) != TRUE )
		return;

	COMMONLIB::LIB_SERVICECTRL::UpdateServiceStatus(SERVICE_START_PENDING);
	COMMONLIB::LIB_SERVICECTRL::UpdateServiceStatus(SERVICE_RUNNING);

	theServer.StartServer(argv);	// ServerID 를 넣자

	COMMONLIB::LIB_SERVICECTRL::UpdateServiceStatus(SERVICE_STOPPED);

	theServer.m_Log.EventLog(1, "SERVER STOP");

	return;
}

BOOL WINAPI ConsoleCtrlHandler(const DWORD dwCtrlType)
{
	BOOL bIsSuccess = TRUE;

	switch ( dwCtrlType )
	{
	case CTRL_C_EVENT		:							break;
	case CTRL_BREAK_EVENT	:							break;
//	case CTRL_CLOSE_EVENT	: theServer.StopServer();	break;	// 굳이 할 필요 없다. 소멸자에서 호출됨으로
	case CTRL_LOGOFF_EVENT	:							break;
	case CTRL_SHUTDOWN_EVENT:							break;
	default					: bIsSuccess = FALSE;
	}

	return bIsSuccess;
}

VOID WINAPI ServiceCtrlHandler(const DWORD dwCtrlType)
{
	switch ( dwCtrlType )
	{
	case SERVICE_CONTROL_STOP		:
	case SERVICE_CONTROL_SHUTDOWN	:	COMMONLIB::LIB_SERVICECTRL::UpdateServiceStatus(SERVICE_STOP_PENDING);
										theServer.StopServer();	break;
	case SERVICE_CONTROL_INTERROGATE:	break;
	}
}
