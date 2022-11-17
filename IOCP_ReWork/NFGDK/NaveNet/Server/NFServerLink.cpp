#include "NaveServer.h"
#include "NFConnection.h"
#include "NFServerLink.h"
#include "NFUpdateManager.h"
//#include <Nave/NFLog.h>

// 1.Server -> 2.Server 로 접속하기 위해 1.Server에서 사용하는 Connect용 클래스이다.
// 이렇게 하면 1.Server에서 2.Server로 접속할때 IOCP를 사용해 Send,Recv등을 한다.

namespace NaveNetLib {
		
	//////////////////////////////////////////////////////////////////
	// [1]DESCRIPTION : 멤버 변수 세팅 및 객체 초기화				//
	// [2]PARAMETER : dwIndex - 객체 고유 ID						//
	//				  hIOCP - IOCP Handle							//
	//				  listener - Listen Socket						//
	//				  pPacketPool - 패킷 Pool 포인터				//
	//				  pMsg - BroadCasting을 휘한 메세지 처리 포인터 //
	//				  MsgWait - 메세지 Send를 위한 이벤트 핸들		// 
	// [3]RETURN : true - 정상 처리, false - 실패 					//
	// [4]DATE : 2003년 10월 24일									//
	//////////////////////////////////////////////////////////////////
	bool NFServerLink::Create(DWORD			dwIndex,
							HANDLE			hIOCP,
							NFPacketPool*	pPacketPool,
							wchar_t*			sIP,
							int				nPort,
							int				nMaxBuf)
	{	
		wcscpy_s(m_strIP, 32, sIP);
		m_iPort = nPort;

		if(NFConnection::Create(dwIndex, hIOCP, NULL, pPacketPool, nMaxBuf) == false)
		{
			OnConnect(false);
			
			// 실패하면 어떻게 해야 하는가? 이 컨넥션 클래스는 죽은 클래스가 된다.
			return false;
		}

		OnConnect(true);
		return true;
	}

	//////////////////////////////////////////////////////////////////
	// [1]DESCRIPTION : 객체 초기화, 리슨 소켓 결합					//
	// [2]PARAMETER : void											//
	// [3]RETURN : true - 정상처리 , false - 실패 					//
	// [4]DATE : 2003년 10월 24일									//
	//////////////////////////////////////////////////////////////////
	bool NFServerLink::Open()
	{
		// MRS의 Open은 1->2 번으로 Connect 할때 사용된다.
		// 절대 값 있어야 함.
		
		//TODO: 최흥배. 아래 두 변수를 조사해서 상황에 맞게 처리하기
		assert(m_nMaxBuf);
		assert(m_nMaxPacketSize);

		// 패킷 Pool이 제대로 설정 되었는지 검사 
		if (m_pPacketPool == nullptr) {
			return false;
		}

		m_RecvIO.InitIOBuf();	// 패킷버퍼를 초기화한다.

 		// create socket for send/recv
		m_Socket = WSASocket( AF_INET, SOCK_STREAM, IPPROTO_IP,NULL,0,WSA_FLAG_OVERLAPPED );

		// 현재 소켓이 제대로 생성 되었는지 검사 
		if (m_Socket == NULL) {
			return false;
		}

		// 소켓 연결 변수 생성 및 초기화 
		SOCKADDR_IN		saServer;		
		memset(&saServer,0,sizeof(saServer));

		// 유니코드가 기본 프로그래밍 이기 때문에 자동으로 멀티바이트로 변경한다. 
		// WideCharToMultiByte 함수를 사용한 이유는 무분별하게 Nave 라이브러리를 링크하지 않기 위해서
		// 이 한줄을 위해서 Nave의 StringUtil을 사용하는건 성능낭비
		char strIP[16];
		int Ret = (int)WideCharToMultiByte(CP_ACP, 0, m_strIP, (int)wcslen(m_strIP), strIP, 15, NULL, NULL);
		strIP[Ret] = 0;

		// 소켓 타입, IP, PORT 설정 
		saServer.sin_family = AF_INET;
		//saServer.sin_addr.s_addr = inet_addr(strIP);
		//saServer.sin_port = htons(m_iPort);
		auto ret = inet_pton(AF_INET, strIP, (void *)&saServer.sin_addr.s_addr);
		saServer.sin_port = htons(m_iPort);

		// 서버와 Connect
		INT nRet = connect(m_Socket,(sockaddr*)&saServer, sizeof(saServer));

		// 소켓 에러이거나 블럭킹이 되었다면 
		if (nRet == SOCKET_ERROR &&	WSAGetLastError() != WSAEWOULDBLOCK)
		{
			//LOG_ERROR((L"Socket Connect Faild : SOCKET_ERROR, %d", WSAGetLastError()));
			closesocket(m_Socket);
			m_Socket = NULL;
			return false;
		}

		//////////////////////////////
		// 메세지 라우팅 서버의 IOCP반응 만들기 
		send(m_Socket,CONNECT_CHECK_DATA,CONNECT_CHECK_SIZE,MSG_DONTROUTE);

		// IOCP 와 연결 
		if(CreateIoCompletionPort((HANDLE)m_Socket,m_hIOCP,0,0) == 0)
		{
			//LOG_ERROR((L"IOCP Connect Faild : SOCKET_ERROR, %d", WSAGetLastError()));
			closesocket(m_Socket);
			m_Socket = NULL;
			return false;
		}
		
		// 연결 확인 변수 ON !!!!!
		InterlockedIncrement((LONG*)&m_bIsConnect);

		// RECEIVE 상태로 객체 전환 
		if(!RecvPost())
		{
			//LOG_ERROR((L"RecvPost Faild : SOCKET_ERROR, %d", WSAGetLastError()));
			closesocket(m_Socket);
			m_Socket = NULL;
			return false;
		}
		
		/////////////////////////////////
		// 소켓의 성능 최적화를 위한 세팅 
		INT zero = 0;
		INT err = 0;

		// Send Buffer에 대한 세팅
		if( (err = setsockopt( m_Socket, SOL_SOCKET, SO_SNDBUF, (CHAR *)&zero, sizeof(zero))) == SOCKET_ERROR)
		{
			//LOG_ERROR((L"Send Buffer setsockopt Faild : SOCKET_ERROR, %d", WSAGetLastError()));
			closesocket(m_Socket);
			m_Socket = NULL;
			return false;
		}

		// Receive Buffer에 대한 세팅 
		if((err = setsockopt( m_Socket, SOL_SOCKET, SO_RCVBUF, (CHAR *)&zero, sizeof(zero) )) == SOCKET_ERROR)
		{
			//LOG_ERROR((L"Receive Buffer setsockopt Faild : SOCKET_ERROR, %d", WSAGetLastError()));
			closesocket(m_Socket);
			m_Socket = NULL;
			return false;
		}

		// Server로 연결이 성공했을때 처음으로 보낼 패킷이다.
		return SendOpenPacket();
	}

	bool NFServerLink::Close_Open( bool bForce )
	{
		// 소켓과 리스너 상태 확인 
		if(m_Socket)
		{
			ShowMessage(CLOSE_SOCKET);
			
			struct linger li = {0, 0};	// Default: SO_DONTLINGER

			// 소켓에 처리 되지 않은 데이타가 소켓 버퍼에 남아있다면,
			if (bForce) {
				li.l_onoff = 1; // SO_LINGER, timeout = 0
			}

			shutdown(m_Socket, SD_BOTH );						// 오잉? 이게 뭐지? ^^;; 담에 찾아보자 
																// 2001년 9월 6일 

			// 잔여 데이타가 있으면 제거, 없으면.. 통과? ^^;; 뭐 그런것..
			setsockopt(m_Socket, SOL_SOCKET, SO_LINGER, (CHAR *)&li, sizeof(li));
			closesocket(m_Socket);								// 소켓 닫기 
			m_Socket = NULL;
		}

		// 변수 초기화 
		InterlockedExchange((LONG*)&m_bIsConnect,0);
		m_uRecvTickCnt = 0;

		OnDisconnect();

		Clear();
		return true;												// 정상 처리 
	}

	//////////////////////////////////////////////////////////////////
	// [1]DESCRIPTION : 메세지 저장 및 Receive 신호 발생, Socket정리//
	// [2]PARAMETER : LPOVERLAPPEDPLUS - 해제할 overlappedplus구조체//
	// [3]RETURN : true - 정상 처리									//
	// [4]DATE : 2003년 10월 24일									//
	//////////////////////////////////////////////////////////////////
	bool NFServerLink::DispatchPacket(LPOVERLAPPEDPLUS lpOverlapPlus, NFUpdateManager* pUpdateManager)
	{
		CHAR*  psrcbuf     =    &( lpOverlapPlus->wbuf.buf[0] );
		INT     srclen     =    lpOverlapPlus->dwBytes;

		// Packet정보 자체가 [H 2byte][P size] 형식이다.
		m_RecvIO.Append(psrcbuf, srclen);

		m_RecvIO.Lock();

		// IOCP는 스레드 패킷처리에 의한 성능향상이 주 능력이다
		// 그런데 아래와 같이 UpdateManaget에 패킷을 넣은후 주 스레드에서 Update를 처리하면
		// 데드락같은 문제는 생기지 않지만 다중처리에 의한 성능향상이 생기지 않는다.
		// 패킷은 스레드 상태에서 바로 처리하게 수정하고 UpdateManager은 커넥트 플래스를
		// 업데이트 하는걸로 제한해보자. 
#ifdef ENABLE_UPDATEQUE
		if(m_RecvIO.GetPacket(&m_RecvPacket) == 1)
		{
			if (m_RecvPacket.IsAliveChecksum()) {
				pUpdateManager->Add(this, &m_RecvPacket);
			}

			m_RecvPacket.Init();
		}
		else
		{
			m_RecvIO.UnLock();
		}
#else
		// 아래와 같이 스레드에서 패킷을 처리하게 되면 
		// Dispatch에 크리티컬 세션을 집어넣어 데드락에 주의해야한다.
		if(m_RecvIO.GetPacket(&m_RecvPacket) == 1)
		{
			DispatchPacket(m_RecvPacket);

			m_RecvPacket.Init();
		}
		else
			m_RecvIO.UnLock();
#endif

		// 그리고 새로운 recieve buffer를 준비하여 Post한다.
		return RecvPost();
	}

}