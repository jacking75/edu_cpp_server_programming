#include <WinSock2.h>
#include <Mswsock.h>
#include <process.h>

#include "Exception.h"
#include "IocpManager.h"
#include "ClientSession.h"
#include "IOThread.h"
#include "ClientSessionManager.h"


IocpManager* GIocpManager = nullptr;

char IocpManager::mAcceptBuf[64] = { 0, };


IocpManager::IocpManager() : mCompletionPort(NULL), mListenSocket(NULL)
{
	memset(mIoWorkerThread, 0, sizeof(mIoWorkerThread));
}


IocpManager::~IocpManager()
{
}

bool IocpManager::Initialize()
{
	/// winsock initializing
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) 
	{
		printf_s("[DEBUG] WSAStartup error: %d\n", WSAGetLastError());
		return false;
	}

	/// Create I/O Completion Port
	mCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (mCompletionPort == INVALID_HANDLE_VALUE) 
	{
		printf_s("[DEBUG] CreateIoCompletionPort error: %d\n", WSAGetLastError());
		return false;
	}

	/// create TCP socket
	mListenSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (mListenSocket == INVALID_SOCKET) 
	{
		printf_s("[DEBUG] create TCP socket error: %d\n", WSAGetLastError());
		return false;
	}

	HANDLE handle = CreateIoCompletionPort((HANDLE)mListenSocket, mCompletionPort, 0, 0);
	if (handle != mCompletionPort)
	{
		printf_s("[DEBUG] listen socket IOCP register error: %d\n", GetLastError());
		return false;
	}

	int opt = 1;
	setsockopt(mListenSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(int));

	/// bind
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(LISTEN_PORT);
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (SOCKET_ERROR == bind(mListenSocket, (SOCKADDR*)&serveraddr, sizeof(serveraddr))) 
	{
		printf_s("[DEBUG] listen socket bind error: %d\n", GetLastError());
		return false;
	}

		
	/// make session pool
	GClientSessionManager->PrepareClientSessions();

	printf_s("[DEBUG][%s] Success\n", __FUNCTION__);
	return true;
}


bool IocpManager::StartIoThreads()
{
	/// create I/O Thread
	for (int i = 0; i < MAX_IO_THREAD; ++i)
	{
		unsigned __int64 threadIndex = i;
		DWORD dwThreadId = 0;

		/// 스레드ID는 DB 스레드 이후에 IO 스레드로..
		HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, IoWorkerThread, (PVOID)threadIndex, CREATE_SUSPENDED, (unsigned int*)&dwThreadId);
		if (hThread == NULL) {
			return false;
		}

		mIoWorkerThread[i] = new IOThread(hThread, mCompletionPort);
		mIoWorkerThread[i]->SetManagedSendIOClientSessionIndex(GClientSessionManager->MaxClientSessionCount(), i, MAX_IO_THREAD);
	}

	/// start!
	for (int i = 0; i < MAX_IO_THREAD; ++i)
	{
		ResumeThread(mIoWorkerThread[i]->GetHandle());
	}

	return true;
}


void IocpManager::StartAccept()
{
	/// listen
	if (SOCKET_ERROR == listen(mListenSocket, SOMAXCONN))
	{
		printf_s("[DEBUG] listen error\n");
		return;
	}
		
	printf_s("[DEBUG][%s]\n", __FUNCTION__);

	while (GClientSessionManager->AcceptClientSessions())
	{
		Sleep(32);
	}

	printf_s("[DEBUG][%s] End Thread\n", __FUNCTION__);
}


void IocpManager::Finalize()
{
	for (int i = 0; i < MAX_IO_THREAD; ++i)
	{
		CloseHandle(mIoWorkerThread[i]->GetHandle());
	}

	CloseHandle(mCompletionPort);

	/// winsock finalizing
	WSACleanup();

}

unsigned int WINAPI IocpManager::IoWorkerThread(LPVOID lpParam)
{
	auto LWorkerThreadId = reinterpret_cast<unsigned __int64>(lpParam);

	printf_s("[DEBUG][%s] LWorkerThreadId: %I64u\n", __FUNCTION__, LWorkerThreadId);

	return GIocpManager->mIoWorkerThread[LWorkerThreadId]->Run();
}