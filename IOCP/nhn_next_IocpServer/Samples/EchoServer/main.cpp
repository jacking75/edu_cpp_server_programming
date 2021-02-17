#include <iostream>

#include "Exception.h"
#include "ServerSession.h"
#include "ClientSession.h"
#include "ClientSessionManager.h"
#include "IocpManager.h"

BOOL WINAPI ConsoleCtrlHandler(const DWORD dwCtrlType);

int main()
{
	if (SetConsoleCtrlHandler(ConsoleCtrlHandler, TRUE) != TRUE) 
	{
		return 0;
	}

	/// for dump on crash
	SetUnhandledExceptionFilter(ExceptionFilter);

	/// Global Managers
	GClientSessionManager = new ClientSessionManager;
	GIocpManager = new IocpManager;
	

	if (false == GIocpManager->Initialize()) 
	{
		std::cout << "Fail GIocpManager->Initialize" << std::endl;
		return -1;
	}
		
	if (false == GIocpManager->StartIoThreads()) 
	{
		std::cout << "Fail GIocpManager->StartIoThreads" << std::endl;
		return -1;
	}

	std::cout << "Start Server" << std::endl;
	
	/*ServerSession* testServerSession = new ServerSession(CONNECT_SERVER_ADDR, CONNECT_SERVER_PORT);
	if (false == testServerSession->ConnectRequest())
	{
		printf_s("Connect Server [%s] Error \n", CONNECT_SERVER_ADDR);
	}*/


	GIocpManager->StartAccept(); ///< block here...


	GIocpManager->Finalize();

	std::cout << "End Server" << std::endl;

	//delete testServerSession;
	delete GIocpManager;
	delete GClientSessionManager;

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