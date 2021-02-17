// MyTestServer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "ServerManager.h"

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#ifdef __TBB_malloc_proxy
#include "tbb/scalable_allocator.h"
#endif


int main()
{
#ifdef __TBB_malloc_proxy
	scalable_allocation_mode(TBBMALLOC_SET_SOFT_HEAP_LIMIT, 4096);
#endif

	CServerBase::_StInitInfo testServerInfo;
	testServerInfo.serverType = CServerBase::LOGIN_SERVER;
	testServerInfo.nListenPortNumber = PORT_NUM;
	testServerInfo.nTransferThreadCount = TRANSFER_THREAD_COUNT;
	testServerInfo.nRecvThreadCount = RECV_THREAD_COUNT;
	testServerInfo.nSendThreadCount = SEND_THREAD_COUNT;

	CServerManager::GetInstance().Init();
	CServerManager::GetInstance().CreateServer(testServerInfo);
	CServerManager::GetInstance().Start();

	CServerManager::GetInstance().Close();


    return 0;
}

