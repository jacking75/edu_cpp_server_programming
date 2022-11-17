//#include <Nave/Nave.h>
//#include <Nave/NFException.h>
//#include <Nave/NFLog.h>
#include "NaveNet.h"
#include "NFNetClient.h"


namespace NaveClientNetLib {

	NFNetClient::NFNetClient() : m_hSocket(NULL), m_nPort(0), m_hRecvThread(NULL), 
								m_hEventThread(NULL), m_hEvent(NULL), m_bConnect(FALSE), m_bClose(FALSE)
	{
		m_SendQueue.Create(32);
		m_RecvQueue.Create();
		m_RecvIO.NewIOBuf(0);			// 0으로 하면 기본적으로 DefaultPacketSize * 1024

		ZeroMemory(m_strIPAddr, sizeof(m_strIPAddr));	// Server IP저장 

		WinSockInit();
	}

	NFNetClient::~NFNetClient()
	{
		// 다시 커넷을 종료
		OnClose();
		Stop();							// 종료 함수 호출 

		// 버퍼삭제
		m_SendQueue.Release();
		m_RecvQueue.Release();
		m_RecvIO.DeleteIOBuf();

		WSACleanup();
	}

	BOOL NFNetClient::WinSockInit()
	{
		WORD wVersionRequested;
		WSADATA wsaData;
		int err;

		wVersionRequested = MAKEWORD( 2, 2 );
	 
		err = WSAStartup( wVersionRequested, &wsaData );
		if ( err != 0 ) {
			WSACleanup();
			// Tell the user that we could not find a usable 
			// WinSock DLL.                                  
			return FALSE;
		}
	 
		if ( LOBYTE( wsaData.wVersion ) != 2 ||
				HIBYTE( wsaData.wVersion ) != 2 ) {
			// Tell the user that we could not find a usable //
			// WinSock DLL.   //
			WSACleanup( );
			return FALSE; 
		}
		return TRUE;
	}

	BOOL NFNetClient::Init( LPCSTR szIP, int nPort )
	{	
		m_nPort = nPort;											// 포트 번호 
		unsigned long dwThreadId = 0;								// 스레드 생성을 위한 변수 

		// ip 어드레스 
		IN_ADDR LoginAddress;
		if (GetHostIPByDomain(LoginAddress, szIP))    
		{ 
			strncpy(m_strIPAddr, inet_ntoa(LoginAddress), MAX_PATH); 
		}

		// 연결을 시킨다. 서버로.. 만약 서버연결에 실패한다면
		// Netword Event 에서 FW_CLOSE가 발생해 소켓이 Close된다.
		if(!Connect())
		{
			// 실패했을경우 종료한다.
			Sleep(100);													// Sleep...
			OnClose();	

			//LOG_ERROR((L"ClientCtrl Init Faild."));
			return FALSE;
		}

		m_bClose = FALSE;

		m_hRecvThread =
				(HANDLE)CreateThread(NULL,							// Security
					0,												// Stack size - use default
					SocketThreadProc,     							// Thread fn entry point
					(void*) this,	      
					0,												// Init flag
					&dwThreadId);									// Thread address

		m_hEventThread =
				(HANDLE)CreateThread(NULL,							// Security
					0,												// Stack size - use default
					EventThreadProc,     							// Thread fn entry point
					(void*) this,	      
					0,												// Init flag
					&dwThreadId);									// Thread address

		return TRUE;
	}

	void NFNetClient::Disconnect()
	{
		// 이건 무조건 Close해줘야하기 때문에  CloseAll() 호출하지 않고 직접 끈다.
		OnClose();
		Stop();								// 종료 함수 호출 
	}

	void NFNetClient::Stop()
	{
		if (m_hSocket)
		{
			struct linger li = {0, 0};	// Default: SO_DONTLINGER

			li.l_onoff = 1; // SO_LINGER, timeout = 0
			shutdown(m_hSocket, SD_BOTH );						// 오잉? 이게 뭐지? ^^;; 담에 찾아보자 
																// 2001년 9월 6일 

			// 클로즈 소켓 전에 큐된 데이타를 보낼지 말지 결정하는 옵션
			setsockopt(m_hSocket, SOL_SOCKET, SO_LINGER, (CHAR *)&li, sizeof(li));
			closesocket(m_hSocket);								// 소켓 닫기 
			m_hSocket = NULL;
		}

		if(m_hRecvThread) CloseHandle(m_hRecvThread);
		m_hRecvThread = NULL;

		if(m_hEventThread) CloseHandle(m_hEventThread);
		m_hEventThread = NULL;

		if(m_hEvent) WSACloseEvent(m_hEvent);
		m_hEvent = NULL;
	}

	void NFNetClient::CloseAll()
	{
		if(m_bConnect)
		{
			OnClose();
			Stop();								// 종료 함수 호출 
		}
	}

	void NFNetClient::UpdateQue()
	{
		while(GetQueCnt() != 0)
		{
			// 패킷이 전달됐는지 검사.
			NFPacket* pPacket = NULL;
			int Cnt = GetPacket(&pPacket);

			// 사이즈가 0보다 작거나 Packet 가 NULL이면.
			if(Cnt <= 0 || pPacket == NULL)
			{
				//LOG_ERROR((L"NFNetClient::UpdateQue() Packet Error"));
				assert( "NFNetClient::UpdateQue() Packet Error.");
			}

			// 함수 호출
			ProcessPacket(pPacket, Cnt);

			// 패킷 제거
			PopPacket();
		}
	}

	void NFNetClient::Update()
	{
		UpdateQue();
	}

	BOOL NFNetClient::OnClose()
	{
		m_bClose = TRUE;
		m_bConnect = FALSE;											// Connect 변수 변경 

		// 실제 파일 
		OnSocketEvent(WM_SOCK_CLOSE, 0);

		return FALSE;
	}

	BOOL NFNetClient::Connect()
	{	
		// 연결중이라면 
		if(m_bConnect) return TRUE;

		// 소켓이 남아 있다면 
		Stop();

		m_RecvQueue.Reset();
		m_SendQueue.Reset();
		m_RecvIO.InitIOBuf();

		// 소켓 생성 
		m_hSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		// 소켓 생성 검사 
		if (m_hSocket == INVALID_SOCKET)
		{
			OnSocketEvent(WM_CONNECT, FALSE);
			return FALSE;
		}

		// 네트워크 이벤트 핸들 생성 
		m_hEvent = WSACreateEvent();
		if (m_hEvent == WSA_INVALID_EVENT)
		{
			Stop();
			return FALSE;
		}

		// Request async notification
		int nRet = WSAEventSelect(m_hSocket,
								m_hEvent,
								FD_CLOSE | FD_CONNECT);	// 신호를 선별하여 받게 한다 
		
		// 에러라면 
		if (nRet == SOCKET_ERROR)
		{
			Stop();
			return FALSE;
		}

		// 비동기 방식
		unsigned long ul = 1;
		nRet = ioctlsocket(m_hSocket, FIONBIO, (unsigned long*)&ul);
		// 소켓 생성 검사 
		if (m_hSocket == SOCKET_ERROR)
		{
			OnSocketEvent(WM_CONNECT, FALSE);
			return FALSE;
		}

		/////////////////////////////////
		// 소켓의 성능 최적화를 위한 세팅 
		int zero = 0;
		int err = 0;

		// Send Buffer에 대한 세팅
		if( (err = setsockopt( m_hSocket, SOL_SOCKET, SO_SNDBUF, (const char*)&zero, sizeof(zero))) == SOCKET_ERROR)
		{
			closesocket(m_hSocket);
			m_hSocket = NULL;
			return FALSE;
		}

		// Receive Buffer에 대한 세팅 
		if((err = setsockopt( m_hSocket, SOL_SOCKET, SO_RCVBUF, (const char*)&zero, sizeof(zero) )) == SOCKET_ERROR)
		{
			closesocket(m_hSocket);
			m_hSocket = NULL;
			return FALSE;
		}

		SOCKADDR_IN		saServer;		

		memset(&saServer,0,sizeof(saServer));

		saServer.sin_family = AF_INET;
		saServer.sin_addr.s_addr = inet_addr(m_strIPAddr);
		saServer.sin_port = htons(m_nPort);

		// 서버와 Connect
		nRet = connect(m_hSocket,(sockaddr*)&saServer, sizeof(saServer));
		// 소켓 에러이거나 블럭킹이 되었다면 
		if (nRet == SOCKET_ERROR &&	WSAGetLastError() != WSAEWOULDBLOCK)
		{
			Stop();
			return FALSE;
		}
	/*
		이렇게 Connect를 하면 서버는 AccetpEx가 성공하여 ClientSceesion은 
		Recv대기상태로 들어간다. 
		소켓 이벤트는 Connect가 성공하면 FD_CONNECT를 발생시킨다.
	*/
		return TRUE;
	}

	DWORD WINAPI NFNetClient::SocketThreadProc(LPVOID lParam)
	{
		// 클래스를 변수로 받음
		NFNetClient* pThis = reinterpret_cast<NFNetClient*>(lParam);

		BOOL bThreadRun = TRUE;			// 무한 루프 변수 

		// 스레드 무한 루프 
		while(bThreadRun)
		{
			if(pThis->m_bClose)
			{
				bThreadRun = FALSE;
				break;
			}

	 		if(!pThis->m_hSocket || !pThis->m_bConnect)
				continue;

			pThis->OnSendPacketData();	
			pThis->OnReadPacketData();	
			Sleep(1);
		}
		return 0;
	}

	DWORD WINAPI NFNetClient::EventThreadProc(LPVOID lParam)
	{
		// 클래스를 변수로 받음
		NFNetClient* pThis = reinterpret_cast<NFNetClient*>(lParam);

		WSANETWORKEVENTS events;		// 네트워크 이벤트 변수 
		BOOL bThreadRun = TRUE;			// 무한 루프 변수 

		// 스레드 무한 루프 
		while(bThreadRun)
		{
			if(pThis->m_bClose)
			{
				bThreadRun = FALSE;
				break;
			}

			DWORD dwRet;
			dwRet = WSAWaitForMultipleEvents(1,
										&pThis->m_hEvent,
										FALSE,
										INFINITE,
										FALSE);
	 
	 		if(!pThis->m_hSocket)
			{	
				// 종료
				bThreadRun = FALSE;
				break;
			}
			// Figure out what happened
			int nRet = WSAEnumNetworkEvents(pThis->m_hSocket,
									pThis->m_hEvent,
									&events);
			
			// 소켓 에러라면,
			if (nRet == SOCKET_ERROR)	
			{
				//LOG_ERROR((L"EventThread : SOCKET_ERROR, %d", WSAGetLastError()) );
				bThreadRun = FALSE;
				break;
			}

			///////////////////
			// Handle events //
			bThreadRun = pThis->NetworkEventHanlder(events.lNetworkEvents);
		}
		// 이리로 스레드가 종료 되면 Server에 의한 클라이언트 종료!!! <비정상 종료>
		// 스레드 초기화는 위에서 해주기 때문에 여기서 하지는 않는다.
//		pThis->CloseAll();
		return 0;
	}

	VOID NFNetClient::OnSendPacketData()
	{
		//////////////////////////////////////////////////////////////////////////////
		// Send
		int rc = 0;
		int idx = 0,size = 0;
		CHAR* send_temp=NULL;
		NFPacket Packet;

		if((size = m_SendQueue.Get(Packet)) != -1)
		{
			// 여기서 암호화 까지 같이 진행 ??
			// 보내기 직전 체크섬을 생성.
			size = Packet.EncryptPacket();

			send_temp = (CHAR*)&Packet;
			while(size > 0)
			{
				// 10004 : WSACancelBlockingCall를 호출하여 차단 작업이 중단되었습니다. 
				// 10058 : 해당 소켓이 종료되었으므로 데이터 보내거나 받을 수 없습니다. 
				// 10038 : 연결이 끊어진 소켓을 사용할려고 할때 난다.
				if((rc = send(m_hSocket, &send_temp[idx], size, 0)) == SOCKET_ERROR)
				{
					// 블럭킹 에러라면 
					if (GetLastError() != WSAEWOULDBLOCK) // 블럭킹 에러가 아니라면 
					{				
						//LOG_ERROR((L"SendThread : SOCKET_ERROR, %d", WSAGetLastError()));
						break;
					}
				}
				else
				{
					// 에러가 없다면 
					size -= rc;
					idx += rc;
				}
			}
			idx = 0;
			size = 0;
		}
		//////////////////////////////////////////////////////////////////////////////
	}

	VOID NFNetClient::OnReadPacketData()
	{
		//////////////////////////////////////////////////////////////////////////////
		// Recv
		int Ret = recv(m_hSocket, m_RecvBuffer, DEF_MAXPACKETSIZE, 0);	// 데이타 Receive
		if(Ret == 0)        // Graceful close
		{
			CloseAll();
			return;
		}
		else if (Ret == SOCKET_ERROR &&	WSAGetLastError() != WSAEWOULDBLOCK ) // 블럭킹 에러가 아니라면 
		{
			//LOG_ERROR((L"[Recv] Packet Size Error : SOCKET_ERROR, %d", WSAGetLastError()));
			CloseAll();
			return;							
		}

		if(m_RecvIO.Append(m_RecvBuffer, Ret) == -1)
		{
			//LOG_ERROR((L"m_RecvIO.Append : Buffer Overflow..") );
		}

		// 받은 패킷은 IOBuffer에 넣어 처리한다.
		if(m_RecvIO.GetPacket(&m_RecvPacket) == 1)
		{
			m_RecvPacket.DecryptPacket();

			if(m_RecvPacket.IsAliveChecksum())
			{
				// 여기서 한패킷 처리 루틴 호출
				m_RecvQueue.Add(m_RecvPacket);

				// Message Type 일때 이걸로 보낸다. 
				// 만약 Update 이벤트 호출이면 이 루틴을 주석처리 해준다.
				OnSocketEvent(WM_RECV_MSG, m_RecvPacket.m_Header.Size);
			}

			m_RecvPacket.Init();
		}
	}

	BOOL NFNetClient::NetworkEventHanlder(LONG lEvent)
	{
		BOOL bFlag = TRUE;

//		if(lEvent & FD_READ)
//		{
//			bFlag = OnReadPacketData();	
//		}
		if(lEvent & FD_CLOSE)
		{
			bFlag = FALSE;
		}
		if(lEvent & FD_CONNECT)
		{
			bFlag = OnConnect();
		}

		return bFlag;
	}

	BOOL NFNetClient::OnConnect()
	{
		//////////////////////////////////////////////////////////
		// IOCP 활성 
		//////////////////////////////////////////////////////////
		int ErrorCode = send(m_hSocket, (const char*)CONNECT_CHECK_DATA,CONNECT_CHECK_SIZE,0);
		if(ErrorCode == SOCKET_ERROR)
		{
			m_bConnect = FALSE;						// 연결 변수 설정 OFF
		}
		else
		{
			m_bConnect = TRUE;						// 연결 변수 설정 ON
		}
		OnSocketEvent(WM_CONNECT, m_bConnect);

		return m_bConnect;
	}

	int NFNetClient::GetQueCnt()
	{
		return m_RecvQueue.GetQueCnt();
	}

	int NFNetClient::GetPacket(NFPacket** Packet)
	{
		return m_RecvQueue.GetPnt(Packet);
	}

	void NFNetClient::PopPacket()
	{
		m_RecvQueue.Del();
	}

	BOOL NFNetClient::SendPost(NFPacket& Packet)
	{
		// 서버로 Send 하기..
		return m_SendQueue.Add(Packet);
	}

	void NFNetClient::GetLocalIP(CHAR* LocalIP)
	{
		CHAR name[256];
		CHAR* TempIp;
		PHOSTENT hostinfo;

		WinSockInit();

		if( gethostname ( name, sizeof(name)) == 0)
		{
			if((hostinfo = gethostbyname(name)) != NULL)
			{
				TempIp = inet_ntoa (*(struct in_addr *)*hostinfo->h_addr_list);
				strcpy(LocalIP, TempIp);
			}
		}
	}

}