#include "stdafx.h"
#include "Listener.h"
#include "ThreadRegister.hpp"
#include "AttachServerMessage.h"

#pragma comment(lib,"mswsock.lib")  


SOCKET CListener::Start(HANDLE hCompletionPort_, u_short nPortNum_)
{
	m_listenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

	SOCKADDR_IN servAddr;
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr.sin_port = htons(nPortNum_);

	//LINGER ls;
	//ls.l_onoff = 1;
	//ls.l_linger = 0;
	//::setsockopt(m_listenSocket, SOL_SOCKET, SO_LINGER, reinterpret_cast<const char *>(&ls), sizeof(ls));
	bool bValid = true;
	::setsockopt(m_listenSocket, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char *>(&bValid), sizeof(bValid));

	if (::bind(m_listenSocket, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR)
	{
		// 로그
		return NULL;
	}

	if (::listen(m_listenSocket, MAX_CONNECTER) == SOCKET_ERROR)
	{
		// 로그
		return NULL;
	}

	__super::RegisterThreadFunc(1, hCompletionPort_, &CListener::AcceptProc);

	return m_listenSocket;
}


bool CListener::AttachModule(CAttachModule* module_)
{
	m_moduleList.push_back(module_);
	return true;
}


void CListener::Close()
{
	m_bIsRunning = false;
	m_moduleList.clear();
}


unsigned __stdcall CListener::AcceptProc(void* data_)
{
	CListener::_StWaitingEvent* waitingInfo = reinterpret_cast<CListener::_StWaitingEvent*>(data_);
	if (waitingInfo == NULL)
	{
		// 로그
		return 0;
	}

	CListener* caller_ptr = waitingInfo->pCaller;
	if (caller_ptr == NULL)
	{
		// 로그
		::CloseHandle(waitingInfo->hThread);
		return 0;
	}

	/// 쓰레드가 만들어 졌으므로 m_hWaitingEvent 핸들에 대해 signal으로 변경
	::WSASetEvent(waitingInfo->hWaitingEvent);

	sockaddr_in clientAddr;
	int nAddrLen = sizeof(clientAddr);

	while (caller_ptr->IsRunning())
	{
		SOCKET hClientSock = ::WSAAccept(caller_ptr->m_listenSocket, (sockaddr*)&clientAddr, &nAddrLen, NULL, NULL);
		if (hClientSock == INVALID_SOCKET)
		{
			if (WSAGetLastError() == WSAEWOULDBLOCK)
			{
				continue;
			} 
			// 로그
		}
		
		/// 주로 쓰이는 쓰레드가 아니므로 여기서는 매니저에 등록 없이 session의 객체만 만든다.
		CSession* session = caller_ptr->m_sessionMgr->CreateSession(hClientSock);
		if (session == NULL)
		{
			// 로그
			::CloseHandle(waitingInfo->hThread);
			return 0;
		}
		
		::CreateIoCompletionPort((HANDLE)hClientSock, waitingInfo->hIOCompltePort, reinterpret_cast<ULONG_PTR>(session), 0);

		/// 모듈에 등록되어 있는 모든 accept 모듈 함수들 수행
		if (caller_ptr->m_moduleList.size() > 0)
		{
			for (ATTACH_MODULE_LIST::iterator it = caller_ptr->m_moduleList.begin(); it != caller_ptr->m_moduleList.end(); ++it)
			{
				(*it)->Accept(session->GetSessionID(), session);
			}
		}

		DWORD dwFlag = 0;
		do
		{
			if (::WSARecv(session->GetSocket(),
				&(session->RecvInfo->wsa),
				1,
				NULL,
				&dwFlag,
				session->RecvInfo->overlapped,
				NULL
			) == SOCKET_ERROR)
			{
				int err = WSAGetLastError();
				if (err != WSA_IO_PENDING)
				{
					if (err == WSAEWOULDBLOCK)
					{
						continue;
					}
					// 로그
				}
			}
			break;
		} while (true);
	}

	::CloseHandle(waitingInfo->hThread);
	::WSACloseEvent(waitingInfo->hWaitingEvent);

	return 0;
}

CListener::CListener(CSessionManager* pSessionMgr) :
	m_sessionMgr(pSessionMgr)
{
}

CListener::~CListener()
{
	Close();
}