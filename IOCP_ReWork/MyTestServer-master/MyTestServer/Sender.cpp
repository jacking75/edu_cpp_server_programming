#include "stdafx.h"
#include "sender.h"
#include "Session.h"
#include "ThreadRegister.hpp"
#include "Define.h"
#include "VirtualMemoryPool.h"
#include "VirtualMemoryPool.hpp"
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl_lite.h>


bool CSender::Start(VEC_IOCP_HANDER& hWorkerHanders_)
{
	int nWorkThreadCount = static_cast<int>(hWorkerHanders_.size());
	if (nWorkThreadCount <= 0)
	{
		// 로그
		return false;
	}

	m_vecWorkerHandle = hWorkerHanders_;
	m_nWorkThreadCount = nWorkThreadCount; 
	__super::RegisterThreadFunc(m_vecWorkerHandle, &CSender::SendProc);

	return true;
}


bool CSender::Push(CSession* session_)
{
	if (!IsRunning())
	{
		// 로그
		return false;
	}

	// worker로 작업 넘김
	HANDLE hIOHandle = GetWorkThreadHandle(session_->GetSessionID());
	::PostQueuedCompletionStatus(hIOHandle, NULL, reinterpret_cast<ULONG_PTR>(session_), NULL);

	return true;
}


inline HANDLE CSender::GetWorkThreadHandle(SESSION_ID sessionId_)
{
	int threadID = sessionId_ % m_nWorkThreadCount;
	return m_vecWorkerHandle[threadID];
}


inline void CSender::Close()
{
	m_bIsRunning = false;
}


bool CSender::SendPacketUnits(CSession *session_)
{
	PACKET_QUEUE& sendQueue = session_->SendInfo->queue;

	SPacketTransInfo* packetInfo;
	SPacketTransInfo::_StInit param(packetdef::Invalid, 0, session_);
	if (!session_->GetPoolObject(packetInfo, &param))
	{
		// 로그
		return false;
	}

	_WSABUF wsabuf;
	wsabuf.buf = static_cast<char*>(packetInfo->message);
	wsabuf.len = 0;
	protobuf::io::ArrayOutputStream os(wsabuf.buf, SPacketTransInfo::MAX_PACKET_SIZE);
	protobuf::io::CodedOutputStream cs(&os);

	while (!sendQueue.empty())
	{
		CPacketTransInfoBase* info;
		sendQueue.try_pop(info);
		if (info == NULL) {
			// 로그
			continue;
		}
		
		int packetSize = info->header.dataSize + packetdef::HeaderSize;
		cs.WriteRaw(info->message, packetSize);
		info->BackPoolObject();

		wsabuf.len += packetSize;
		if (wsabuf.len >= PACKET_SIZE)
			break;
	};

	DWORD dwSendBytes;
	do
	{
		if (::WSASend(session_->GetSocket(), &wsabuf, 1, &dwSendBytes, 0,
			packetInfo, NULL) == SOCKET_ERROR)
		{
			int err = WSAGetLastError();
			if (err != WSA_IO_PENDING)
			{
				if (err == WSAEWOULDBLOCK)
				{
					continue;
				}

				printf("WSAGetLastError = >%d\n", err);
				/// 처리..
				return false;
			}
		}
		break;
	} while (true);

	return true;
}


bool CSender::SendPacketUnit(CSession *session_)
{
	PACKET_QUEUE& sendQueue = session_->SendInfo->queue;

	CPacketTransInfoBase* info;
	if (!sendQueue.try_pop(info))
	{
		// 로그
		return false;
	}

	_WSABUF wsabuf;
	wsabuf.buf = static_cast<char*>(info->message);
	wsabuf.len = info->header.dataSize + packetdef::HeaderSize;

	DWORD dwSendBytes;
	do
	{
		if (::WSASend(session_->GetSocket(), &wsabuf, 1, &dwSendBytes, 0,
			info, NULL) == SOCKET_ERROR)
		{
			int err = WSAGetLastError();
			if (err != WSA_IO_PENDING)
			{
				if (err == WSAEWOULDBLOCK)
				{
					continue;
				}

				printf("WSAGetLastError = >%d\n", err);
				/// 처리..
				return false;
			}
		}
		break;
	} while (true);

	return true;
}


unsigned CALLBACK CSender::SendProc(void* data_)
{
	CSender::_StWaitingEvent* waitingInfo = reinterpret_cast<CSender::_StWaitingEvent*>(data_);
	if (waitingInfo == NULL)
	{
		// 로그
		return 0;
	}

	CSender* caller_ptr = waitingInfo->pCaller;
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
		OVERLAPPED* emptyOverlapped;
		DWORD		size;

		::GetQueuedCompletionStatus(waitingInfo->hIOCompltePort, &size, (PULONG_PTR)&session, &emptyOverlapped, INFINITE);

		caller_ptr->SendPacketUnit(session);
	}

	::CloseHandle(waitingInfo->hThread);
	::WSACloseEvent(waitingInfo->hWaitingEvent);
	return 0;
}


CSender::CSender()
{
}

CSender::~CSender()
{
	Close();
}