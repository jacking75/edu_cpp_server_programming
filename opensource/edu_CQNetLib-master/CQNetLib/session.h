#pragma once

#include <winsock2.h>
#include <Ws2tcpip.h>
#include <Mswsock.h>
#include <ctime>

#include "Delegate.h"
#include "commonDef.h"
#include "spinLock.h"
#include "queue.h"
#include "ringBuffer.h"
#include "vBuffer.h"


namespace CQNetLib
{
	class Session
	{
	public:
		Session(void)
		{			
			InitializeConnection();
		}

		virtual ~Session(void)
		{
			m_sockListener = INVALID_SOCKET;
			m_socket = INVALID_SOCKET;
		}

		void SetDelegate(const SA::delegate<void(Session*)>& onCloseFunc,
						const SA::delegate<bool(Session*, bool)>& closeConnectionFunc)
		{
			OnCloseDelegate = onCloseFunc;
			CloseConnectionDelegate = closeConnectionFunc;
		}

	public:
		/*--------------------------------------------------------------------------------------------*/
		//초기화 관련 함수
		/*--------------------------------------------------------------------------------------------*/
		void InitializeConnection()
		{
			ZeroMemory(m_szIp, MAX_IP_LENGTH);
			m_socket = INVALID_SOCKET;
			m_bIsConnect = FALSE;
			m_bIsClosed = FALSE;
			m_isSendIoOpNone = TRUE;
			m_dwSendIoRefCount = 0;
			m_dwRecvIoRefCount = 0;
			m_dwAcceptIoRefCount = 0;
			m_dwServerTick = 0;
			m_nPort = 0;
			m_bIsReconnect = false;

			m_ringRecvBuffer.Initialize();
			m_ringSendBuffer.Initialize();

			m_uiConnectStartTime = 0;
		}

		ERROR_CODE CreateConnection(const INITCONFIG& initConfig)
		{
			m_nIndex = initConfig.nIndex;
			m_sockListener = initConfig.sockListener;
			m_ConnectionType = initConfig.ConnectionType;
			strncpy_s(m_szIp, initConfig.szIP, MAX_IP_LENGTH);

			if (CT_CLIENT == initConfig.ConnectionType)
				m_nPort = initConfig.nClientPort;
			else
				m_nPort = initConfig.nServerPort;

			SAFE_DELETE(m_lpRecvOverlappedEx);
			SAFE_DELETE(m_lpSendOverlappedEx);

			m_lpRecvOverlappedEx = new OVERLAPPED_EX(this);
			m_lpSendOverlappedEx = new OVERLAPPED_EX(this);
			m_ringRecvBuffer.Create(initConfig.nRecvBufSize * initConfig.nRecvBufCnt);
			m_ringSendBuffer.Create(initConfig.nSendBufSize * initConfig.nSendBufCnt);

			m_nRecvBufSize = initConfig.nRecvBufSize;
			m_nSendBufSize = initConfig.nSendBufSize;

			m_InitConfig = initConfig;

			return BindAcceptExSock();
		}

		/*--------------------------------------------------------------------------------------------*/
		//연결 관련 관리 함수
		/*--------------------------------------------------------------------------------------------*/
		ERROR_CODE ConnectTo(const char* szIp, unsigned short usPort, HANDLE hIOCP)	//연결을 한다.
		{
			SOCKADDR_IN	si_addr;
			INT32 			nRet;
			
			// create listen socket.
			m_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_IP, NULL, 0, WSA_FLAG_OVERLAPPED);
			if (INVALID_SOCKET == m_socket)
			{
				return ERROR_CODE::session_connectTo_create_socket_fail;
			}

			// bind listen socket with si_addr struct.
			si_addr.sin_family = AF_INET;//si_addr.sin_port = htons(usPort); //si_addr.sin_addr.s_addr = inet_addr(szIp);
			auto ret = inet_pton(AF_INET, szIp, (void*)& si_addr.sin_addr.s_addr); UNREFERENCED_PARAMETER(ret);
			si_addr.sin_port = htons(usPort);

			nRet = WSAConnect(m_socket, (sockaddr*)& si_addr, sizeof(sockaddr), NULL, NULL, NULL, NULL);

			if (SOCKET_ERROR == nRet)
			{
				closesocket(m_socket);
				m_socket = INVALID_SOCKET;
				return ERROR_CODE::session_connectTo_remote_fail;
			}

			if (BindIOCP(hIOCP) == false)
			{
				closesocket(m_socket);
				m_socket = INVALID_SOCKET;
				return ERROR_CODE::session_connectTo_bind_iocp_fail;
			}

			m_bIsConnect = TRUE;

			//Recv버퍼를 Overlapped I/O로 연결한다.
			if (auto ret1 = RecvPost(m_ringRecvBuffer.GetBeginMark(), 0); ret1 != ERROR_CODE::none)
			{
				return ret1;
			}

			SetConnectionIp(szIp);
			SetConnectionPort(usPort);

			time_t CurTime;
			time(&CurTime);
			SetConnectStartTime((UINT32)CurTime);

			return ERROR_CODE::none;
		}

		bool CloseConnection(bool bForce = FALSE)	//연결을 종료한다.
		{
			//TODO 락 범위를 줄여본다
			SpinLockGuard lock(m_Lock);

			struct linger li = { 0, 0 };	// Default: SO_DONTLINGER
			if (bForce)
				li.l_onoff = 1; // SO_LINGER, timeout = 0

			if (TRUE == m_bIsConnect)
			{
				OnCloseDelegate(this);//m_pIocpServer->OnClose(this);
			}

			shutdown(m_socket, SD_BOTH);
			setsockopt(m_socket, SOL_SOCKET, SO_LINGER, (char*)& li, sizeof(li));
			closesocket(m_socket);

			m_socket = INVALID_SOCKET;

			if (m_lpRecvOverlappedEx != nullptr)
			{
				m_lpRecvOverlappedEx->s_dwRemain = 0;
				m_lpRecvOverlappedEx->s_nTotalBytes = 0;
			}

			if (m_lpSendOverlappedEx != nullptr)
			{
				m_lpSendOverlappedEx->s_dwRemain = 0;
				m_lpSendOverlappedEx->s_nTotalBytes = 0;
			}

			//connection을 다시 초기화 시켜준다.
			InitializeConnection();
			BindAcceptExSock();

			return true;
		}
		
		bool BindIOCP(HANDLE& hIOCP) //IOCP와 연결한다.
		{
			SpinLockGuard lock(m_Lock);

			auto hIOCPHandle = CreateIoCompletionPort((HANDLE)m_socket, hIOCP, reinterpret_cast<ULONG_PTR>(this), 0);

			if (nullptr == hIOCPHandle || hIOCP != hIOCPHandle)
			{
				return false;
			}

			m_hIOCP = hIOCP;
			return true;
		}
		
		ERROR_CODE RecvPost(char* pNext, UINT32 dwRemain) //리시브를 한다.
		{
			if (false == m_bIsConnect || nullptr == m_lpRecvOverlappedEx)
			{
				return ERROR_CODE::session_recvPost_not_connected;
			}

			m_lpRecvOverlappedEx->s_eOperation = OP_RECV;
			m_lpRecvOverlappedEx->s_dwRemain = dwRemain;
			auto nMoveMark = (INT32)(dwRemain - (m_ringRecvBuffer.GetCurrentMark() - pNext));
			m_lpRecvOverlappedEx->s_WsaBuf.len = m_nRecvBufSize;
			m_lpRecvOverlappedEx->s_WsaBuf.buf =
				m_ringRecvBuffer.ForwardMark(nMoveMark, m_nRecvBufSize, dwRemain);
			if (nullptr == m_lpRecvOverlappedEx->s_WsaBuf.buf)
			{
				CloseConnectionDelegate(this, false);
				return ERROR_CODE::session_recvPost_null_bufer;
			}

			m_lpRecvOverlappedEx->s_lpSocketMsg = m_lpRecvOverlappedEx->s_WsaBuf.buf - dwRemain;

			memset(&m_lpRecvOverlappedEx->s_Overlapped, 0, sizeof(OVERLAPPED));
			IncrementRecvIoRefCount();

			//사용한 양 체크
			m_ringRecvBuffer.SetUsedBufferSize(nMoveMark);

			DWORD				dwFlag = 0;
			DWORD				dwRecvNumBytes = 0;

			INT32 ret = WSARecv(
				m_socket,
				&m_lpRecvOverlappedEx->s_WsaBuf,
				1,
				&dwRecvNumBytes,
				&dwFlag,
				&m_lpRecvOverlappedEx->s_Overlapped,
				NULL);

			if (ret == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING)
			{
				DecrementRecvIoRefCount();
				CloseConnectionDelegate(this, false);
				return ERROR_CODE::session_recvPost_WSARecv_fail;
			}
			return ERROR_CODE::none;
		}
		
		ERROR_CODE SendPost(INT32 nSendSize)
		{
			//보내는 양이 있다면, 즉 IocpServer class의 DoSend()에서 불린게 아니라 PrepareSendPacket()함수를 부르고 
			//SendPost가 불렸다면 보내는 양이 있고 DoSend에서 불렸다면 0이 온다.
			if (nSendSize > 0)
				m_ringSendBuffer.SetUsedBufferSize(nSendSize);

			if (InterlockedCompareExchange((LPLONG)& m_isSendIoOpNone, FALSE, TRUE) == TRUE)
			{
				INT32 nReadSize;
				char* pBuf = m_ringSendBuffer.GetBuffer(m_nSendBufSize, nReadSize);
				if (nullptr == pBuf)
				{
					InterlockedExchange((LPLONG)& m_isSendIoOpNone, TRUE);
					return ERROR_CODE::session_sendPost_null_buffer;
				}

				m_lpSendOverlappedEx->s_dwRemain = 0;
				m_lpSendOverlappedEx->s_eOperation = OP_SEND;
				m_lpSendOverlappedEx->s_nTotalBytes = nReadSize;
				ZeroMemory(&m_lpSendOverlappedEx->s_Overlapped, sizeof(OVERLAPPED));
				m_lpSendOverlappedEx->s_WsaBuf.len = nReadSize;
				m_lpSendOverlappedEx->s_WsaBuf.buf = pBuf;
				m_lpSendOverlappedEx->s_lpConnection = this;

				IncrementSendIoRefCount();

				DWORD dwBytes = 0;
				INT32 ret = WSASend(
					m_socket,
					&m_lpSendOverlappedEx->s_WsaBuf,
					1,
					&dwBytes,
					0,
					&m_lpSendOverlappedEx->s_Overlapped,
					NULL);
				if (ret == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING)
				{
					DecrementSendIoRefCount();
					CloseConnectionDelegate(this, false); 
					InterlockedExchange((LPLONG)& m_isSendIoOpNone, TRUE);
					return ERROR_CODE::session_sendPost_WSASend_fail;
				}
			}
			return ERROR_CODE::none;
		}
		
		ERROR_CODE BindAcceptExSock()	//AcceptEx에 socket을 연결한다.
		{
			//리슨 소켓이 없다면 acceptex에 bind하지 않는다.
			if (INVALID_SOCKET == m_sockListener)
			{
				return ERROR_CODE::none;
			}

			DWORD dwBytes = 0;
			memset(&m_lpRecvOverlappedEx->s_Overlapped, 0, sizeof(OVERLAPPED));
			m_lpRecvOverlappedEx->s_WsaBuf.buf = m_szAddressBuf;
			m_lpRecvOverlappedEx->s_lpSocketMsg = &m_lpRecvOverlappedEx->s_WsaBuf.buf[0];
			m_lpRecvOverlappedEx->s_WsaBuf.len = m_nRecvBufSize;
			m_lpRecvOverlappedEx->s_eOperation = OP_ACCEPT;
			m_lpRecvOverlappedEx->s_lpConnection = this;
			m_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_IP,
				NULL, 0, WSA_FLAG_OVERLAPPED);
			if (INVALID_SOCKET == m_socket)
			{
				return ERROR_CODE::session_create_socket_fail;
			}

			IncrementAcceptIoRefCount();

			BOOL bRet = AcceptEx(m_sockListener, m_socket,
				m_lpRecvOverlappedEx->s_WsaBuf.buf,
				0,
				sizeof(SOCKADDR_IN) + 16,
				sizeof(SOCKADDR_IN) + 16,
				&dwBytes,
				(LPOVERLAPPED)m_lpRecvOverlappedEx
			);
			if (!bRet && WSAGetLastError() != WSA_IO_PENDING)
			{
				DecrementAcceptIoRefCount();
				return ERROR_CODE::session_post_accept_socket_fail;
			}

			return ERROR_CODE::none;
		}
		
		void SetConnectStartTime(UINT32 uiTime) { m_uiConnectStartTime = uiTime; }
		
		UINT32 GetConnectStartTime() { return m_uiConnectStartTime; }

		inline void							SetIsConnect(bool bIsConnect) { m_bIsConnect = bIsConnect; }
		inline bool							GetIsConnect() const { return m_bIsConnect; }
		inline void							SetIsReconnect(bool bIsReconnect) { m_bIsReconnect = bIsReconnect; }
		inline bool							GetIsReconnect() const { return m_bIsReconnect; }
		inline eConnectionType				GetConnectionType() const { return m_ConnectionType; }
		inline void							SetConnectionType(eConnectionType ConnectionType) { m_ConnectionType = ConnectionType; }

		//Overlapped i/o작업 요청 횟수
		inline INT32  						GetRecvIoRefCount() const { return m_dwRecvIoRefCount; }
		inline INT32  						GetSendIoRefCount() const { return m_dwSendIoRefCount; }
		inline INT32  						GetAcceptIoRefCount() const { return m_dwAcceptIoRefCount; }
		inline INT32 							IncrementRecvIoRefCount() { return InterlockedIncrement((LPLONG)&m_dwRecvIoRefCount); }
		inline INT32 							IncrementSendIoRefCount() { return InterlockedIncrement((LPLONG)&m_dwSendIoRefCount); }
		inline INT32  						IncrementAcceptIoRefCount() { return InterlockedIncrement((LPLONG)&m_dwAcceptIoRefCount); }
		inline INT32  						DecrementRecvIoRefCount() { return (m_dwRecvIoRefCount ? InterlockedDecrement((LPLONG)&m_dwRecvIoRefCount) : 0); }
		inline INT32  						DecrementSendIoRefCount() { return (m_dwSendIoRefCount ? InterlockedDecrement((LPLONG)&m_dwSendIoRefCount) : 0); }
		inline INT32  						DecrementAcceptIoRefCount() { return (m_dwAcceptIoRefCount ? InterlockedDecrement((LPLONG)&m_dwAcceptIoRefCount) : 0); }

		/*--------------------------------------------------------------------------------------------*/
		//버퍼 관련 관리 함수
		//보낼 패킷 버퍼를 준비한다. 만약 중간에 취소된다면 ReleaseSendPacket호출
		/*--------------------------------------------------------------------------------------------*/
		char* PrepareSendPacket(INT32 nLen) 
		{
			if (false == m_bIsConnect)
			{
				return nullptr; 
			}

			char* pBuf = m_ringSendBuffer.ForwardMark(nLen);
			if (nullptr == pBuf)
			{
				CloseConnectionDelegate(this, false);
				return nullptr;
			}

			ZeroMemory(pBuf, nLen);
			CopyMemory(pBuf, &nLen, PACKET_SIZE_LENGTH);
			return pBuf;
		}

		bool ReleaseSendPacket(const char* pReleasePacket) //PrepareSendPacket하였던 버퍼를 원래대로 돌려놓는다.
		{
			//PrepareSendPacket을 하여 얻었던 버퍼를 원래대로 돌려놓는다.
			m_ringSendBuffer.BackwardMark(pReleasePacket);
			return true;
		}

		inline INT32  						GetRecvBufSize() const { return m_nRecvBufSize; }		//Recv 버퍼 사이즈를 돌려준다.
		inline INT32  						GetSendBufSize() const { return m_nSendBufSize; }		//Send 버퍼 사이즈를 돌려준다.
		inline RingBuffer &				GetRingBufferRecv() { return m_ringRecvBuffer; }
		inline RingBuffer &				GetRingBufferSend() { return m_ringSendBuffer; }

		inline BOOL &						GetIsSending() { return m_isSendIoOpNone; }
		inline BOOL &						GetIsClosing() { return m_bIsClosed; }

		bool SendVBuffer() //g_pVBuffer있는 가변 정보를 보낸다.
		{
			return SendBuffer(g_pVBuffer->GetBeginMark(), g_pVBuffer->GetCurBufSize());
		}

		bool SendBuffer(const char* dataPacket, INT32 sizePacket) //인자로 받은 버퍼를 그대로 복사하여 보낸다.
		{
			char* pSendBuffer = PrepareSendPacket(sizePacket);
			if (nullptr == pSendBuffer)
			{
				unsigned short usType;
				CopyMemory(&usType, dataPacket + PACKET_SIZE_LENGTH, PACKET_TYPE_LENGTH);
				return false;
			}

			INT32 nPacketLen = sizePacket;
			CopyMemory(pSendBuffer, &nPacketLen, PACKET_SIZE_LENGTH);
			CopyMemory(pSendBuffer + PACKET_SIZE_LENGTH, dataPacket + PACKET_SIZE_LENGTH, sizePacket - PACKET_SIZE_LENGTH);
			SendPost(nPacketLen);
			return true;
		}

		/*--------------------------------------------------------------------------------------------*/
		//기타 속성 관련 함수
		/*--------------------------------------------------------------------------------------------*/
		inline INT32 							GetIndex() const { return m_nIndex; }
		void 								SetSocket(SOCKET socket) { m_socket = socket; }
		SOCKET 								GetSocket() const { return m_socket; }
		inline void  						SetConnectionIp(const char* Ip) { strncpy_s(m_szIp, Ip, MAX_IP_LENGTH); }
		inline char* 						GetConnectionIp() { return m_szIp; }
		inline void	 						SetConnectionPort(INT32 nPort) { m_nPort = nPort; }
		inline INT32   						GetConnectionPort() const { return m_nPort; }
		inline void							SetServerTick(UINT32 dwServerTick) { m_dwServerTick = dwServerTick; }
		inline UINT32						GetServerTick() const { return m_dwServerTick; }
		inline void							SetInitConfig(const INITCONFIG & initConfig) { m_InitConfig = initConfig; }
		inline const INITCONFIG &			GetInitConfig() const { return m_InitConfig; }
		inline char*						GetAddressBuf() { return m_szAddressBuf; }


		virtual Session*				Clone() { return new Session; }

		//static inline void					SetIocpServer(IocpServer* pIocpServer) { m_pIocpServer = pIocpServer; }

	protected:

		/*--------------------------------------------------------------------------------------------*/
		//버퍼 관련 변수
		/*--------------------------------------------------------------------------------------------*/
		LPOVERLAPPED_EX						m_lpRecvOverlappedEx = nullptr;
		LPOVERLAPPED_EX						m_lpSendOverlappedEx = nullptr;

		//클라이언트와 데이터 송수신을 위한 링 버퍼
		RingBuffer							m_ringRecvBuffer;					//연결의 RECV에 할당된 버퍼
		RingBuffer							m_ringSendBuffer;					//연결의 SEND에 할당된 버퍼

		INT32 									m_nRecvBufSize = 0;						//한번에 수신할 수 있는 데이터의 최대 크기
		INT32 									m_nSendBufSize = 0;						//한번에 송신할 수 있는 데이터의 최대 크기	

		UINT32								m_dwSendIoRefCount;					//Overlapped i/o작업을 요청한 개수
		UINT32								m_dwRecvIoRefCount;
		UINT32								m_dwAcceptIoRefCount;

		/*--------------------------------------------------------------------------------------------*/
		//연결 관련 변수
		/*--------------------------------------------------------------------------------------------*/
		char								m_szAddressBuf[1024];				//클라이언트 주소를 받기위한 버퍼
		BOOL								m_bIsClosed = FALSE;						//클라이언트와 연결 종료가 되었는지 여부
		BOOL								m_isSendIoOpNone = true;							//현재 Overlapped I/O 전송 작업을 하고 있는지 여부
		SOCKET								m_socket = INVALID_SOCKET;							//연결 소켓
		SOCKET								m_sockListener = INVALID_SOCKET;						//리슨 소켓
		HANDLE								m_hIOCP = INVALID_HANDLE_VALUE;							//IOCP 핸들
		bool								m_bIsConnect = false;						//클라이언트와 연결이 되어있는지 여부
		bool								m_bIsReconnect = false;						//재 연결을 요청할때 필요한 변수. 서버간 연결에서 필요
		eConnectionType						m_ConnectionType = CT_CLIENT;		//연결의 종류( 클라이언트와 연결인지, db서버와 연결인지??등)

		/*--------------------------------------------------------------------------------------------*/
		//속성 관련 변수
		/*--------------------------------------------------------------------------------------------*/
		INT32 									m_nIndex;							//연결 인덱스, 특별한 의미는 없다.
		char								m_szIp[MAX_IP_LENGTH];
		INT32 									m_nPort;
		UINT32								m_dwServerTick;						//서버 틱
		INITCONFIG							m_InitConfig;						//서버 설정 정보를 가지고 있는다.
		
		//static IocpServer*					m_pIocpServer;						//IocpServer객체


	private:
		SpinLock m_Lock;
		UINT32						m_uiConnectStartTime;				// 연결 시작 시간.(초 단위)
		
		SA::delegate<void(Session*)> OnCloseDelegate;
		SA::delegate<bool(Session*, bool)> CloseConnectionDelegate;
		
		SET_NO_COPYING(Session);
	};

		
}