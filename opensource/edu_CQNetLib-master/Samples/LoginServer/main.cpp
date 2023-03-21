#include <iostream>

#include "../../CQNetLib/commonDef.h"
#include "../../CQNetLib/log.h"

#include "LoginIocpServer.h"


BOOL WINAPI ConsoleCtrlHandler(const DWORD dwCtrlType);

int main()
{
	if (SetConsoleCtrlHandler(ConsoleCtrlHandler, TRUE) != TRUE)
	{
		return 0;
	}

	g_pLoginIocpServer->ServerStart();

	LOG(CQNetLib::LOG_INFO_NORMAL, "SYSTEM | CMainFrame::OnServerStop() | 서버 멈춤 시작..");
	
	auto pTickThread = g_pLoginIocpServer->GetTickThread();
	if (pTickThread)
	{
		pTickThread->Stop();
		LOG(CQNetLib::LOG_INFO_NORMAL, "SYSTEM | CMainFrame::OnServerStop() | TickThread Stop 성공..");
	}

	g_pLoginIocpServer->ServerOff();

	LOG(CQNetLib::LOG_INFO_NORMAL, "SYSTEM | CMainFrame::OnServerStop() | 서버 멈춤 성공..");

	return 0;
}

BOOL WINAPI ConsoleCtrlHandler(const DWORD dwCtrlType)
{
	BOOL bIsSuccess = TRUE;

	switch (dwCtrlType)
	{
	case CTRL_C_EVENT:
		break;
	case CTRL_BREAK_EVENT:
		break;
		//	case CTRL_CLOSE_EVENT	: theServer.StopServer();	break;	// 굳이 할 필요 없다. 소멸자에서 호출됨으로
	case CTRL_LOGOFF_EVENT:
		break;
	case CTRL_SHUTDOWN_EVENT:
		break;
	default: bIsSuccess = FALSE;
	}

	return bIsSuccess;
}
