#pragma once

#include <mutex>

#define WIN32_LEAN_AND_MEAN 
#include <WinSock2.h>
#include <mswsock.h>

#include "RingBuffer.h"
#include "NetDefine.h"

//TODO 네트워크 api를 호출하는 부분은 따로 함수로 분리한다

namespace NetLib
{	
	//TODO 델리게이션을 사용하여 IOCPServer 클래스의 HandleExceptionCloseConnection 함수를 호출 할 수 있어야 한다.

	class Connection
	{
	public:	
		Connection() {}
		~Connection() {}

		Message* GetConnectionMsg() { return m_pConnectionMsg; } 
		Message* GetCloseMsg() { return m_pCloseMsg; }
		

		void Init(const SOCKET listenSocket, const int index, const ConnectionNetConfig config)
		{
			m_ListenSocket = listenSocket;
			m_Index = index;
			m_RecvBufSize = config.MaxRecvOverlappedBufferSize;
			m_SendBufSize = config.MaxSendOverlappedBufferSize;

			Init();

			m_pRecvOverlappedEx = new OVERLAPPED_EX(index);
			m_pSendOverlappedEx = new OVERLAPPED_EX(index);
			m_RingRecvBuffer.Create(config.MaxRecvConnectionBufferCount);
			m_RingSendBuffer.Create(config.MaxSendConnectionBufferCount);

			m_pConnectionMsg = new Message { OP_CONNECTION, nullptr };
			m_pCloseMsg = new Message{ OP_CLOSE, nullptr };

			BindAcceptExSocket();
		}
	
		bool CloseComplete(void)
		{			
			//소켓만 종료한 채로 전부 처리될 때까지 대기
			if (IsConnect() && (m_AcceptIORefCount != 0 || m_RecvIORefCount != 0 || m_SendIORefCount != 0) )
			{
				DisconnectConnection();
				return false;
			}

			//한번만 접속 종료 처리를 하기 위해 사용
			if (InterlockedCompareExchange(reinterpret_cast<LPLONG>(&m_IsClosed), TRUE, FALSE) == static_cast<long>(FALSE))
			{
				return true;
			}

			return false;
		}
		
		void DisconnectConnection(void)
		{
			SetNetStateDisConnection();

			std::lock_guard<std::mutex> Lock(m_MUTEX);

			//shutdown(m_ClientSocket, SD_BOTH);
			if (m_ClientSocket != INVALID_SOCKET)
			{
				closesocket(m_ClientSocket);
				m_ClientSocket = INVALID_SOCKET;
			}			
		}
		
		E_NET_RESULT ResetConnection(void)
		{
			std::lock_guard<std::mutex> Lock(m_MUTEX);

			if (m_pRecvOverlappedEx != nullptr)
			{
				m_pRecvOverlappedEx->OverlappedExRemainByte = 0;
				m_pRecvOverlappedEx->OverlappedExTotalByte = 0;
			}

			if (m_pSendOverlappedEx != nullptr)
			{
				m_pSendOverlappedEx->OverlappedExRemainByte = 0;
				m_pSendOverlappedEx->OverlappedExTotalByte = 0;
			}

			Init();
			return BindAcceptExSocket();
		}
		
		bool BindIOCP(const HANDLE hWorkIOCP)
		{
			std::lock_guard<std::mutex> Lock(m_MUTEX);

			//즉시 접속 종료하기 위한 소켓 옵션 추가
			linger li = { 0, 0 };
			li.l_onoff = 1;
			setsockopt(m_ClientSocket, SOL_SOCKET, SO_LINGER, reinterpret_cast<char*>(&li), sizeof(li));

			auto hIOCPHandle = CreateIoCompletionPort(
				reinterpret_cast<HANDLE>(m_ClientSocket),
				hWorkIOCP,
				reinterpret_cast<ULONG_PTR>(this),
				0);

			if (hIOCPHandle == INVALID_HANDLE_VALUE || hWorkIOCP != hIOCPHandle)
			{
				return false;
			}

			return true;
		}
		
		E_NET_RESULT PostRecv(const char* pNextBuf, const DWORD remainByte)
		{
			if (m_IsConnect == FALSE || m_pRecvOverlappedEx == nullptr)
			{
				return E_NET_RESULT::PostRecv_Null_Obj;
			}

			m_pRecvOverlappedEx->OverlappedExOperationType = OP_RECV;
			m_pRecvOverlappedEx->OverlappedExRemainByte = remainByte;

			auto moveMark = static_cast<int>(remainByte - (m_RingRecvBuffer.GetCurMark() - pNextBuf));
			m_pRecvOverlappedEx->OverlappedExWsaBuf.len = m_RecvBufSize;
			m_pRecvOverlappedEx->OverlappedExWsaBuf.buf = m_RingRecvBuffer.ForwardMark(moveMark, m_RecvBufSize, remainByte);

			if (m_pRecvOverlappedEx->OverlappedExWsaBuf.buf == nullptr)
			{
				return E_NET_RESULT::PostRecv_Null_WSABUF;
			}

			m_pRecvOverlappedEx->pOverlappedExSocketMessage = m_pRecvOverlappedEx->OverlappedExWsaBuf.buf - remainByte;

			ZeroMemory(&m_pRecvOverlappedEx->Overlapped, sizeof(OVERLAPPED));

			IncrementRecvIORefCount();

			DWORD flag = 0;
			DWORD recvByte = 0;
			auto result = WSARecv(
				m_ClientSocket,
				&m_pRecvOverlappedEx->OverlappedExWsaBuf,
				1,
				&recvByte,
				&flag,
				&m_pRecvOverlappedEx->Overlapped,
				NULL);

			if (result == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING)
			{
				DecrementRecvIORefCount();
				return E_NET_RESULT::PostRecv_Null_Socket_Error;
			}

			return E_NET_RESULT::Success;
		}
		
		bool PostSend(const int sendSize)
		{
			//남은 패킷이 존재하는지 확인하기 위한 과정
			if (sendSize > 0)
			{
				m_RingSendBuffer.SetUsedBufferSize(sendSize);
			}

			if (InterlockedCompareExchange(reinterpret_cast<LPLONG>(&m_IsSendable), FALSE, TRUE))
			{
				auto readSize = 0;
				char* pBuf = m_RingSendBuffer.GetBuffer(m_SendBufSize, readSize);
				if (pBuf == nullptr)
				{
					InterlockedExchange(reinterpret_cast<LPLONG>(&m_IsSendable), TRUE);
					return true;
				}

				ZeroMemory(&m_pSendOverlappedEx->Overlapped, sizeof(OVERLAPPED));

				m_pSendOverlappedEx->OverlappedExWsaBuf.len = readSize;
				m_pSendOverlappedEx->OverlappedExWsaBuf.buf = pBuf;
				m_pSendOverlappedEx->ConnectionIndex = GetIndex();

				m_pSendOverlappedEx->OverlappedExRemainByte = 0;
				m_pSendOverlappedEx->OverlappedExTotalByte = readSize;
				m_pSendOverlappedEx->OverlappedExOperationType = OP_SEND;

				IncrementSendIORefCount();

				DWORD flag = 0;
				DWORD sendByte = 0;
				auto result = WSASend(
					m_ClientSocket,
					&m_pSendOverlappedEx->OverlappedExWsaBuf,
					1,
					&sendByte,
					flag,
					&m_pSendOverlappedEx->Overlapped,
					NULL);

				if (result == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING)
				{
					DecrementSendIORefCount();
					return false;					
				}
			}
			return true;
		}
		
		E_NET_RESULT ReservedSendPacketBuffer(OUT char** ppBuf, const int sendSize)
		{
			if (!m_IsConnect)
			{
				*ppBuf = nullptr;
				return E_NET_RESULT::ReservedSendPacketBuffer_Not_Connected;
			}

			*ppBuf = m_RingSendBuffer.ForwardMark(sendSize);
			if (*ppBuf == nullptr)
			{
				return E_NET_RESULT::ReservedSendPacketBuffer_Empty_Buffer;
			}

			return E_NET_RESULT::Success;
		}


		SOCKET GetClientSocket(void) { return m_ClientSocket; }

		void SetConnectionIP(const char* szIP) { CopyMemory(m_szIP, szIP, MAX_IP_LENGTH); }
		int GetIndex(void) { return m_Index; }

		int IncrementRecvIORefCount(void) { return InterlockedIncrement(reinterpret_cast<LPLONG>(&m_RecvIORefCount)); }
		int IncrementSendIORefCount(void) { return InterlockedIncrement(reinterpret_cast<LPLONG>(&m_SendIORefCount)); }
		int IncrementAcceptIORefCount(void) { return InterlockedIncrement(reinterpret_cast<LPLONG>(&m_AcceptIORefCount)); }
		int DecrementRecvIORefCount(void) { return (m_RecvIORefCount ? InterlockedDecrement(reinterpret_cast<LPLONG>(&m_RecvIORefCount)) : 0); }
		int DecrementSendIORefCount(void) { return (m_SendIORefCount ? InterlockedDecrement(reinterpret_cast<LPLONG>(&m_SendIORefCount)) : 0); }
		int DecrementAcceptIORefCount(void) { return (m_AcceptIORefCount ? InterlockedDecrement(reinterpret_cast<LPLONG>(&m_AcceptIORefCount)) : 0); }

		bool IsConnect() { return m_IsConnect; }

		void SetNetStateConnection()
		{
			InterlockedExchange(reinterpret_cast<LPLONG>(&m_IsConnect), TRUE);
		}

		void SetNetStateDisConnection()
		{
			InterlockedExchange(reinterpret_cast<LPLONG>(&m_IsConnect), FALSE);
		}

		INT32 RecvBufferSize() { return m_RingRecvBuffer.GetBufferSize(); }

		char* RecvBufferBeginPos() {	return m_RingRecvBuffer.GetBeginMark();	}

		void RecvBufferReadCompleted(const INT32 size)
		{
			m_RingRecvBuffer.ReleaseBuffer(size);
		}

		bool SetNetAddressInfo()
		{
			SOCKADDR* pLocalSockAddr = nullptr;
			SOCKADDR* pRemoteSockAddr = nullptr;

			int	localSockaddrLen = 0;
			int	remoteSockaddrLen = 0;

			GetAcceptExSockaddrs(m_AddrBuf, 0,
				sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16,
				&pLocalSockAddr, &localSockaddrLen,
				&pRemoteSockAddr, &remoteSockaddrLen);

			if (remoteSockaddrLen != 0)
			{
				SOCKADDR_IN* pRemoteSockAddrIn = reinterpret_cast<SOCKADDR_IN*>(pRemoteSockAddr);
				if (pRemoteSockAddrIn != nullptr)
				{
					char szIP[MAX_IP_LENGTH] = { 0, };
					inet_ntop(AF_INET, &pRemoteSockAddrIn->sin_addr, szIP, sizeof(szIP));

					SetConnectionIP(szIP);
				}

				return true;
			}
			
			return false;
		}

		void SendBufferSendCompleted(const INT32 sendSize)
		{
			m_RingSendBuffer.ReleaseBuffer(sendSize);
		}

		void SetEnableSend()
		{
			InterlockedExchange(reinterpret_cast<LPLONG>(&m_IsSendable), TRUE);
		}

	private:
		void Init(void)
		{
			ZeroMemory(m_szIP, MAX_IP_LENGTH);

			m_RingRecvBuffer.Init();
			m_RingSendBuffer.Init();

			m_IsConnect = FALSE;
			m_IsClosed = FALSE;
			m_IsSendable = TRUE;

			m_SendIORefCount = 0;
			m_RecvIORefCount = 0;
			m_AcceptIORefCount = 0;
		}
		
		E_NET_RESULT BindAcceptExSocket(void)
		{
			ZeroMemory(&m_pRecvOverlappedEx->Overlapped, sizeof(OVERLAPPED));

			m_pRecvOverlappedEx->OverlappedExWsaBuf.buf = m_AddrBuf;
			m_pRecvOverlappedEx->pOverlappedExSocketMessage = m_pRecvOverlappedEx->OverlappedExWsaBuf.buf;
			m_pRecvOverlappedEx->OverlappedExWsaBuf.len = m_RecvBufSize;
			m_pRecvOverlappedEx->OverlappedExOperationType = OP_ACCEPT;
			m_pRecvOverlappedEx->ConnectionIndex = GetIndex();

			m_ClientSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_IP, NULL, 0, WSA_FLAG_OVERLAPPED);
			if (m_ClientSocket == INVALID_SOCKET)
			{
				return E_NET_RESULT::BindAcceptExSocket_fail_WSASocket;
			}

			IncrementAcceptIORefCount();

			DWORD acceptByte = 0;
			auto result = AcceptEx(
				m_ListenSocket,
				m_ClientSocket,
				m_pRecvOverlappedEx->OverlappedExWsaBuf.buf,
				0,
				sizeof(SOCKADDR_IN) + 16,
				sizeof(SOCKADDR_IN) + 16,
				&acceptByte,
				reinterpret_cast<LPOVERLAPPED>(m_pRecvOverlappedEx));

			if (!result && WSAGetLastError() != WSA_IO_PENDING)
			{
				DecrementAcceptIORefCount();

				return E_NET_RESULT::BindAcceptExSocket_fail_AcceptEx;
			}

			return E_NET_RESULT::Success;
		}


	private:
		int m_Index = INVALID_VALUE;
		//TODO ConnectionUnique 추가하자
				
		SOCKET m_ClientSocket = INVALID_SOCKET;
		SOCKET m_ListenSocket = INVALID_SOCKET;

		std::mutex m_MUTEX;

		OVERLAPPED_EX* m_pRecvOverlappedEx = nullptr;
		OVERLAPPED_EX* m_pSendOverlappedEx = nullptr;

		RingBuffer m_RingRecvBuffer;
		RingBuffer m_RingSendBuffer;

		char m_AddrBuf[MAX_ADDR_LENGTH] = { 0, };

		BOOL m_IsClosed = FALSE;
		BOOL m_IsConnect = FALSE;
		BOOL m_IsSendable = TRUE;

		int	m_RecvBufSize = INVALID_VALUE;
		int	m_SendBufSize = INVALID_VALUE;
				
		char m_szIP[MAX_IP_LENGTH] = { 0, };

		//TODO 아래 함수를 제거하여 IO 에러가 발생해서 접속을 짤라야하면 바로 짜를 수 있도록 하자
		DWORD m_SendIORefCount = 0; 
		DWORD m_RecvIORefCount = 0; 
		DWORD m_AcceptIORefCount = 0; 

		Message* m_pConnectionMsg = nullptr;
		Message* m_pCloseMsg = nullptr;
	};
}