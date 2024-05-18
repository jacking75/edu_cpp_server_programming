#pragma once

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "mswsock.lib")

#include <WinSock2.h>
#include <Mswsock.h>
#include <ws2tcpip.h>
#include <process.h>

#include "commonDef.h"
#include "spinLock.h"
#include "session.h"

namespace CQNetLib
{	
	class Session;
	
	class IocpServer
	{
	public:
#ifdef CQNET_USE_HEAD_ONLY
		IocpServer(void)
		{
		}
#else
		IocpServer(void);
#endif
		virtual ~IocpServer(void)
		{
			if (m_lpProcessPacket)
			{
				delete[] m_lpProcessPacket;
			}

			WSACleanup();
		}

		//연결 관련 관리 함수
		ERROR_CODE InitializeSocket()
		{
			for (UINT32 i = 0; i < m_dwWorkerThreadCount; i++) {
				m_hWorkerThread[i] = INVALID_HANDLE_VALUE;
			}

			for (INT32 i = 0; i < MAX_PROCESS_THREAD; i++) {
				m_hProcessThread[i] = INVALID_HANDLE_VALUE;
			}

			m_hWorkerIOCP = INVALID_HANDLE_VALUE;
			m_hProcessIOCP = INVALID_HANDLE_VALUE;
			m_ClientListenSock = INVALID_SOCKET;
			m_ServerListenSock = INVALID_SOCKET;

			WSADATA		WsaData;
			INT32 nRet = WSAStartup(MAKEWORD(2, 2), &WsaData);
			if (nRet)
			{
				return ERROR_CODE::WSAStartup_faile;
			}

			return ERROR_CODE::none;
		}

		virtual ERROR_CODE ServerStart(INITCONFIG &initConfig)
		{
			auto result = ERROR_CODE::none;
			m_usServerPort = (UINT16)initConfig.nServerPort;
			m_usClientPort = (UINT16)initConfig.nClientPort;
			m_dwWorkerThreadCount = initConfig.nWorkerThreadCnt;
			m_dwProcessThreadCount = initConfig.nProcessThreadCnt;

			if (result = InitializeSocket(); result != ERROR_CODE::none) {
				return result;
			}

			if (result = CreateWorkerIOCP(); result != ERROR_CODE::none) {
				return result;
			}

			if (result = CreateProcessIOCP(); result != ERROR_CODE::none) {
				return result;
			}

			if (result = CreateWorkerThreads(); result != ERROR_CODE::none) {
				return result;
			}

			if (result = CreateProcessThreads(); result != ERROR_CODE::none) {
				return result;
			}

			//서버연결을 위한 리슨소켓을 연다면.
			if (initConfig.nClientPort != 0)
			{
				if (result = CreateClinetListenSock(); result != ERROR_CODE::none)
				{
					return result;
				}

				initConfig.sockListener = GetClientListenSocket();
			}

			//서버연결을 위한 리슨소켓을 연다면.
			if (initConfig.nServerPort != 0)
			{
				if (result = CreateServerListenSock(); result != ERROR_CODE::none)
					return result;
			}

			if (m_lpProcessPacket) {
				delete[] m_lpProcessPacket;
			}

			m_lpProcessPacket = new PROCESSPACKET[initConfig.nProcessPacketCnt];
			m_dwMaxProcessPacketCnt = initConfig.nProcessPacketCnt;

			m_hServerOff = CreateEvent(NULL, true, false, NULL);

			m_ServerState = SERVER_START;
			return result;
		}

		virtual bool ServerOff()
		{
			LogFuncPtr((int)LogLevel::Info, "SYSTEM | cIocpServer::ServerOff() | 서버 종료를 시작합니다.");

			m_ServerState = SERVER_CLOSE;

			DestoryIOCP();
			
			StopAllThreads();
			
			DestoryAllListenSockets();

			if (m_hServerOff)
			{
				SetEvent(m_hServerOff);
				m_hServerOff = NULL;
			}

			LogFuncPtr((int)LogLevel::Info, "SYSTEM | cIocpServer::ServerOff() | 서버가 완전히 종료 되었습니다.");
			return true;

		}

		//패킷의 헤더를 분석
		virtual bool PacketTotalSize(const INT32 packetSizeLimit, const INT32 receivedSize, const char* pReceiveBuf, INT32& packetSize)
		{
			if (receivedSize >= PACKET_SIZE_LENGTH)
			{
				CopyMemory(&packetSize, pReceiveBuf, PACKET_SIZE_LENGTH);
			}
			else
			{
				packetSize = 0;
			}

			//arrive wrong packet..
			if (packetSize <= 0 || packetSize > packetSizeLimit)
			{
				return false;
			}

			return true;
		}

		/*--------------------------------------------------------------------------------------------*/
		//순수 가상 함수 : 이 함수들은 상속 받는 클래스에서 구현해야 한다.
		/*--------------------------------------------------------------------------------------------*/
		//client가 접속될때 호출되는 함수
		virtual	bool						OnAccept(Session* lpConnection) = 0;
		//client에서 packet이 도착하여 순서 성 있는 패킷을 처리할 때 호출되는 함수
		virtual	bool						OnRecv(Session* lpConnection, UINT32 dwSize, char* pRecvedMsg) = 0;
		//client에서 packet이 도착하여 순서 성 없는 패킷을 처리할 때 호출되는 함수
		virtual	bool						OnRecvImmediately(Session* lpConnection, UINT32 dwSize, char* pRecvedMsg) = 0;
		//client 접속 종료시 호출되는 함수
		virtual	void						OnClose(Session* lpConnection) = 0;
		//서버에서 ProcessThread가 아닌 다른 쓰레드에서 발생시킨 
		//메시지가 순서 성있게 처리되야 한다면 이 함수를 사용.
		virtual bool						OnSystemMsg(void* pOwnerKey, UINT32 dwMsgType, LPARAM lParam) = 0;
		

		void WorkerThread() // Network I/O를 처리하는 Thread
		{
			BOOL					bSuccess = false;
			LPOVERLAPPED			lpOverlapped = nullptr;
			Session* lpConnection = nullptr;
			DWORD					dwIoSize = 0;

			while (m_bWorkerThreadFlag)
			{
				dwIoSize = 0;
				lpOverlapped = nullptr;
				bSuccess = GetQueuedCompletionStatus(m_hWorkerIOCP,
					&dwIoSize,
					(PULONG_PTR)& lpConnection,
					&lpOverlapped,
					INFINITE);

				//GetQueuedCompletionStatus()함수 호출이 실패했을 경우.
				LPOVERLAPPED_EX lpOverlappedEx = (LPOVERLAPPED_EX)lpOverlapped;
				if (nullptr == lpOverlappedEx)
				{
					LogFuncPtr((int)LogLevel::Warning,"SYSTEM | cIocpServer::WorkerThread() | GetQueuedCompletionStatus() Failed : (%u) 작업 쓰레드 종료.",	GetLastError());
					continue;
				}
				//서버가 멈추었다면 더 이상 작업을 수행하지 않는다.
				if (SERVER_CLOSE == m_ServerState)
				{
					continue;
				}

				ProcessNetIOCompletion(bSuccess, dwIoSize, lpOverlappedEx, lpConnection);
			}
		}

		void ProcessThread() // 순서성 있는 패킷을 처리하는 Thread
		{
			BOOL					bSuccess = FALSE;
			LPPROCESSPACKET			lpProcessPacket = nullptr;
			Session* lpConnection = nullptr;
			void* pOwnerKey = nullptr;
			DWORD					dwIoSize = 0;

			while (m_bProcessThreadFlag)
			{

				dwIoSize = 0;
				bSuccess = GetQueuedCompletionStatus(m_hProcessIOCP,
					&dwIoSize,
					(PULONG_PTR)& pOwnerKey,
					(LPOVERLAPPED*)& lpProcessPacket,
					INFINITE);

				//쓰레드 종료
				if ((TRUE == bSuccess && NULL == pOwnerKey) || (NULL == lpProcessPacket))
				{
					LogFuncPtr((int)LogLevel::Warning, "SYSTEM | cIocpServer::ProcessThread() | GetQueuedCompletionStatus() Failed : (%u) 처리 쓰레드 종료",	GetLastError());
					continue;
				}

				//서버가 멈추었다면 더 이상 작업을 수행하지 않는다.
				if (SERVER_CLOSE == m_ServerState)
					continue;

				switch (lpProcessPacket->s_eOperationType)
				{
				case OP_CLOSE:
				{
					lpConnection = (Session*)pOwnerKey;
					lpConnection->CloseConnection(true);
					break;
				}
				case OP_RECVPACKET:
				{
					if (NULL == lpProcessPacket->s_lParam)
						continue;

					lpConnection = (Session*)pOwnerKey;
					OnRecv(lpConnection, dwIoSize, (char*)lpProcessPacket->s_lParam);
					lpConnection->GetRingBufferRecv().ReleaseBuffer(dwIoSize);
					break;
				}
				case OP_SYSTEM:
				{
					OnSystemMsg(pOwnerKey, (UINT32)lpProcessPacket->s_lParam, lpProcessPacket->s_wParam);
					break;
				}
				}

				ClearProcessPacket(lpProcessPacket);
			}
		}
		
		//client의 접속 끊기 처리, cConnection클래스 있는 것과 다른점은 이 함수는 자체는 순서성 없이 Close작업을 하며
		//인자로 받은 cConnetion을 순서성 있는 Close작업을 하도록 만든다.
		bool CloseConnection(Session* lpConnection, bool bIsWorkerThread/* = false*/)
		{
			UNREFERENCED_PARAMETER(bIsWorkerThread);

			//서버간 연결이 끊겼다면
			if (lpConnection->GetConnectionType() != CT_CLIENT)
			{
				LogFuncPtr((int)LogLevel::Error,"SYSTEM | cIocpServer::CloseConnection() | 서버간 연결이 끊겼다.ServerType(%d) ErrorCode(%d) socket(%d) WorkerThread(%d)",lpConnection->GetConnectionType(), GetLastError(), lpConnection->GetSocket(), bIsWorkerThread);
			}

			//레퍼런스 카운트가 남아있다면 소켓을 끊고 iocp에서 completion될때가지 기다려야한다.
			if (lpConnection->GetAcceptIoRefCount() != 0 ||
				lpConnection->GetRecvIoRefCount() != 0 ||
				lpConnection->GetSendIoRefCount() != 0)
			{
				//소켓 초기화
				shutdown(lpConnection->GetSocket(), SD_BOTH);
				closesocket(lpConnection->GetSocket());
				lpConnection->SetSocket(INVALID_SOCKET);
				return true;

			}

			BOOL& bIsClosing = lpConnection->GetIsClosing();

			if (InterlockedCompareExchange((LPLONG)& bIsClosing, TRUE, FALSE) == FALSE)
			{
				LPPROCESSPACKET lpProcessPacket = GetProcessPacket(OP_CLOSE, NULL, NULL);
				if (nullptr == lpProcessPacket)
				{
					return false;
				}

				if (0 == PostQueuedCompletionStatus(m_hProcessIOCP, 0, (ULONG_PTR)lpConnection, (LPOVERLAPPED)lpProcessPacket))
				{
					ClearProcessPacket(lpProcessPacket);
					
					LogFuncPtr((int)LogLevel::Error, "SYSTEM | cIocpServer::CloseConnection() | PostQueuedCompletionStatus Failed : [%d], socket[%d]", GetLastError(), lpConnection->GetSocket());
					
					lpConnection->CloseConnection(true);
				}
			}
			return true;
		}
		
		//recv한 패킷을 처리
		bool ProcessPacket(Session* lpConnection, char* pCurrent, UINT32 dwCurrentSize)
		{
			//만약 단일로 처리 하지 않아도 되는것이면 곧바로 처리한다.
			if (!OnRecvImmediately(lpConnection, dwCurrentSize, pCurrent))
			{
				LPPROCESSPACKET lpProcessPacket = GetProcessPacket(OP_RECVPACKET, (LPARAM)pCurrent, NULL);

				if (nullptr == lpProcessPacket)
				{
					CloseConnection(lpConnection, false);
					return false;
				}

				if (0 == PostQueuedCompletionStatus(m_hProcessIOCP,
					dwCurrentSize, (ULONG_PTR)lpConnection, (LPOVERLAPPED)lpProcessPacket))
				{
					ClearProcessPacket(lpProcessPacket);
					LogFuncPtr((int)LogLevel::Error,"SYSTEM | cIocpServer::ProcessPacket() | PostQueuedCompletionStatus Failed : [%d], socket[%d]", GetLastError(), lpConnection->GetSocket());
				}
			}			
			else //처리된 패킷은 링 버퍼에서 할당된 버퍼를 해제한다.
			{
				lpConnection->GetRingBufferRecv().ReleaseBuffer(dwCurrentSize);
			}

			return true;
		}
		
		
		//해당 작업 처리
		void DoAccept(LPOVERLAPPED_EX lpOverlappedEx) //Aceept처리를 한다.
		{
			SOCKADDR *lpLocalSockAddr = nullptr, *lpRemoteSockAddr = nullptr;
			INT32 nLocalSockaddrLen = 0, nRemoteSockaddrLen = 0;

			auto lpConnection = (Session*)lpOverlappedEx->s_lpConnection;
			if (lpConnection == nullptr) {
				return;
			}

			lpConnection->DecrementAcceptIoRefCount();

			//remote address를 알아낸다.
			GetAcceptExSockaddrs(lpConnection->GetAddressBuf(), 0, sizeof(SOCKADDR_IN) + 16,
				sizeof(SOCKADDR_IN) + 16, &lpLocalSockAddr, &nLocalSockaddrLen,
				&lpRemoteSockAddr, &nRemoteSockaddrLen);

			if (0 != nRemoteSockaddrLen)
			{
				char clientIP[65] = { 0, };
				inet_ntop(AF_INET, &((SOCKADDR_IN*)lpRemoteSockAddr)->sin_addr, clientIP, 65 - 1);
				lpConnection->SetConnectionIp(clientIP);
			}
			else
			{
				LogFuncPtr((int)LogLevel::Error,"SYSTEM | cIocpServer::WorkerThread() | GetAcceptExSockaddrs() Failed : (%u) socket(%d)",GetLastError(), lpConnection->GetSocket());
				CloseConnection(lpConnection, false);
				return;
			}

			//bind Completion key & connection context..
			if (lpConnection->BindIOCP(m_hWorkerIOCP) == false)
			{
				CloseConnection(lpConnection, false);
				return;
			}

			lpConnection->SetIsConnect(true);
			
			if (auto ret = lpConnection->RecvPost(lpConnection->GetRingBufferRecv().GetBeginMark(), 0); ret != ERROR_CODE::none)
			{
				CloseConnection(lpConnection, false);
				return;
			}

			OnAccept(lpConnection);
		}

		void DoRecv(LPOVERLAPPED_EX lpOverlappedEx, const UINT32 dwIoSize) //Recv처리를 한다.
		{
			auto lpConnection = (Session*)lpOverlappedEx->s_lpConnection;
			if (lpConnection == nullptr) {
				return;
			}

			lpConnection->DecrementRecvIoRefCount();

			INT32 nMsgSize = 0, nRemain = 0;
			char* pCurrent = nullptr, * pNext = nullptr;
			const auto packetSizeLimit = lpConnection->GetRingBufferRecv().GetBufferSize(); //TODO 패킷의 허용 최대 크기는 설정 정보에서 얻도록 한다

			nRemain = lpOverlappedEx->s_dwRemain;
			lpOverlappedEx->s_WsaBuf.buf = lpOverlappedEx->s_lpSocketMsg;
			lpOverlappedEx->s_dwRemain += dwIoSize;

			//TODO 관련 코드 테스트를 하면 삭제하기
			//if (lpOverlappedEx->s_dwRemain >= PACKET_SIZE_LENGTH)
			//	CopyMemory(&nMsgSize, &(lpOverlappedEx->s_WsaBuf.buf[0]), PACKET_SIZE_LENGTH);
			//else
			//	nMsgSize = 0;

			////arrive wrong packet..
			//if (nMsgSize <= 0 || nMsgSize > lpConnection->GetRingBufferRecv().GetBufferSize())
			//{
			//	//LOG(LOG_ERROR_NORMAL,"SYSTEM | cIocpServer::DoRecv() | arrived wrong packet : (%u) socket(%d)",GetLastError(), lpConnection->GetSocket());
			//	CloseConnection(lpConnection, false);
			//	return;
			//}
			if (PacketTotalSize(packetSizeLimit, lpOverlappedEx->s_dwRemain, &(lpOverlappedEx->s_WsaBuf.buf[0]), nMsgSize) == false)
			{
				LogFuncPtr((int)LogLevel::Error,"SYSTEM | cIocpServer::DoRecv() | arrived wrong packet : (%u) socket(%d)",GetLastError(), lpConnection->GetSocket());
				CloseConnection(lpConnection, false);
			}

			//lpOverlappedEx->s_nTotalBytes = nMsgSize;

			// not all message recved.
			if ((lpOverlappedEx->s_dwRemain < ((UINT32)nMsgSize)))
			{
				nRemain = lpOverlappedEx->s_dwRemain;
				pNext = lpOverlappedEx->s_WsaBuf.buf;

			}
			else	//하나 이상의 패킷의 데이터를 모두 받았다면
			{
				pCurrent = &(lpOverlappedEx->s_WsaBuf.buf[0]);
				INT32 	  dwCurrentSize = nMsgSize;

				nRemain = lpOverlappedEx->s_dwRemain;
				if (ProcessPacket(lpConnection, pCurrent, dwCurrentSize) == false)
				{
					CloseConnection(lpConnection, false);
					return;
				}

				nRemain -= dwCurrentSize;
				pNext = pCurrent + dwCurrentSize;

				bool bLoop = true;
				while (bLoop)
				{
					if (PacketTotalSize(packetSizeLimit, nRemain, pNext, nMsgSize) == false)
					{
						LogFuncPtr((int)LogLevel::Error, "SYSTEM | cIocpServer::DoRecv() | arrived wrong packet : (%u) socket(%d)",GetLastError(), lpConnection->GetSocket());
						CloseConnection(lpConnection, false);
						return;
					}
					
					if (nMsgSize > 0)
					{
						dwCurrentSize = nMsgSize;
												
						//lpOverlappedEx->s_nTotalBytes = dwCurrentSize;
						if (nRemain >= dwCurrentSize)
						{
							if (ProcessPacket(lpConnection, pNext, dwCurrentSize) == false)
							{
								CloseConnection(lpConnection, false);
								return;
							}

							nRemain -= dwCurrentSize;
							pNext += dwCurrentSize;
						}
						else
						{
							break;
						}
					}
					else
					{
						break;
					}
										
					//TODO 관련 코드 테스트를 하면 삭제하기
					//if (nRemain >= PACKET_SIZE_LENGTH)  
					//{

					//	CopyMemory(&nMsgSize, pNext, PACKET_SIZE_LENGTH);
					//	dwCurrentSize = nMsgSize;

					//	//arrive wrong packet..
					//	if (nMsgSize <= 0 || nMsgSize > lpConnection->GetRingBufferRecv().GetBufferSize())
					//	{
					//		//LOG(LOG_ERROR_NORMAL, "SYSTEM | cIocpServer::DoRecv() | arrived wrong packet2 : (%u) socket(%d), MsgSize(%d)",GetLastError(), lpConnection->GetSocket(), nMsgSize);

					//		CloseConnection(lpConnection, false);
					//		return;
					//	}
					//	lpOverlappedEx->s_nTotalBytes = dwCurrentSize;
					//	if (nRemain >= dwCurrentSize)
					//	{

					//		if (ProcessPacket(lpConnection, pNext, dwCurrentSize) == false)
					//		{
					//			CloseConnection(lpConnection, false);
					//			return;
					//		}
					//		nRemain -= dwCurrentSize;
					//		pNext += dwCurrentSize;
					//	}
					//	else
					//	{
					//		break;
					//	}
					//}
					//else
					//{
					//	break;
					//}
				}

			}

			lpConnection->RecvPost(pNext, nRemain);
		}

		void DoSend(LPOVERLAPPED_EX lpOverlappedEx, const UINT32 dwIoSize) //Send처리를 한다.
		{
			auto lpConnection = (Session*)lpOverlappedEx->s_lpConnection;
			if (lpConnection == NULL)
				return;

			lpConnection->DecrementSendIoRefCount();

			lpOverlappedEx->s_dwRemain += dwIoSize;
			// 만약 모든 메시지를 다 보내지 못했다면
			if ((UINT32)lpOverlappedEx->s_nTotalBytes > lpOverlappedEx->s_dwRemain)
			{
				UINT32 dwFlag = 0;
				DWORD dwSendNumBytes = 0;
				lpOverlappedEx->s_WsaBuf.buf += dwIoSize;
				lpOverlappedEx->s_WsaBuf.len -= dwIoSize;
				memset(&lpOverlappedEx->s_Overlapped, 0, sizeof(OVERLAPPED));
				lpConnection->IncrementSendIoRefCount();
				INT32 nRet = WSASend(lpConnection->GetSocket(),
					&(lpOverlappedEx->s_WsaBuf),
					1,
					&dwSendNumBytes,
					dwFlag,
					&(lpOverlappedEx->s_Overlapped),
					NULL);

				if (nRet == SOCKET_ERROR && (WSAGetLastError() != ERROR_IO_PENDING))
				{
					lpConnection->DecrementSendIoRefCount();
					LogFuncPtr((int)LogLevel::Error,"SYSTEM | cIocpServer::WorkerThread() | WSASend Failed : (%u) socket(%d)",GetLastError(), lpConnection->GetSocket());
					CloseConnection(lpConnection, false);
					return;
				}

			}
			else	//모든 메시지를 다 보냈다면
			{
				lpConnection->GetRingBufferSend().ReleaseBuffer(lpOverlappedEx->s_nTotalBytes);
				BOOL& bIsSending = lpConnection->GetIsSending();
				InterlockedExchange((LPLONG)& bIsSending, TRUE);
				//아직 보내지 못한 데이터를 보냄
				lpConnection->SendPost(0);
			}
		}

		inline SOCKET						GetClientListenSocket() { return m_ClientListenSock; }
		inline SOCKET						GetServerListenSocket() { return m_ServerListenSock; }
		inline unsigned short 				GetClientPort() { return m_usClientPort; }
		inline unsigned short 				GetServerPort() { return m_usServerPort; }
		inline char*						GetServerIp() { return m_szIp; }
		inline HANDLE						GetWorkerIOCP() { return m_hWorkerIOCP; }
		inline HANDLE						GetServerOffEvent() { return m_hServerOff; }

		//ProcessPacket처리 함수
		LPPROCESSPACKET GetProcessPacket(eOperationType operationType, LPARAM lParam, WPARAM wParam)
		{
			UINT32 dwProcessPacketIndex = 0;
			//do while로 도는 이유는 이 구문에 여러개의 쓰레드가 동시에 들어왔다면
			//m_dwMaxProcessPacketCnt를 넘을 수 있다 그럼 dwProcessPacketIndex가 m_dwMaxProcessPacketCnt일때는 reset을 해주지만 
			//m_dwMaxProcessPacketCnt을 넘는다면 다시 dwProcessPacketIndex를 구해야한다. sleep을 걸어 한 템포 늦춘다음에
			//다시 구해 오게한다.
			do
			{
				InterlockedIncrement((LPLONG)& m_dwCurrentProcessPacketCnt);

				dwProcessPacketIndex = InterlockedIncrement((LPLONG)& m_dwProcessPacketIndex);
				//현재 큐 Size를 초과하였다면 
				if (m_dwMaxProcessPacketCnt == dwProcessPacketIndex)
				{
					InterlockedExchange((LONG*)& m_dwProcessPacketIndex, 0);
					dwProcessPacketIndex = 0;
				}

			} while (dwProcessPacketIndex >= m_dwMaxProcessPacketCnt);

			LPPROCESSPACKET lpProcessPacket = &m_lpProcessPacket[dwProcessPacketIndex];
			if (OP_NONE != lpProcessPacket->s_eOperationType)
			{
				// 로그를 자세하게 남긴다. -__-. 또 일단 서버는 돌아가야 되니 패킷을 초기화 시킨다.
				LogFuncPtr((int)LogLevel::Error, "SYSTEM | cIocpServer::GetProcessPacket() | s_eOperationType(%d), Max(%d) Cur(%d) Idx(%d) System Msg Packet이 모두 찼다. 용량을 늘려야한다. 처리할 수 없다.", lpProcessPacket->s_eOperationType, m_dwMaxProcessPacketCnt, m_dwCurrentProcessPacketCnt, dwProcessPacketIndex);
								
				// 임시 방편으로 초기화 시킨다. -_-;;
				ClearProcessPacket(lpProcessPacket);
				return nullptr;
			}

			lpProcessPacket->s_eOperationType = operationType;
			lpProcessPacket->s_lParam = lParam;
			lpProcessPacket->s_wParam = wParam;
			return lpProcessPacket;
		}

		void ClearProcessPacket(LPPROCESSPACKET lpProcessPacket)
		{
			lpProcessPacket->Init();
			InterlockedDecrement((LPLONG)& m_dwCurrentProcessPacketCnt);
		}

		
		// 서버의 상태
		short		GetServerStatus() { return (short)m_ServerState; }

		// 서버는 시작하고 있나 ?
		bool		IsStart()
		{
			if (SERVER_START != m_ServerState) {
				return false;
			}

			return true;
		}


	private:
		static UINT32 CallWorkerThread(LPVOID p)
		{
			IocpServer* pServerSock = (IocpServer*)p;
			pServerSock->WorkerThread();
			return 1;
		}

		static UINT32 CallProcessThread(LPVOID p)
		{
			IocpServer* pServerSock = (IocpServer*)p;
			pServerSock->ProcessThread();
			return 1;
		}

		/*--------------------------------------------------------------------------------------------*/
		//내부 함수
		/*--------------------------------------------------------------------------------------------*/
		ERROR_CODE CreateProcessThreads()
		{
			HANDLE	hThread;
			UINT	uiThreadId;

			// create worker thread.
			for (UINT32 dwCount = 0; dwCount < m_dwProcessThreadCount; dwCount++)
			{
				hThread = (HANDLE)_beginthreadex(NULL, 0, &IocpServer::CallProcessThread, this, CREATE_SUSPENDED, &uiThreadId);
				if (hThread == nullptr)
				{
					return ERROR_CODE::create_ProcessThread_faile;
				}
				m_hProcessThread[dwCount] = hThread;
				ResumeThread(hThread);
				SetThreadPriority(hThread, THREAD_PRIORITY_TIME_CRITICAL);
			}
			return ERROR_CODE::none;
		}
		
		ERROR_CODE CreateWorkerThreads()
		{
			HANDLE	hThread;
			UINT	uiThreadId;

			for (UINT32 dwCount = 0; dwCount < m_dwWorkerThreadCount; dwCount++)
			{
				hThread = (HANDLE)_beginthreadex(NULL, 0, &IocpServer::CallWorkerThread,
					this, CREATE_SUSPENDED, &uiThreadId);

				if (hThread == nullptr)
				{
					return ERROR_CODE::create_WorkerThread_faile;
				}
				m_hWorkerThread[dwCount] = hThread;
				ResumeThread(hThread);
			}
			return ERROR_CODE::none;
		}
		
		void GetProperThreadsCount()
		{
			SYSTEM_INFO		SystemInfo;
			UINT32			ProperCount = 0;

			GetSystemInfo(&SystemInfo);
			ProperCount = SystemInfo.dwNumberOfProcessors * 2 + 1;

			if (ProperCount > MAX_WORKER_THREAD)
				ProperCount = (UINT32)MAX_WORKER_THREAD;

			m_dwWorkerThreadCount = ProperCount;
		}

		ERROR_CODE CreateWorkerIOCP()
		{
			//WorkerIOCP의 Concurrent Thread는 1개이상이어도 상관없다.
			m_hWorkerIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
			if (m_hWorkerIOCP == NULL)
			{
				return ERROR_CODE::create_WorkerIOCP_faile;
			}
			return ERROR_CODE::none;
		}
		
		ERROR_CODE CreateProcessIOCP()
		{
			//ProcessIOCP의 Concurrent Thread는 1개이여야 한다.
			m_hProcessIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 1);
			if (m_hProcessIOCP == NULL)
			{
				return ERROR_CODE::create_ProcessIOCP_faile;
			}
			return ERROR_CODE::none;
		}
		
		ERROR_CODE CreateClinetListenSock()
		{
			SOCKADDR_IN	si_addr;
			INT32 			nRet;

			// create listen socket.
			m_ClientListenSock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_IP, NULL, 0, WSA_FLAG_OVERLAPPED);
			if (INVALID_SOCKET == m_ClientListenSock)
			{
				return ERROR_CODE::create_listen_socket_fail;
			}

			INT32 rc, optval = 1;

			// ======================================================================
			// 바인딩되었던 소켓을 다시 사용할 수 있게 설정한다.
			// ======================================================================
			rc = setsockopt(m_ClientListenSock, SOL_SOCKET, SO_REUSEADDR, (char*)& optval, sizeof(optval));
			// ======================================================================
			if (SOCKET_ERROR == rc)
			{
				return ERROR_CODE::listen_socket_set_option_fail;
			}

			// bind listen socket with si_addr struct.
			si_addr.sin_family = AF_INET;
			si_addr.sin_port = htons(m_usClientPort);
			si_addr.sin_addr.s_addr = htonl(INADDR_ANY);

			nRet = bind(m_ClientListenSock, (struct sockaddr*) & si_addr, sizeof(si_addr));

			if (SOCKET_ERROR == nRet)
			{
				return ERROR_CODE::listen_socket_bind_fail;
			}


			// start listening..
			nRet = listen(m_ClientListenSock, 5);

			if (SOCKET_ERROR == nRet)
			{
				return ERROR_CODE::listen_fail;
			}

			HANDLE hIOCPHandle;
			hIOCPHandle = CreateIoCompletionPort((HANDLE)m_ClientListenSock, m_hWorkerIOCP, (UINT32)0, 0);

			if (nullptr == hIOCPHandle || m_hWorkerIOCP != hIOCPHandle)
			{
				return ERROR_CODE::listen_socket_bind_IOCP_fail;
			}

			return ERROR_CODE::none;
		}
		
		ERROR_CODE CreateServerListenSock()
		{
			m_ServerListenSock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_IP, NULL, 0, WSA_FLAG_OVERLAPPED);
			if (INVALID_SOCKET == m_ServerListenSock)
			{
				return ERROR_CODE::create_server_listen_socket_fail;
			}

			INT32 rc, optval = 1;

			// ======================================================================
			// 바인딩되었던 소켓을 다시 사용할 수 있게 설정한다.
			// ======================================================================
			rc = setsockopt(m_ServerListenSock, SOL_SOCKET, SO_REUSEADDR, (char*)& optval, sizeof(optval));
			// ======================================================================
			if (SOCKET_ERROR == rc)
			{
				return ERROR_CODE::server_listen_socket_set_option_fail;
			}

			// bind listen socket with si_addr struct.
			SOCKADDR_IN	si_addr;
			si_addr.sin_family = AF_INET;
			si_addr.sin_port = htons(m_usServerPort);
			si_addr.sin_addr.s_addr = htonl(INADDR_ANY);

			auto nRet = bind(m_ServerListenSock, (struct sockaddr*) & si_addr, sizeof(si_addr));

			if (SOCKET_ERROR == nRet)
			{
				return ERROR_CODE::server_listen_socket_bind_fail;
			}

			// server start listening..
			nRet = listen(m_ServerListenSock, 5);

			if (SOCKET_ERROR == nRet)
			{
				return ERROR_CODE::server_listen_fail;
			}


			HANDLE hIOCPHandle = CreateIoCompletionPort((HANDLE)m_ServerListenSock, m_hWorkerIOCP, (UINT32)0, 0);

			if (nullptr == hIOCPHandle || m_hWorkerIOCP != hIOCPHandle)
			{
				return ERROR_CODE::server_listen_socket_bind_IOCP_fail;
			}

			return ERROR_CODE::none;

		}

		void DestoryIOCP()
		{
			// 모든 쓰레드를 멈추고.. 완전히 종료 하기위해 뒷 처리를 해준다.
			if (m_hWorkerIOCP)
			{
				m_bWorkerThreadFlag = false;
				
				for (UINT32 i = 0; i < m_dwWorkerThreadCount; i++)
				{
					// WorkerThread에 종료 메시지를 보낸다.
					PostQueuedCompletionStatus(m_hWorkerIOCP, 0, 0, NULL);
				}
				
				CloseHandle(m_hWorkerIOCP);
				m_hWorkerIOCP = INVALID_HANDLE_VALUE;
			}

			if (m_hProcessIOCP)
			{
				m_bProcessThreadFlag = false;
				
				for (UINT32 i = 0; i < m_dwProcessThreadCount; i++)
				{
					// ProcessThread에 종료 메시지를 보낸다.
					PostQueuedCompletionStatus(m_hProcessIOCP, 0, 0, NULL);
				}
				
				CloseHandle(m_hProcessIOCP);
				m_hProcessIOCP = INVALID_HANDLE_VALUE;
			}
		}

		void StopAllThreads()
		{
			// 핸들을 닫는다.
			for (UINT32 i = 0; i < m_dwWorkerThreadCount; i++)
			{
				if (m_hWorkerThread[i] != INVALID_HANDLE_VALUE)
				{
					WaitForSingleObject(m_hWorkerThread[i], INFINITE);
					CloseHandle(m_hWorkerThread[i]);
				}
				m_hWorkerThread[i] = INVALID_HANDLE_VALUE;
			}

			// 핸들을 닫는다.
			for (UINT32 i = 0; i < m_dwProcessThreadCount; i++)
			{
				if (m_hProcessThread[i] != INVALID_HANDLE_VALUE)
				{
					WaitForSingleObject(m_hProcessThread[i], INFINITE);
					CloseHandle(m_hProcessThread[i]);
				}
				m_hProcessThread[i] = INVALID_HANDLE_VALUE;
			}
		}

		void DestoryAllListenSockets()
		{
			if (m_ClientListenSock != INVALID_SOCKET)
			{
				closesocket(m_ClientListenSock);
				m_ClientListenSock = INVALID_SOCKET;
			}

			if (m_ServerListenSock != INVALID_SOCKET)
			{
				closesocket(m_ServerListenSock);
				m_ServerListenSock = INVALID_SOCKET;
			}
		}

		void ProcessNetIOCompletion(const bool bSuccess, const DWORD dwIoSize, LPOVERLAPPED_EX lpOverlappedEx, Session* lpConnection)
		{
			//함수가 실패햇거나 client가 접속을 끊었을때..			
			if (!bSuccess || (dwIoSize == 0 && lpOverlappedEx->s_eOperation != OP_ACCEPT))
			{
				lpConnection = (Session*)lpOverlappedEx->s_lpConnection;
				if (lpConnection == nullptr)
				{
					return;
				}

				//Overlapped I/O요청 되어있던 작업의 카운트를 줄인다.
				if (lpOverlappedEx->s_eOperation == OP_ACCEPT)
					lpConnection->DecrementAcceptIoRefCount();
				else if (lpOverlappedEx->s_eOperation == OP_RECV)
					lpConnection->DecrementRecvIoRefCount();
				else if (lpOverlappedEx->s_eOperation == OP_SEND)
					lpConnection->DecrementSendIoRefCount();

				CloseConnection(lpConnection, true);
				return;
			}

			switch (lpOverlappedEx->s_eOperation)
			{
			case OP_ACCEPT:
				DoAccept(lpOverlappedEx);
				break;
			case OP_RECV:
				DoRecv(lpOverlappedEx, dwIoSize);
				break;
			case OP_SEND:
				DoSend(lpOverlappedEx, dwIoSize);
				break;
			}
		}

	protected:
		eServerNetState						m_ServerState = SERVER_INIT;								//서버 상태
		SOCKET								m_ClientListenSock = INVALID_SOCKET;							//클라이언트 접속위한 리슨 소켓
		SOCKET								m_ServerListenSock = INVALID_SOCKET;							//서버 접속을 위한 리슨 소켓

		HANDLE								m_hWorkerIOCP = INVALID_HANDLE_VALUE;								//네트워크 워커쓰레드를 위한 IOCP 핸들
		HANDLE								m_hProcessIOCP = INVALID_HANDLE_VALUE;						//프로세스 쓰레드를 위한 IOCP핸들
		HANDLE								m_hServerOff = INVALID_HANDLE_VALUE;								//서버가 Off되었는지 확인

		HANDLE								m_hWorkerThread[MAX_WORKER_THREAD];		//워커 쓰레드 핸들
		HANDLE								m_hProcessThread[MAX_PROCESS_THREAD];		//프로세스 쓰레드 핸들

		unsigned short						m_usClientPort = 0;			//클라이언트들의 접속을 받는 포트
		unsigned short						m_usServerPort = 0;			//서버간 통신을 하기위해 서버들의 접속받는 포트

		char								m_szIp[MAX_IP_LENGTH];						//IP

		UINT32								m_dwWorkerThreadCount = 0;						//워커 쓰레드 개수
		UINT32								m_dwProcessThreadCount = 0;						//프로세스 쓰레드 개수

		bool								m_bWorkerThreadFlag = true;						//워커 쓰레드 동작 플레그
		bool								m_bProcessThreadFlag = true;						//프로세스 쓰레드 동작 플레그

		LPPROCESSPACKET						m_lpProcessPacket = nullptr;							//시스템 메시지 처리를 위한 패킷
		UINT32								m_dwProcessPacketIndex = UINT32_MAX;						//시스템 메시지 처리 패킷의 현재 처리되고 있는 인덱스
		UINT32								m_dwMaxProcessPacketCnt = 0;					//시스템 메시지 처리 패킷의 최대 개수
		UINT32								m_dwCurrentProcessPacketCnt = 0;				//현재 사용중인 시스템 

	private:
		SpinLock	m_Lock;

	};


}