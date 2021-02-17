#include "stdafx.h"
#include "Transfer.h"
#include "ThreadRegister.hpp"
#include "Session.h"
#include "AttachModule.h"
#include "ServerBase.h"
#include "Common/VirtualMemoryPool.hpp"


inline void CTransfer::RunRecvHandler(CSession* session_, DWORD bytes_)
{
	/// Recv 핸들러에 등록되어 있는 모든 함수들 수행
	for (ATTACH_MODULE_LIST::iterator it = m_moduleList.begin(); it != m_moduleList.end(); ++it)
	{
		(*it)->Recv(session_->GetSessionID(), session_, bytes_);
	}
}


inline void CTransfer::RunSendHandler(CSession* session_, DWORD bytes_)
{
	/// Send 핸들러에 등록되어 있는 모든 함수들 수행
	for (ATTACH_MODULE_LIST::iterator it = m_moduleList.begin(); it != m_moduleList.end(); ++it)
	{
		(*it)->Send(session_->GetSessionID(), session_, bytes_);
	}
}


inline void CTransfer::RunCloseHandler(CSession* session_, int reason_)
{
	/// Close 핸들러에 등록되어 있는 모든 함수들 수행
	for (ATTACH_MODULE_LIST::iterator it = m_moduleList.begin(); it != m_moduleList.end(); ++it)
	{
		(*it)->SessionClose(session_, reason_);
	}

	m_pServer->SessionClose(session_);
}


bool CTransfer::Start(HANDLE hCompletionPort_, int nTransperThreadCount_, VEC_IOCP_HANDER& hWorkerHanders_)
{
	int nWorkThreadCount = static_cast<int>(hWorkerHanders_.size());
	if (nWorkThreadCount <= 0)
	{
		// 로그
		return false;
	}

	m_vecWorkerHandle = hWorkerHanders_;
	m_nRecvThreadCount = nWorkThreadCount;
	__super::RegisterThreadFunc(nTransperThreadCount_, hCompletionPort_, &CTransfer::TransferProc);

	return true;
}


inline HANDLE CTransfer::GetWorkThreadHandle(THREAD_REGION threadRegion_)
{
	//int threadID = threadRegion_ % m_nRecvThreadCount;
	//return m_vecWorkerHandle[threadID];
	return m_vecWorkerHandle[threadRegion_];
}


inline CSender* CTransfer::GetSender() const
{
	return m_pSender;
}


bool CTransfer::AttachModule(CAttachModule* module_)
{
	m_moduleList.push_back(module_);
	return true;
}


inline void CTransfer::Close()
{
	m_bIsRunning = false;
	m_moduleList.clear();
}


unsigned CALLBACK CTransfer::TransferProc(void* data_)
{
	CTransfer::_StWaitingEvent* waitingInfo = reinterpret_cast<CTransfer::_StWaitingEvent*>(data_);
	if (waitingInfo == NULL)
	{
		// 로그
		return 0;
	}

	CTransfer* caller_ptr = waitingInfo->pCaller;
	if (caller_ptr == NULL)
	{
		// 로그
		::CloseHandle(waitingInfo->hThread);
		return 0;
	}

	/// 쓰레드가 만들어 졌으므로 hWaitingEvent 핸들에 대해 signal으로 변경
	::WSASetEvent(waitingInfo->hWaitingEvent);

	while (caller_ptr->IsRunning())
	{
		CSession*   session;
		
		LPOVERLAPPED overlapped;
		DWORD       dwBytesTransfer;
		DWORD       dwFlag = 0;

		if (::GetQueuedCompletionStatus(waitingInfo->hIOCompltePort,
			&dwBytesTransfer,
			reinterpret_cast<PULONG_PTR>(&session),
			&overlapped,
			INFINITE) == FALSE)
		{
			DWORD dwIOError = ::WSAGetLastError();
			if (dwIOError != WAIT_TIMEOUT)
			{
				if (dwIOError == ERROR_NETNAME_DELETED)
				{
					// 이유
					caller_ptr->RunCloseHandler(session, 0);
					continue;
				}

				if (overlapped == NULL)
				{
					// 이유
					caller_ptr->RunCloseHandler(session, 0);
					continue;
				}
			}
		}

		if (dwBytesTransfer == 0)
		{
			// 이유
			caller_ptr->RunCloseHandler(session, 0);
			continue;
		}

		CPacketTransInfoBase* transInfo = static_cast<CPacketTransInfoBase*>(overlapped);

		switch (transInfo->GetType())
		{
			case CPacketTransInfoBase::TYPE_SEND:
			{
				caller_ptr->RunSendHandler(session, dwBytesTransfer);
				transInfo->BackPoolObject();

			//	while (!session->SendInfo->queue.empty())
			//	{
			//		caller_ptr->GetSender()->SendPacketUnits(session);
			//	}			
			}
			continue;

			case CPacketTransInfoBase::TYPE_RECV:
			{
				caller_ptr->RunRecvHandler(session, dwBytesTransfer);

				/// worker로 작업 넘김
				HANDLE hIOHandle = caller_ptr->GetWorkThreadHandle(session->GetThreadRegion());
				::PostQueuedCompletionStatus(hIOHandle, dwBytesTransfer, reinterpret_cast<ULONG_PTR>(session), NULL);
			}
			break;

			default:
			{
				// 로그
				break;
			}
		}
	}
	::CloseHandle(waitingInfo->hThread);
	::WSACloseEvent(waitingInfo->hWaitingEvent);

	return 0;
}


CTransfer::CTransfer(CSender* sender_, CServerBase* server_) :
	m_pSender(sender_),
	m_pServer(server_)
{	
}

CTransfer::~CTransfer()
{
	Close();
}