#pragma once

#include "Define.h"
#include <stdio.h>
#include <mutex>
#include <queue>
#include "TcpSession.h"
#include "../../LogicLib/LogicLib/PacketDef.h"

namespace NServerNetLib
{	

	void TcpSession::Init(const UINT32 index, HANDLE iocpHandle)
	{
		m_Index = index;
		m_IOCPHandle = iocpHandle;

		m_PakcetDataBuffer = new char[PACKET_DATA_BUFFER_SIZE];

		m_PakcetDataBufferWPos = 0;
		m_PakcetDataBufferRPos = 0;
	}


	bool TcpSession::OnConnect(HANDLE iocpHandle, SOCKET socket)
	{
		m_Socket = socket;
		m_IsConnect = 1;

		if (BindIOCompletionPort(iocpHandle) == false)
		{
			return false;
		}

		return BindRecv();
	}

	void TcpSession::Close(bool bIsForce)
	{
		struct linger stLinger = { 0, 0 };

		if (true == bIsForce)
		{
			stLinger.l_onoff = 1;
		}

		shutdown(m_Socket, SD_BOTH);

		setsockopt(m_Socket, SOL_SOCKET, SO_LINGER, (char*)&stLinger, sizeof(stLinger));

		m_IsConnect = 0;
		m_LatestClosedTimeSec = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now().time_since_epoch()).count();

		closesocket(m_Socket);
		m_Socket = INVALID_SOCKET;
	}

	bool TcpSession::PostAccept(SOCKET listenSock, const UINT64 curTimeSec)
	{
		m_LatestClosedTimeSec = UINT32_MAX;

		m_Socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_IP, NULL, 0, WSA_FLAG_OVERLAPPED);
		if (INVALID_SOCKET == m_Socket)
		{
			return false;
		}

		ZeroMemory(&m_AcceptContext, sizeof(stOverlappedEx));

		DWORD bytes = 0;
		DWORD flags = 0;
		m_AcceptContext.m_wsaBuf.len = 0;
		m_AcceptContext.m_wsaBuf.buf = nullptr;
		m_AcceptContext.m_eOperation = IOOperation::ACCEPT;
		m_AcceptContext.SessionIndex = m_Index;

		if (FALSE == AcceptEx(listenSock, m_Socket, m_AcceptBuf, 0,
			sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, &bytes, (LPWSAOVERLAPPED) & (m_AcceptContext)))
		{
			if (WSAGetLastError() != WSA_IO_PENDING)
			{
				return false;
			}
		}

		return true;
	}

	bool TcpSession::AcceptCompletion()
	{
		if (OnConnect(m_IOCPHandle, m_Socket) == false)
		{
			return false;
		}

		return true;
	}

	bool TcpSession::BindIOCompletionPort(HANDLE iocpHandle_)
	{
		auto hIOCP = CreateIoCompletionPort((HANDLE)GetSock(), iocpHandle_, (ULONG_PTR)(this), 0);

		if (hIOCP == INVALID_HANDLE_VALUE)
		{
			return false;
		}

		return true;
	}

	bool TcpSession::BindRecv()
	{
		DWORD dwFlag = 0;
		DWORD dwRecvNumBytes = 0;

		m_RecvOverlappedEx.m_wsaBuf.len = MAX_SOCK_RECVBUF;
		m_RecvOverlappedEx.m_wsaBuf.buf = m_RecvBuf;
		m_RecvOverlappedEx.m_eOperation = IOOperation::RECV;

		int nRet = WSARecv(m_Socket,
			&(m_RecvOverlappedEx.m_wsaBuf),
			1,
			&dwRecvNumBytes,
			&dwFlag,
			(LPWSAOVERLAPPED) & (m_RecvOverlappedEx),
			NULL);

		if (nRet == SOCKET_ERROR && (WSAGetLastError() != ERROR_IO_PENDING))
		{
			return false;
		}

		return true;
	}

	bool TcpSession::SendMsg(const short bodySize, char* pMsg)
	{
		auto sendOverlappedEx = new stOverlappedEx;
		ZeroMemory(sendOverlappedEx, sizeof(stOverlappedEx));
		sendOverlappedEx->m_wsaBuf.len = bodySize;
		sendOverlappedEx->m_wsaBuf.buf = new char[bodySize];
		CopyMemory(sendOverlappedEx->m_wsaBuf.buf, pMsg, bodySize);
		sendOverlappedEx->m_eOperation = IOOperation::SEND;


		std::lock_guard<std::mutex> guard(m_SendLock);

		m_SendDataqueue.push(sendOverlappedEx);

		if (m_SendDataqueue.size() == 1)
		{
			SendIO();
		}

		return true;
	}

	void TcpSession::SendCompleted(const UINT32 dataSize)
	{
		std::lock_guard<std::mutex> guard(m_SendLock);

		delete[] m_SendDataqueue.front()->m_wsaBuf.buf;
		delete m_SendDataqueue.front();

		m_SendDataqueue.pop();

		if (m_SendDataqueue.empty() == false)
		{
			SendIO();
		}
	}


	bool TcpSession::SendIO()
	{
		auto sendOverlappedEx = m_SendDataqueue.front();

		DWORD dwRecvNumBytes = 0;
		int nRet = WSASend(m_Socket,
			&(sendOverlappedEx->m_wsaBuf),
			1,
			&dwRecvNumBytes,
			0,
			(LPWSAOVERLAPPED)sendOverlappedEx,
			NULL);

		if (nRet == SOCKET_ERROR && (WSAGetLastError() != ERROR_IO_PENDING))
		{
			return false;
		}

		return true;
	}

	bool TcpSession::SetSocketOption()
	{
		int opt = 1;
		if (SOCKET_ERROR == setsockopt(m_Socket, IPPROTO_TCP, TCP_NODELAY, (const char*)&opt, sizeof(int)))
		{
			return false;
		}

		opt = 0;
		if (SOCKET_ERROR == setsockopt(m_Socket, SOL_SOCKET, SO_RCVBUF, (const char*)&opt, sizeof(int)))
		{
			return false;
		}

		return true;
	}

	void TcpSession::SetPacketData(const UINT32 dataSize, char* pData)
	{
		if ((m_PakcetDataBufferWPos + dataSize) >= PACKET_DATA_BUFFER_SIZE)
		{
			auto remainDataSize = m_PakcetDataBufferWPos - m_PakcetDataBufferRPos;

			if (remainDataSize > 0)
			{
				CopyMemory(&m_PakcetDataBuffer[0], &m_PakcetDataBuffer[m_PakcetDataBufferRPos], remainDataSize);
				m_PakcetDataBufferWPos = remainDataSize;
			}
			else
			{
				m_PakcetDataBufferWPos = 0;
			}

			m_PakcetDataBufferRPos = 0;
		}

		CopyMemory(&m_PakcetDataBuffer[m_PakcetDataBufferWPos], pData, dataSize);
		m_PakcetDataBufferWPos += dataSize;
	}

	RecvPacketInfo TcpSession::GetPacket()
	{
		UINT32 remainByte = m_PakcetDataBufferWPos - m_PakcetDataBufferRPos;

		if (remainByte < OmokServerLib::PACKET_HEADER_SIZE)
		{
			return RecvPacketInfo();
		}

		auto pHeader = (OmokServerLib::PktHeader*)&m_PakcetDataBuffer[m_PakcetDataBufferRPos];

		if (pHeader->TotalSize > remainByte)
		{
			return RecvPacketInfo();
		}

		RecvPacketInfo packetInfo;
		packetInfo.PacketId = pHeader->Id;
		packetInfo.PacketBodySize = pHeader->TotalSize;
		packetInfo.pRefData = &m_PakcetDataBuffer[m_PakcetDataBufferRPos];

		m_PakcetDataBufferRPos += pHeader->TotalSize;

		return packetInfo;
	}
	
	
}