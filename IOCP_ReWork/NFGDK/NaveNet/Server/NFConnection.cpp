#include "NaveServer.h"
#include "NFPacketPool.h"
#include "NFConnection.h"
#include "NFUpdateManager.h"
//#include <Nave/NFLog.h>


namespace NaveNetLib {

	NFConnection::NFConnection()
	{
		m_RecvPacket.Init();
		m_RecvIO.NewIOBuf(0);	// 0으로 하면 기본적으로 1024*64만큼 잡힌다.
	}

	NFConnection::~NFConnection()
	{
		m_nMaxBuf = m_nMaxPacketSize = 0;
		m_sckListener	= NULL;

		Disconnect();

		m_eConnectFlag	= CONNECT_NONE;
		m_bIsConnect	= false;
		m_bForce		= false;

		m_hIOCP			= NULL;
		m_pPacketPool	= nullptr;
		m_uRecvTickCnt	= 0;

		m_RecvIO.DeleteIOBuf();
	}

	bool NFConnection::Create(	DWORD			dwIndex,
								HANDLE			hIOCP,
								SOCKET			listener,
								NFPacketPool*	pPacketPool,
								INT				nMaxBuf)
	{	
		// 객체의 멤버 변수 설정 
		m_dwIndex			= dwIndex;
		m_hIOCP				= hIOCP;
		m_sckListener		= listener;
		m_pPacketPool		= pPacketPool;

		m_nMaxBuf			= m_nMaxPacketSize	= nMaxBuf;

		// 객체 OPEN
		if(!Open())
		{
			// 실패하면 어떻게 해야 하는가? 이 컨넥션 클래스는 죽은 클래스가 된다.
			Close_Open(false);
			return false;
		}

		return true;
	}

	bool NFConnection::Open()
	{
		// 절대 값 있어야 함.
		assert(m_nMaxBuf);
		assert(m_nMaxPacketSize);
		assert(m_pPacketPool);
		assert(m_sckListener);

		m_RecvIO.InitIOBuf();	// 패킷버퍼를 초기화한다.
		
 		// create socket for send/recv
		m_Socket = WSASocket( AF_INET, SOCK_STREAM, IPPROTO_IP,NULL,0,WSA_FLAG_OVERLAPPED );

		// 현재 소켓이 제대로 생성 되었는지 검사 
		if (m_Socket == NULL) {
			return false;
		}

		// Accpet할 오버랩구조체와 패킷버퍼를 준비한다.
		LPOVERLAPPEDPLUS newolp = PrepareAcptPacket();

		if(newolp == nullptr) 
		{
			closesocket(m_Socket);
			m_Socket = NULL;
			return false;
		}

		/////////////////////////////////////////////////////////////////////
		// Socket과 Listener와 연결
		// Overlapped에 들어가는 변수가 나중에 IOCP 이벤트 발생 처리에 쓰인다
		static char acceptDummyBuf[64] = {0, };
		bool bRet = AcceptEx(newolp->sckListen,						// Listen Socket
							newolp->sckClient,						// Socket
			acceptDummyBuf, //newolp->wbuf.buf,		// 버퍼 포인터 
							0,	//m_nMaxBuf							// 버퍼 사이즈. 0 이 아니면 데이터를 받아야지 접속 처리한다.  
							sizeof(sockaddr_in) + 16,				// 소켓 정보 - IP, Address, Name.. etc
							sizeof(sockaddr_in) + 16,				// 소켓 정보 - IP, Address, Name.. etc
							&newolp->dwBytes,						// 처리 바이트 크기 
							&newolp->overlapped);					// *중요*

		// 에러 처리 
		if(!bRet && WSAGetLastError() != WSA_IO_PENDING)
		{
			ShowMessage(ACCEPT_FAILED);
			closesocket(m_Socket);
			m_Socket = NULL;
			ReleaseAcptPacket(newolp);
			return false;
		}

		/////////////////////////////////
		// 소켓의 성능 최적화를 위한 세팅 
		INT zero = 0;
		INT err = 0;

		// Send Buffer에 대한 세팅
		if( (err = setsockopt( m_Socket, SOL_SOCKET, SO_SNDBUF, (CHAR *)&zero, sizeof(zero))) == SOCKET_ERROR)
		{
			ShowMessage(ACCEPT_FAILED);
			closesocket(m_Socket);
			m_Socket = NULL;
			ReleaseAcptPacket(newolp);
			return false;
		}

		// Receive Buffer에 대한 세팅 
		if((err = setsockopt( m_Socket, SOL_SOCKET, SO_RCVBUF, (CHAR *)&zero, sizeof(zero) )) == SOCKET_ERROR)
		{
			ShowMessage(ACCEPT_FAILED);
			closesocket(m_Socket);
			m_Socket = NULL;
			ReleaseAcptPacket(newolp);
			return false;
		}

		// 변수 초기화 
		InterlockedExchange((LONG*)&m_bIsConnect,0);
		Clear();

		m_uRecvTickCnt = 0 ;

		return true;
	}

	bool NFConnection::Close_Open( bool bForce )
	{
		// 소켓과 리스너 상태 확인 
		// Disconnect 함수안에서 OnDisconnect(), Clear() 이 호출된다.
		Disconnect(bForce);

		// 이 패킷 다시 초기화 
		if(!Open())
		{
			// 실패하면 어떻게 해야 하는가? 이 컨넥션 클래스는 죽은 클래스가 된다.
			// 뭔가 죽은 상태를 표시해주고 체크하자. 그리고 나중에 타이머에서 다시 초기화 해본다.
			ShowMessage(DEAD_CONNECTION);
			Disconnect();
			return false;
		}

		return true;												// 정상 처리 
	}

	void NFConnection::ShowMessage(INT MsgIndex)
	{
		switch(MsgIndex)
		{
		case ACCEPT_FAILED:
			//LOG_ERROR((L"[%04d] AcceptEx(): SOCKET_ERROR, %d.", GetIndex(), WSAGetLastError()));
			break;
		case CLOSE_SOCKET:
			//LOG_INFO((L"[%04d] 접속 해제.", GetIndex()));
			break;
		case DEAD_CONNECTION:
			//LOG_ERROR((L"[%04d] Dead Connection.", GetIndex()));
			break;
		case CONNECT_SUCCESS:
			//LOG_INFO((L"[%04d] 접속 성공.", GetIndex()));
			break;
		case CONNECT_FAILED:
			//LOG_ERROR((L"[%04d] 접속 실패 (Check Sum 오류).", GetIndex()));
			break;
		case DISPATCH_FAILED:
			//LOG_ERROR((L"[%04d] Dispatch return false.", GetIndex()));
			break;
		case DOIOSWITCH_FAILED:
			//LOG_ERROR((L"[%04d] DoIo(..) - Switch Default.", GetIndex()));
			break;
		case ALLOCACPT_FAILED:
			//LOG_ERROR((L"[%04d] PrepareAcptPacket() acpt packet alloc failed.", GetIndex()));
			break;
		case PREPAREACPT_FAILED:
			//LOG_ERROR((L"[%04d] PrepareAcptPacket().newolp == NULL.", GetIndex()));
			break;
		case PREPARERECVSIZE_FAILED:
			//LOG_ERROR((L"[%04d] PrepareRecvPacket() srclen > m_nMaxPacketSize.", GetIndex()));
			break;
		case PREPARESENDSIZE_FAILED:
			//LOG_ERROR((L"[%04d] PrepareSendPacket() srclen > m_nMaxPacketSize.", GetIndex()));
			break;
		case ALLOCRECV_FAILED:
			//LOG_ERROR((L"[%04d] PrepareRecvPacket() recv packet alloc failed.", GetIndex()));
			break;
		case ALLOCSEND_FAILED:
			//LOG_ERROR((L"[%04d] PrepareSendPacket() send packet alloc failed.", GetIndex()));
			break;
		case PREPARERECV_FAILED:
			//LOG_ERROR((L"[%04d] PrepareRecvPacket().newolp == NULL.", GetIndex()));
			break;
		case PREPARESEND_FAILED:
			//LOG_ERROR((L"[%04d] PrepareSendPacket().newolp == NULL.", GetIndex()));
			break;
		case RELEASEACPT_FAILED:
			//LOG_ERROR((L"[%04d] ReleaseAcptPacket() free acpt packet failed.", GetIndex()));
			break;
		case RELEASERECV_FAILED:
			//LOG_ERROR((L"[%04d] ReleaseRecvPacket() free recv packet failed.", GetIndex()));
			break;
		case RELEASESEND_FAILED:
			//LOG_ERROR((L"[%04d] ReleaseSendPacket() free send packet failed.", GetIndex()));
			break;
		case BINDIOCP_FAILED:
			//LOG_ERROR((L"[%04d] BindIOCP().lpOverlapPlus == NULL.", GetIndex()));
			break;
		case RECVPOST_FAILED:
			//LOG_ERROR((L"[%04d] RecvPost() m_Socket == NULL or IsConnect() == false.", GetIndex()));
			break;
		case RECVPOSTPENDING_FAILED:
			//LOG_ERROR((L"[%04d] RecvPost().WSARecv() == SOCKET_ERROR, %d.", GetIndex(), WSAGetLastError()));
			break;
		case SENDPOSTPENDING_FAILED:
			//LOG_ERROR((L"[%04d] SendPost().WSASend() == SOCKET_ERROR, %d.", GetIndex(), WSAGetLastError()));
			break;
		case SENDPOST_FAILED:
			//LOG_ERROR((L"[%04d] SendPost() == SOCKET_ERROR, %d.", GetIndex(), WSAGetLastError()));
			break;
		}
	}

	bool NFConnection::DoIo( LPOVERLAPPEDPLUS lpOverlapPlus, NFUpdateManager* pUpdateManager)
	{
		// 할당 패킷의 상태 확인 
		switch(lpOverlapPlus->nConnState)
		{
		// ACCEPT관련 처리
		case ClientIoAccept:
			BindIOCP(lpOverlapPlus);							// 현재 소켓과 IOCP 바인딩 처리 

			InterlockedIncrement((LONG*)&m_bIsConnect);		// 접속 상태 변수 ON !!!
			
			SetConnectFlag(CONNECT_true, pUpdateManager);
			
			// Accept 할당 패킷 해제 
			if(!RecvPost())
			{
				SetClose_Open(lpOverlapPlus, pUpdateManager);
				break;
			}

			// TICK 카운트 설정 
			InterlockedExchange((LONG*)&m_uRecvTickCnt,timeGetTime());

			ReleaseAcptPacket( lpOverlapPlus );
			break;

		// RECEIVE 관련 처리 
		case ClientIoRead:
			// TICK 카운트 설정 
			InterlockedExchange((LONG*)&m_uRecvTickCnt,timeGetTime());

			// 처리 데이타가 없다면 
			if(lpOverlapPlus->dwBytes == 0)
			{
				SetClose_Open(lpOverlapPlus, pUpdateManager, false);			// 에러, 객체 다시 초기화 
			}
			else if((INT)lpOverlapPlus->dwBytes == SOCKET_ERROR) // 에러라면 
			{
				SetClose_Open(lpOverlapPlus, pUpdateManager, true);			// 에러, 객체 다시 초기화 
			}
			else// 정상이라면 
			{
				// 메세지 저장 
				if(!DispatchPacket(lpOverlapPlus, pUpdateManager))
				{
					ShowMessage(DISPATCH_FAILED);
					SetClose_Open( lpOverlapPlus, pUpdateManager, true );
				}
				else
				{
					// Receive 할당 패킷 해제 
					ReleaseRecvPacket( lpOverlapPlus );
				}
			}
			break;

		case ClientIoWrite:
			ReleaseSendPacket( lpOverlapPlus );
			break;

		default:
			ShowMessage(DOIOSWITCH_FAILED);
			assert(0);
			break;
		}
		return true;
	}

	LPOVERLAPPEDPLUS NFConnection::PrepareAcptPacket()
	{
		LPOVERLAPPEDPLUS newolp = NULL;

		// get accept overlapped structure and packet buffer.
		if(m_pPacketPool->AllocAcptPacket(newolp) == false)
		{
			ShowMessage(ALLOCACPT_FAILED);
			return nullptr;
		}

		// 치명적인 에러 
		if(!newolp)
		{
			ShowMessage(PREPAREACPT_FAILED);
			return nullptr;
		}

		// clear buffer
		memset(&newolp->overlapped	, NULL, sizeof(OVERLAPPED));
		memset(&newolp->wbuf.buf[0], NULL, sizeof(m_nMaxPacketSize));

		// init olp structure
		newolp->sckListen	= m_sckListener;
		newolp->sckClient	= m_Socket;
		newolp->nConnState	= ClientIoAccept;
		newolp->pClientConn = (void*)this;
		newolp->wbuf.len	= CONNECT_CHECK_SIZE;//	newolp->wbuf.len	= MAXPACKETSIZE;
												// ** WARNING ** 
												// When you change your packet certfying correct connection,
												// you must change the size of definition 'CONNECT_CHECK_SIZE'.
		return newolp;
	}

	LPOVERLAPPEDPLUS NFConnection::PrepareRecvPacket(UINT srclen)
	{
		// 치명적인 에러
		if(srclen > (UINT)m_nMaxPacketSize)
		{
			ShowMessage(PREPARERECVSIZE_FAILED);
			return nullptr;
		}

		LPOVERLAPPEDPLUS newolp = nullptr;

		// get recv overlapped structure and packet buffer.
		if( false == m_pPacketPool->AllocRecvPacket(newolp) )
		{
			ShowMessage(ALLOCRECV_FAILED);
			return nullptr;
		}

		// 치명적인 에러
		if(!newolp)
		{
			ShowMessage(PREPARERECV_FAILED);
			return nullptr;
		}

		// clear buffer
		memset(&newolp->overlapped	, NULL, sizeof(OVERLAPPED));
		memset(&newolp->wbuf.buf[0], NULL, sizeof(m_nMaxPacketSize));

		// init olp structure
		newolp->sckListen	= m_sckListener;
		newolp->sckClient	= m_Socket;
		newolp->nConnState	= ClientIoRead;
		newolp->pClientConn = (void*)this;

		if (srclen == 0) {
			newolp->wbuf.len = m_nMaxPacketSize;
		}
		else {
			newolp->wbuf.len = srclen;
		}

		return newolp;
	}

	LPOVERLAPPEDPLUS NFConnection::PrepareSendPacket(CHAR *psrcbuf, UINT srclen)
	{
		// 치명적인 에러
		if(srclen < 0 || srclen > (UINT)m_nMaxPacketSize)
		{
			ShowMessage(PREPARESENDSIZE_FAILED);
			return nullptr;
		}

		LPOVERLAPPEDPLUS newolp = nullptr;

		// get recv overlapped structure and packet buffer.
		if( false == m_pPacketPool->AllocSendPacket(newolp) )
		{
			ShowMessage(ALLOCSEND_FAILED);
			return nullptr;
		}

		// 치명적인 에러
		if(!newolp)
		{
			ShowMessage(PREPARESEND_FAILED);
			return nullptr;
		}

		// clear buffer
		memset(&newolp->overlapped	, NULL, sizeof(OVERLAPPED));
		memset(&newolp->wbuf.buf[0], NULL, sizeof(m_nMaxPacketSize));

		// init olp structure
		newolp->sckListen	= m_sckListener;
		newolp->sckClient	= m_Socket;
		newolp->nConnState	= ClientIoWrite;
		newolp->pClientConn = (void*)this;
		newolp->wbuf.len	= srclen; 
		memcpy(newolp->wbuf.buf,psrcbuf,srclen);
		
		return newolp;
	}

	bool NFConnection::ReleaseAcptPacket(LPOVERLAPPEDPLUS olp)
	{
		if (NULL == olp) {
			return false;
		}

		if (NULL == olp->wbuf.buf) {
			return false;
		}

		if(!m_pPacketPool->FreeAcptPacket(olp))
		{
			ShowMessage(RELEASEACPT_FAILED);
			return false;
		}

		return true;
	}

	bool NFConnection::ReleaseRecvPacket(LPOVERLAPPEDPLUS olp)
	{
		if (olp == NULL) {
			return false;
		}

		if (olp->wbuf.buf == NULL) {
			return false;
		}

		if(!m_pPacketPool->FreeRecvPacket(olp))
		{
			ShowMessage(RELEASERECV_FAILED);
			return false;
		}
		return true;
	}

	bool NFConnection::ReleaseSendPacket(LPOVERLAPPEDPLUS olp)
	{
		if (olp == NULL) {
			return false;
		}

		if (olp->wbuf.buf == NULL) {
			return false;
		}

		if(!m_pPacketPool->FreeSendPacket(olp))
		{
			ShowMessage(RELEASESEND_FAILED);
			return false;
		}
		return true;
	}

	bool NFConnection::BindIOCP(LPOVERLAPPEDPLUS lpOverlapPlus)
	{
		// 치명적인 에러
		if(!lpOverlapPlus)
		{
			ShowMessage(BINDIOCP_FAILED);
			return false;
		}

		INT	locallen = 0, remotelen = 0;

		sockaddr_in *	plocal = 0;
		sockaddr_in *	premote	= 0;

		GetAcceptExSockaddrs(
			&(lpOverlapPlus->wbuf.buf[0]),
			0,//m_nMaxBuf,
			sizeof(sockaddr_in) + 16,
			sizeof(sockaddr_in) + 16,
			(sockaddr **)&plocal,									// 서버단 
			&locallen,
			(sockaddr **)&premote,									// 로컬단 
			&remotelen
		);

		memcpy(&m_Local, plocal, sizeof(sockaddr_in));
		memcpy(&m_Peer, premote, sizeof(sockaddr_in));

		if (CreateIoCompletionPort((HANDLE)lpOverlapPlus->sckClient, m_hIOCP, 0, 0) == 0) {
			return false;
		}

		return true;
	}

	void NFConnection::SetClose_Open(LPOVERLAPPEDPLUS lpOverlapPlus, NFUpdateManager* pUpdateManager, bool bForce)
	{
		if(m_eConnectFlag != CONNECT_NONE)
		{
			//LOG_ERROR((L"NFConnection::SetClose_Open() m_eConnectFlag == %d", (int)m_eConnectFlag));
		}

		// 할당 패킷 상태 검사후 패킷 할당 해제 
		if(NULL != lpOverlapPlus)
		{
			if(NULL != lpOverlapPlus->wbuf.buf && NULL != m_pPacketPool)
			{
				// 마지막으로 완료 되었던 오버랩구조체와 버퍼를 릴리즈한다.
				switch( lpOverlapPlus->nConnState)
				{
				case ClientIoAccept:
					ReleaseAcptPacket( lpOverlapPlus );
					break;

				case ClientIoRead:
					ReleaseRecvPacket( lpOverlapPlus );
					break;

				case ClientIoWrite:
					ReleaseSendPacket( lpOverlapPlus );
					break;

				default:
					break;
				}
			}
		}

		m_bForce = bForce;

		InterlockedExchange((LONG*)&m_eConnectFlag,CLOSEOPEN_true);

		pUpdateManager->Add(this, NULL);
	}

	void NFConnection::SetConnectFlag(CONNECT_EVENT eState, NFUpdateManager* pUpdateManager)
	{
		if(m_eConnectFlag != CONNECT_NONE)
		{
			//LOG_ERROR((L"NFConnection::SetConnectFlag() m_eConnectFlag == %s", (int)m_eConnectFlag));
		}

		InterlockedExchange((LONG*)&m_eConnectFlag,eState);

		pUpdateManager->Add(this, NULL);
	}

	bool NFConnection::DispatchPacket(LPOVERLAPPEDPLUS lpOverlapPlus, NFUpdateManager* pUpdateManager)
	{
		// Read같은경우 RecvPost 할때 마다 1번 발생하는데 현재
		// Read할때 RecvPost를 1번만 하기 대문에 스레드가 아무리 많아도.DispatchPacket은 1번만 일어난다.

		CHAR*  psrcbuf     =	&( lpOverlapPlus->wbuf.buf[0] );
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
			m_RecvPacket.DecryptPacket();

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
			m_RecvPacket.DecryptPacket();

			if(m_RecvPacket.IsAliveChecksum())
			{	
				DispatchPacket(m_RecvPacket);
			}

			m_RecvPacket.Init();
		}
		else
			m_RecvIO.UnLock();
#endif

		// 그리고 새로운 recieve buffer를 준비하여 Post한다.
		return RecvPost();
	}

	void NFConnection::UpdatePacket(NFPacket& Packet)
	{
		if(Packet.GetCommand() != 0)
		{
			DispatchPacket(Packet);
			return;
		}

		// 커넥션의 상태를 업데이트 하기위한 부분.
		if (m_eConnectFlag == CONNECT_NONE) {
			return;
		}

		if(m_eConnectFlag == CONNECT_true)
		{
			InterlockedExchange((LONG*)&m_eConnectFlag,CONNECT_NONE);
			OnConnect(true);
		}
		else if(m_eConnectFlag == CONNECT_false)
		{
			InterlockedExchange((LONG*)&m_eConnectFlag,CONNECT_NONE);
			OnConnect(false);
			Close_Open();
		}
		else if(m_eConnectFlag == DISCONNECT_TURE)
		{
			InterlockedExchange((LONG*)&m_eConnectFlag,CONNECT_NONE);
			Disconnect(false);
		}
		else if(m_eConnectFlag  == CLOSEOPEN_true)
		{
			InterlockedExchange((LONG*)&m_eConnectFlag,CONNECT_NONE);
			Close_Open(m_bForce);
		}
	}

	bool NFConnection::SendPost( CHAR* pPackte, INT Len )
	{
		if (!m_Socket) {
			return false;
		}

		if (!IsConnect()) {
			return false;
		}

		if(m_Socket == NULL || IsConnect() == false)
		{
			ShowMessage(SENDPOST_FAILED);
			return false;
		}

		// prepare recieve buffer
		LPOVERLAPPEDPLUS newolp = PrepareSendPacket(pPackte,Len);

		// 제대로 할당 받았는지 조사
		if (newolp == nullptr) {
			return false;
		}

		INT ret = WSASend(	newolp->sckClient,
							&newolp->wbuf,
							1,
							&newolp->dwBytes,// 만약 호출했을때 바로 받았다면 여기로 받은 크기가 넘어오지만 iocp에서는 의미가 없다.
							newolp->dwFlags,
							&newolp->overlapped,// Overlapped 구조체 
							NULL );
		
		// 에러 처리 
		if(ret == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING)
		{
			ShowMessage(SENDPOSTPENDING_FAILED);
			ReleaseSendPacket(newolp);
			return false;
		}
		return true;	
	}

	bool NFConnection::SendPost( NFPacket& Packet )		// 실제 Send 처리
	{
		// 보내기 직전 체크섬을 생성한다.
		Packet.EncryptPacket();

		INT Len = Packet.m_Header.Size;
		CHAR* pPacket = (CHAR*)&Packet;
		return SendPost(pPacket, Len);
	}

	bool NFConnection::RecvPost(UINT buflen)
	{
		if(m_Socket == NULL || IsConnect() == false)
		{
			ShowMessage(RECVPOST_FAILED);
			return false;
		}

		// prepare recieve buffer
		LPOVERLAPPEDPLUS newolp = PrepareRecvPacket(buflen);

		// 제대로 할당 받았는지 조사
		if (newolp == nullptr) {
			return false;
		}

		INT ret = WSARecv(	newolp->sckClient,
							&newolp->wbuf,
							1,
							&newolp->dwBytes,						// 만약 호출했을때 바로 받았다면 여기로 받은 크기가 넘어오지만 iocp에서는 의미가 없다.
							&newolp->dwFlags,
							&newolp->overlapped,					// Overlapped 구조체 
							NULL );
		
		// 에러 처리 
		if(ret == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING)
		{
			ShowMessage(RECVPOSTPENDING_FAILED);
			ReleaseRecvPacket(newolp);
			return false;
		}
		return true;
	}

	bool NFConnection::GetClientIP(INT *iIP)
	{
		if (iIP == NULL) {
			return false;
		}

		if(!IsConnect())
		{
			iIP[0] = 0;
			return false;
		}

		iIP[0] = m_Peer.sin_addr.S_un.S_un_b.s_b1;
		iIP[1] = m_Peer.sin_addr.S_un.S_un_b.s_b2;
		iIP[2] = m_Peer.sin_addr.S_un.S_un_b.s_b3;
		iIP[3] = m_Peer.sin_addr.S_un.S_un_b.s_b4;

		return true;
	}

	bool NFConnection::GetClientIP(CHAR *szIP)
	{
		if (szIP == NULL) {
			return false;
		}

		if(!IsConnect())
		{
			szIP[0] = NULL;
			return false;
		}

		sprintf_s(szIP, 64,"%d.%d.%d.%d",(INT)m_Peer.sin_addr.S_un.S_un_b.s_b1,
									(INT)m_Peer.sin_addr.S_un.S_un_b.s_b2,
									(INT)m_Peer.sin_addr.S_un.S_un_b.s_b3,
									(INT)m_Peer.sin_addr.S_un.S_un_b.s_b4);

		return true;
	}

	bool NFConnection::GetClientIP(wchar_t *szIP)
	{
		if (szIP == NULL) {
			return false;
		}

		if(!IsConnect())
		{
			szIP[0] = NULL;
			return false;
		}

		swprintf_s(szIP, 64, L"%d.%d.%d.%d",(INT)m_Peer.sin_addr.S_un.S_un_b.s_b1,
									(INT)m_Peer.sin_addr.S_un.S_un_b.s_b2,
									(INT)m_Peer.sin_addr.S_un.S_un_b.s_b3,
									(INT)m_Peer.sin_addr.S_un.S_un_b.s_b4);

		return true;
	}

	bool NFConnection::GetClientIP(sockaddr_in& addr)
	{
		if(!IsConnect())
		{
			memcpy(&addr,0,sizeof(sockaddr_in));
			return false;
		}

		CopyMemory((CHAR*)&addr,(CHAR*)&m_Peer,sizeof(sockaddr_in));
		return true;
	}

	void NFConnection::Disconnect(bool bForce)
	{
		// 연결 되어있는지 확인 
		if (!IsConnect()) {
			return;
		}

		// 변수 초기화 
		InterlockedExchange((LONG*)&m_eConnectFlag,CONNECT_NONE);
		InterlockedExchange((LONG*)&m_bIsConnect,false);
		m_uRecvTickCnt = 0;
		m_bForce		= false;

		// 소켓과 리스너 상태 확인 
		if(m_Socket)
		{
			struct linger li = {0, 0};								// Default: SO_DONTLINGER
			shutdown(m_Socket, SD_BOTH );						

			// 소켓에 처리 되지 않은 데이타가 소켓 버퍼에 남아있다면,
			if (bForce) {
				li.l_onoff = 1; // SO_LINGER, timeout = 0
			}

			// 잔여 데이타가 있으면 제거
			setsockopt(m_Socket, SOL_SOCKET, SO_LINGER, (CHAR *)&li, sizeof(li));
			closesocket(m_Socket);									// 소켓 닫기 
			m_Socket = NULL;										// 초기화 

			ShowMessage(CLOSE_SOCKET);
		}

		OnDisconnect();

		Clear();
	}

	bool NFConnection::IsConnect()
	{
		bool bRet = m_bIsConnect;
		return bRet;
	}

	LONG NFConnection::GetRecvTickCnt()
	{
		LONG lRecvTickCount = m_uRecvTickCnt;
		return lRecvTickCount;
	}
}