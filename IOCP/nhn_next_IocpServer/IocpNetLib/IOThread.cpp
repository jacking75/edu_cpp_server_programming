#include <stdio.h>

#include "ContentsConfig.h"
#include "Exception.h"
#include "IOThread.h"
#include "ClientSession.h"
#include "ServerSession.h"
#include "ClientSessionManager.h"
#include "IocpManager.h"

IOThread::IOThread(HANDLE hThread, HANDLE hCompletionPort) 
	: mThreadHandle(hThread), mCompletionPort(hCompletionPort)
{
}


IOThread::~IOThread()
{
	CloseHandle(mThreadHandle);
}

DWORD IOThread::Run()
{

	while (true)
	{
		DoIocpJob();

		DoSendJob(); ///< aggregated sends
	}

	return 1;
}

void IOThread::SetManagedSendIOClientSessionIndex(const int maxClientSessionCount, const int thisThreadIndex, const int maxThreadCount)
{
	auto averCount = (int)(maxClientSessionCount / maxThreadCount);
	
	m_SendIOClientSessionBeginIndex = averCount * thisThreadIndex;
	m_SendIOClientSessionEndIndex = m_SendIOClientSessionBeginIndex + averCount;

	if (thisThreadIndex == (maxThreadCount - 1)) 
	{
		auto restCount = (int)(maxClientSessionCount % maxThreadCount);
		m_SendIOClientSessionEndIndex += restCount;
	}
}

void IOThread::DoIocpJob()
{
	DWORD dwTransferred = 0;
	LPOVERLAPPED overlapped = nullptr;
	
	ULONG_PTR completionKey = 0;

	int ret = GetQueuedCompletionStatus(mCompletionPort, &dwTransferred, (PULONG_PTR)&completionKey, &overlapped, GQCS_TIMEOUT);

	
	OverlappedIOContext* context = reinterpret_cast<OverlappedIOContext*>(overlapped);
	
	Session* remote = context ? context->mSessionObject : nullptr;

	if (ret == 0 || dwTransferred == 0)
	{
		/// check time out first 
		if (context == nullptr && GetLastError() == WAIT_TIMEOUT) {
			return;
		}

	
		if (context->mIoType == IO_RECV || context->mIoType == IO_SEND)
		{
			CRASH_ASSERT(nullptr != remote);

			auto disconnectReason = static_cast<OverlappedDisconnectContext*>(context)->mDisconnectReason;
			DeleteIoContext(context);

			/// In most cases in here: ERROR_NETNAME_DELETED(64)
			remote->DisconnectCompletion(disconnectReason);
			
			return;
		}
	}

	CRASH_ASSERT(nullptr != remote);

	bool completionOk = false;
	switch (context->mIoType)
	{
	case IO_CONNECT:		
		completionOk = dynamic_cast<ServerSession*>(remote)->ConnectCompletion();

		// TODO: completionOk가 false인 경우 실패 이유가 설정 되어야 한다. remote->SetDisconnectReason
		break;
	
	case IO_ACCEPT:		
		completionOk = dynamic_cast<ClientSession*>(remote)->AcceptCompletion();

		// TODO: completionOk가 false인 경우 실패 이유가 설정 되어야 한다. remote->SetDisconnectReason
		break;

	case IO_SEND:
		remote->SendCompletion(dwTransferred);

		if (context->mWsaBuf.len != dwTransferred) {
			printf_s("Partial SendCompletion requested [%d], sent [%d]\n", context->mWsaBuf.len, dwTransferred);
		}
		else {
			completionOk = true;
		}
		
		// TODO: completionOk가 false인 경우 실패 이유가 설정 되어야 한다. remote->SetDisconnectReason
		break;

	case IO_RECV:
		remote->RecvCompletion(dwTransferred);
	
		completionOk = remote->PostRecv();

		// TODO: completionOk가 false인 경우 실패 이유가 설정 되어야 한다. remote->SetDisconnectReason
		break;

	default:
		printf_s("Unknown I/O Type: %d\n", context->mIoType);
		CRASH_ASSERT(false);
		break;
	}

	DeleteIoContext(context);

	if (!completionOk)
	{
		/// connection closing
		remote->DisconnectCompletion(DR_IO_REQUEST_ERROR);
	}	
}


void IOThread::DoSendJob()
{
	for (int i = m_SendIOClientSessionBeginIndex; i < m_SendIOClientSessionEndIndex; ++i)
	{
		auto session = GClientSessionManager->GetClientSession(i);

		if (session == nullptr) {
			break;
		}

		session->FlushSend();		
	}
	
}

