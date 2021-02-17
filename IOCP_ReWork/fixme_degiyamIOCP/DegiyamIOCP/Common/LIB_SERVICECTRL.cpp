
#include "LIB_SERVICECTRL.h"

using namespace COMMONLIB;

SERVICE_STATUS_HANDLE LIB_SERVICECTRL::theServiceStatusHandle = (SERVICE_STATUS_HANDLE)0;
SERVICE_STATUS LIB_SERVICECTRL::theServiceStatus;

const BOOL LIB_SERVICECTRL::InstallServer(const LPCTSTR strServiceName, const LPCTSTR strServiceDir)
{
	SC_HANDLE hSCM		= NULL;
	SC_HANDLE hService	= NULL;

	hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if ( hSCM == NULL )
	{
		// SCM 을 열 수 없습니다.
		return FALSE;
	}

	// 서비스를 등록한다.
	hService = CreateService(
		hSCM,						// handle to SCM database
		strServiceName,				// name of service to start
		strServiceName,				// display name
		SERVICE_PAUSE_CONTINUE | SERVICE_CHANGE_CONFIG,	// type of access to service
		SERVICE_WIN32_OWN_PROCESS,	// type of service
		SERVICE_DEMAND_START,		// when to start service
		SERVICE_ERROR_IGNORE,		// serverity of service failure
		strServiceDir,				// name of binary file
		NULL,						// name of load ordering group
		NULL,						// tag identifier
		NULL,						// array of dependency names
		NULL,						// account name
		NULL);						// account password

	if ( hService == NULL )
	{
		// 설치하지 못했습니다.
		CloseServiceHandle(hService);
		CloseServiceHandle(hSCM);
		return FALSE;
	}

	CloseServiceHandle(hService);
	CloseServiceHandle(hSCM);

	return TRUE;
}

const BOOL LIB_SERVICECTRL::UnInstallServer(const LPCTSTR strServiceName)
{
	SC_HANDLE		hSCM		= NULL;
	SC_HANDLE		hService	= NULL;
	SERVICE_STATUS	ssStatus;

	hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if ( hSCM == NULL )
	{
		// SCM 을 열 수 없습니다.
		return FALSE;
	}

	hService = OpenService(hSCM, strServiceName, SERVICE_ALL_ACCESS);
	if ( hService == NULL )
	{
		CloseServiceHandle(hSCM);
		// 서비스가 설치되어 있지 않았습니다.
		return FALSE;
	}

	// 실행중이면 중지시킨다.
	QueryServiceStatus(hService, &ssStatus);
	if ( ssStatus.dwCurrentState != SERVICE_STOPPED )
	{
		if ( ControlService( hService, SERVICE_CONTROL_STOP, &ssStatus ) )
		{
			Sleep( 1000 );

			while( QueryServiceStatus(hService, &ssStatus) )
			{
				if ( ssStatus.dwCurrentState == SERVICE_STOP_PENDING )
					Sleep( 1000 );
				else
					break;
			}
		}
	}

	// 서비스 제거
	if ( DeleteService(hService) )
	{
		// 서비스를 제거했습니다.
	}
	else
	{
		// 서비스를 제거하지 못했습니다.
		CloseServiceHandle(hService);
		CloseServiceHandle(hSCM);
		return FALSE;
	}

	CloseServiceHandle(hService);
	CloseServiceHandle(hSCM);

	return TRUE;
}

const DWORD LIB_SERVICECTRL::ServiceQueryState(const LPCTSTR strServiceName)
{
	SC_HANDLE		hSCM		= NULL;
	SC_HANDLE		hService	= NULL;
	SERVICE_STATUS	ssStatus;

	hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if ( hSCM == NULL )
	{
		// SCM 을 열 수 없습니다.
		return SERVICESTATE_SCMERROR;
	}

	hService = OpenService(hSCM, strServiceName, SERVICE_ALL_ACCESS);
	if ( hService == NULL )
	{
		CloseServiceHandle(hSCM);
		// 서비스가 설치되어 있지 않았습니다.
		return SERVICESTATE_NOTINSTALL;
	}

	if(QueryServiceStatus(hService, &ssStatus) == FALSE) 
	{
		CloseServiceHandle(hSCM);
		return SERVICESTATE_ERROR;
	}

	return ssStatus.dwCurrentState;
}

const BOOL LIB_SERVICECTRL::ServiceStart(const LPCTSTR strServiceName)
{
	SC_HANDLE		hSCM		= NULL;
	SC_HANDLE		hService	= NULL;
	SERVICE_STATUS	ssStatus;
	ssStatus.dwWaitHint			= 1000;

	hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if ( hSCM == NULL )
	{
		// SCM 을 열 수 없습니다.
		return FALSE;
	}

	hService = OpenService(hSCM, strServiceName, SERVICE_ALL_ACCESS);
	if ( hService == NULL )
	{
		CloseServiceHandle(hSCM);
		// 서비스가 설치되어 있지 않았습니다.
		return FALSE;
	}

	if ( StartService(hService, 0, NULL) == TRUE ) 
	{
		for ( int i = 0; i < 5; i++ )
		{
			Sleep(ssStatus.dwWaitHint);
			QueryServiceStatus(hService, &ssStatus);
			if ( ssStatus.dwCurrentState == SERVICE_RUNNING )
				break;
		}
		
		if ( ssStatus.dwCurrentState != SERVICE_RUNNING )
		{
			CloseServiceHandle(hService);
			CloseServiceHandle(hSCM);
			return FALSE;
		}
	}
	else 
	{
		CloseServiceHandle(hService);
		CloseServiceHandle(hSCM);
		return FALSE;
	}
	
	return TRUE;
}

const BOOL LIB_SERVICECTRL::ServiceStop(const LPCTSTR strServiceName)
{
	SC_HANDLE		hSCM		= NULL;
	SC_HANDLE		hService	= NULL;
	SERVICE_STATUS	ssStatus;

	hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if ( hSCM == NULL )
	{
		// SCM 을 열 수 없습니다.
		return FALSE;
	}

	hService = OpenService(hSCM, strServiceName, SERVICE_ALL_ACCESS);
	if ( hService == NULL )
	{
		CloseServiceHandle(hSCM);
		// 서비스가 설치되어 있지 않았습니다.
		return FALSE;
	}

	QueryServiceStatus(hService, &ssStatus);
	if(ssStatus.dwCurrentState != SERVICE_RUNNING) 
	{
		CloseServiceHandle(hService);
		CloseServiceHandle(hSCM);
		return FALSE;
	}

	ControlService(hService, SERVICE_CONTROL_STOP, &ssStatus);
	for ( int i = 0; i < 5; i++ )
	{
		Sleep(200);
		QueryServiceStatus(hService, &ssStatus);
		if ( ssStatus.dwCurrentState == SERVICE_STOPPED )
			break;
	}

	if ( ssStatus.dwCurrentState != SERVICE_STOPPED ) 
	{
		CloseServiceHandle(hService);
		CloseServiceHandle(hSCM);
		return FALSE;
	}

	CloseServiceHandle(hService);
	CloseServiceHandle(hSCM);

	return TRUE;
}

const BOOL LIB_SERVICECTRL::RegisterService(const LPCTSTR strServiceName, const LPHANDLER_FUNCTION lpHandlerProc)
{
	DWORD status								= 0;
	DWORD specificError							= 0;
	theServiceStatus.dwServiceType				= SERVICE_WIN32;
	theServiceStatus.dwCurrentState				= SERVICE_STOPPED;
	theServiceStatus.dwControlsAccepted			= SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
	theServiceStatus.dwWin32ExitCode			= 0;
	theServiceStatus.dwServiceSpecificExitCode	= 0;
	theServiceStatus.dwCheckPoint				= 0;
	theServiceStatus.dwWaitHint					= 0;

	theServiceStatusHandle = RegisterServiceCtrlHandler(strServiceName, lpHandlerProc); 
	if ( theServiceStatusHandle == (SERVICE_STATUS_HANDLE)0 )
	{
		DWORD dwError = ::GetLastError();
		if ( dwError == ERROR_INVALID_NAME )
		{
//			_tprintf(_T("service name is invalid\n"));
		}
		else if ( dwError == ERROR_SERVICE_DOES_NOT_EXIST )
		{
//			_tprintf(_T("service does not exist\n"));
		}
		else
		{

		}

		return FALSE;
	}

	return TRUE;
}

const VOID LIB_SERVICECTRL::UpdateServiceStatus(const DWORD dwStatus)
{
	theServiceStatus.dwCurrentState	 = dwStatus;
	theServiceStatus.dwWin32ExitCode = 0;
	theServiceStatus.dwCheckPoint	 = 0;
	theServiceStatus.dwWaitHint		 = 0;

	::SetServiceStatus(theServiceStatusHandle, &theServiceStatus);
}