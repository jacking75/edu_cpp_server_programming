#include "NaveServer.h"
#include "NFServerCtrl.h"
#include "NFUpdateManager.h"
//#include <Nave/NFException.h>
//#include <Nave/NFLog.h>
//#include <Nave/NFSync.h>

namespace NaveNetLib {

	#ifndef _DELETE_ARRAY
	#define _DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
	#endif

	//////////////////////////////////////////////////////////////////////
	// Construction/Destruction
	//////////////////////////////////////////////////////////////////////
	NFServerCtrl::NFServerCtrl()
	{
		m_bServerRun	= false;							// 서버 구동상태
		m_bPause		= false;

		m_nMaxThreadNum	= (GetNumberOfProcess() * 2);								// CPU수 * 2개로 Thread 수 결정 

		m_iPort			= 0;
		m_iMaxConn		= 50;

		//m_pWorkThread		= NULL;								// 메인 스레드 핸들 
		//m_hProcThread	= 0;
		//m_hPacketThread  = 0;
		m_hIOCP			= NULL;								// IOCP 핸들 
		m_pUpdateManager = new NFUpdateManager();
	}

	NFServerCtrl::~NFServerCtrl()
	{
		m_bServerRun = false;
		
		if (m_pUpdateManager != nullptr)
		{
			delete m_pUpdateManager;
			m_pUpdateManager = nullptr;
		}
	}

	//////////////////////////////////////////////////////////////////
	// [1]DESCRIPTION : IOCP 핸들 생성								//
	// [2]PARAMETER : void											//
	// [3]RETURN : HANDLE = 생성된 IOCP								//
	// [4]DATE : 2003년 10월 24일									//
	//////////////////////////////////////////////////////////////////
	HANDLE NFServerCtrl::CreateIOCP()
	{
		auto handle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
		return handle;
	}

	//////////////////////////////////////////////////////////////////
	// [1]DESCRIPTION : Listen 소켓 생성 							//
	// [2]PARAMETER : void											//
	// [3]RETURN : SOCKET = 생성된 LISTEN SOCKET					//
	// [4]DATE : 2003년 10월 24일									//
	//////////////////////////////////////////////////////////////////
	SOCKET NFServerCtrl::CreateListenSocket(INT nServerPort, CHAR cBindQue)
	{
		SOCKET Socket = NULL;								// a Socket Variable for using Listener
				
		// [1] Create Listen Socket
		Socket = WSASocket( AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED );

		if (Socket == INVALID_SOCKET) {
			return Socket;
		}				

		// [2] bind listen socket
		SOCKADDR_IN serveraddr;
		ZeroMemory(&serveraddr, sizeof(serveraddr));
		serveraddr.sin_family = AF_INET;
		serveraddr.sin_port = htons(nServerPort);
		serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

		if (bind(Socket, (SOCKADDR *)&serveraddr, sizeof(serveraddr)) == SOCKET_ERROR) {
			return Socket;
		}

		// [3] listening for an concurrent incoming connections limited in 5
		if (listen(Socket, cBindQue) == SOCKET_ERROR) {
			return Socket;
		}

		LISTENER stLin;
		stLin.Init();

		stLin.s = Socket;
		stLin.nPort = nServerPort;
		stLin.cBindQue = cBindQue;

		m_vecListener.push_back(stLin);

		return Socket;
	}

	//////////////////////////////////////////////////////////////////
	// [1]DESCRIPTION : Listen Socket 을 iocp 소켓에 연결			//
	// [2]PARAMETER : SOCKET sckListener							//
	// [3]RETURN : HANDLE = 연결된 IOCP								//
	// [4]DATE : 2003년 10월 24일									//
	//////////////////////////////////////////////////////////////////
	HANDLE NFServerCtrl::ConnectIOCPSocket(SOCKET sckListener)
	{
		return CreateIoCompletionPort((HANDLE)sckListener,m_hIOCP,0,0);
	}

	//////////////////////////////////////////////////////////////////
	// [1]DESCRIPTION : SOCKET과련 라이브러리 활성화 				//
	// [2]PARAMETER : void											//
	// [3]RETURN : void							 					//
	// [4]DATE : 2003년 10월 24일									//
	//////////////////////////////////////////////////////////////////
	bool NFServerCtrl::InitSocket()
	{
		WSADATA wsa;
		if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
			return false;
		}

		return true;
	}

	//////////////////////////////////////////////////////////////////
	// [1]DESCRIPTION : 시스템의 CPU 수를 구함 						//
	// [2]PARAMETER : void											//
	// [3]RETURN : INT = 현재 시스템의 CPU 수 반환 					//
	// [4]DATE : 2003년 10월 24일									//
	//////////////////////////////////////////////////////////////////
	INT NFServerCtrl::GetNumberOfProcess()
	{
		SYSTEM_INFO si;												// a System Info Structure Object
		GetSystemInfo( &si );										// Get the System Information
		return (INT)si.dwNumberOfProcessors;						// return the number of processors
	}

	//////////////////////////////////////////////////////////////////
	// [1]DESCRIPTION : 서버 정지 			 						//
	// [2]PARAMETER : void											//
	// [3]RETURN : bool - 의미 없음				 					//
	// [4]DATE : 2003년 10월 24일									//
	//////////////////////////////////////////////////////////////////
	bool NFServerCtrl::Stop()
	{
		//////////////////////////////////////////////////////////////////////
		//						Server Closing Process						//
		//////////////////////////////////////////////////////////////////////
		INT nCnt;
		// [01] Queue Suicide Packets into each IOCP Main Thread
		for( nCnt = 0; nCnt < m_nMaxThreadNum; nCnt++ )
		{
			if (PostQueuedCompletionStatus(m_hIOCP, 0, IOCP_SHUTDOWN, 0) == NULL) {
				return false;
			}
		}
				
		for (auto& thread : m_MainThreadList) {
			if (thread.joinable()) {
				thread.join();
			}
		}

		// [04] Process Thread Stop and Close
		m_bServerRun = false;
		
		if (m_ProcessThread.joinable()) {
			m_ProcessThread.join();
		}
		if (m_PacketThread.joinable()) {
			m_PacketThread.join();
		}

		if(ReleaseSession())
		{
			//LOG_IMPORTANT((L"shutdown session.."));
		}

		// [05] Close Listener Socket
		auto iListener = (int)m_vecListener.size();
		for(int i = 0; i < iListener; ++i)
		{
			m_vecListener[i].Release();
		}

		// [6] Close IOCP Handle
		if (m_hIOCP) {
			CloseHandle(m_hIOCP);
		}

		// [7] 소켓 라이브러리 종료 
		WSACleanup();

		// [8] Show the Result of Close Processing
		//LOG_IMPORTANT((L"shutdown sequence finished.."));

		return true;
	}

	//////////////////////////////////////////////////////////////////
	// [1]DESCRIPTION : 서버 구동			 						//
	// [2]PARAMETER : void											//
	// [3]RETURN : bool - 의미 없음				 					//
	// [4]DATE : 2003년 10월 24일									//
	//////////////////////////////////////////////////////////////////
	bool NFServerCtrl::Start(INT nPort, INT nMaxConn, INT nMaxThreadNum)
	{
		m_iPort = nPort;
		m_iMaxConn = nMaxConn;

		// 현재 사용되는 스레드는 Work용 스레드 CPU*2개
		// 받은 패킷이나 이벤트등을 처리하는 Update용 스레드 1개를 사용한다.
		// 만약 몬스터나 기타 다른 게임의 상태를 업데이트 하기위해서는 
		// 직접 작업용 스레드를 생성해 물리면 된다.
		/*
			생각하는것
			IOCP용 스레드 CPU개수만큼 (IOCP를 통해 Accept, Recv, Send를 하기 위한 스레드)
			EventUpdate용 스레드 1개  (패킷을 업데이트 하기 위한 큐)
			게임의 상태(몬스터및기타상태)용 스레드 ?개 
		*/

		// 0이아니면 그 개수 사용
		if (nMaxThreadNum != 0) {
			m_nMaxThreadNum = nMaxThreadNum;
		}
		else {
			m_nMaxThreadNum = (GetNumberOfProcess() * 2);			// CPU수 * 2개로 Thread 수 결정 
		}

		// [00] 변수 및 객체 선언 
		INT				nCnt = 0;									// 루프 변수  
		//UINT			nDummy;										// 쓰레기 값 처리 
		SOCKET			skListener;

		// [01] initialize socket library
		if (!InitSocket()) {
			goto Error_Procss;
		}

		// [02] Create IOCP 
		if ((m_hIOCP = CreateIOCP()) == NULL) {
			goto Error_Procss;
		}

		// [03] Create Listener Socket
		if ((skListener = CreateListenSocket(m_iPort)) == INVALID_SOCKET) {
			goto Error_Procss;
		}

		// [04] Connect listener socket to IOCP
		if (ConnectIOCPSocket(skListener) == NULL) { // 내가 주석 달았다
			goto Error_Procss;
		}

		// [05] 화면에 서버 정보 표시 
		ShowServerInfo();

		// [06] Create Session..  // 내가 주석 달았다
		if (CreateSession(skListener) == false) {
			goto Error_Procss;
		}

		// [07] Update Session..
		//if ((m_pWorkThread = new HANDLE[m_nMaxThreadNum]) == NULL) {
		//	goto Error_Procss;	// Create thread Control Handle
		//}
		//for(nCnt = 0; nCnt < m_nMaxThreadNum; nCnt++)								// Run Thread 
		//{
		//	if ((m_pWorkThread[nCnt] = (HANDLE)_beginthreadex(0, 0, Thread_MainEx, m_hIOCP, 0, &nDummy)) == NULL) {
		//		goto Error_Procss;
		//	}
		//}
		for (nCnt = 0; nCnt < m_nMaxThreadNum; nCnt++)								// Run Thread 
		{
			m_MainThreadList.emplace_back([this]() { Thread_MainEx();  });
		}

		//--------------> Server Initializing has been done <---------------//
		// [8] Process Thread 생성하기
		m_bServerRun = true;
		/*if ((m_hProcThread = (HANDLE)_beginthreadex(0, 0, Process_MainEx, this, 0, &nDummy)) == NULL) {
			goto Error_Procss;
		}*/
		m_ProcessThread = std::thread([this]() { Process_MainEx();  });

		// [9] Process Thread 생성하기
		/*if ((m_hPacketThread = (HANDLE)_beginthreadex(0, 0, Packet_MainEx, this, 0, &nDummy)) == NULL) {
			goto Error_Procss;
		}*/
		m_PacketThread = std::thread([this]() { Packet_MainEx();  });

		return true;

	Error_Procss:
		Stop();
		m_bServerRun = false;
		return false;
	}

	void NFServerCtrl::ShowServerInfo()
	{
		//wchar_t	szDate[32], szTime[32];

		//_tzset();
		//_wstrdate( szDate );
		//_wstrtime( szTime );
		//LOG_IMPORTANT((L"------------------------------------------------"));
		//LOG_IMPORTANT((L" %s initialized at %s, %s", L"Server", szDate, szTime) );
		//LOG_IMPORTANT((L"------------------------------------------------"));

		//////////////////////////////////////////////////////////////////////
		//							Server start							//
		//////////////////////////////////////////////////////////////////////
		//LOG_IMPORTANT((L"------------------------------------------------"));
		//LOG_IMPORTANT((L"|                 SERVER START                 |"));
		//LOG_IMPORTANT((L"------------------------------------------------"));

		std::string Ip;
		GetLocalIP(Ip);
		//LOG_IMPORTANT((L"IP(%s) Port(%d)", Ip, m_iPort ));
	}

	void NFServerCtrl::Pause(bool bPause)
	{
		m_bPause = bPause;
	}

	void NFServerCtrl::Update()
	{
/*		if(timeGetTime() - uRecvTickCnt > 1000)
		{
			InterlockedExchange((LONG*)&iMaxRecvPacket,iRecvPacket);
			InterlockedExchange((LONG*)&iRecvPacket,0);
			InterlockedExchange((LONG*)&uRecvTickCnt,timeGetTime());
		}
*/
	}

	//////////////////////////////////////////////////////////////////
	// [1]DESCRIPTION : IOCP처리를 담당하는 메인 스레드 			//
	// [2]PARAMETER : lpvoid - IOCP Handle							//
	// [3]RETURN : bool - 의미 없음				 					//
	// [4]DATE : 2003년 10월 24일									//
	//////////////////////////////////////////////////////////////////
	void NFServerCtrl::Thread_MainEx()
	{
		DWORD			dwIoSize = 0;									// 완료처리 사이즈 얻음 
		ULONG_PTR		lCompletionKey = 0;					// 무한 루프 탈출 용이 됨 
		bool			bSuccess;									// 블럭킹 처리 에러 확인  
		HANDLE			hIOCP = m_hIOCP;					// IOCP 핸들 얻음 
		LPOVERLAPPED	lpOverlapped = NULL;			// 중첩 확장 포인터 
																
		while (true)
		{
			// IOCP 처리를 기다리는 BLOCKING MODE
			bSuccess = GetQueuedCompletionStatus(hIOCP,										// IOCP Handle
												&dwIoSize,									// 처리 사이즈 
				(PULONG_PTR)&lCompletionKey,							// 완료 키 
												(LPOVERLAPPED*)&lpOverlapped,				// 중첩 확장 
												INFINITE);									// Waiting Time 

			LPOVERLAPPEDPLUS lpOverlapPlus = (LPOVERLAPPEDPLUS)lpOverlapped;

			if (bSuccess)
			{
				// 종료 신호가 들어왔다면, 루프 탈출 
				if (lCompletionKey == IOCP_SHUTDOWN) {
					break;
				}
				
				if (NULL != lpOverlapPlus)
				{
					///////////////////////////////////////////////
					// 처리가 정상적으로 이루어진다면 이쪽으로...
					lpOverlapPlus->dwBytes = dwIoSize;				// 처리 데이타 Size
																	// 처리 변수 Cast 변환 
																	// 속도를 위해 메인프레임에선 try~catch를 뺀다.
																	// 알수 없는 오류를 검사하기 위해선 Exception::EnableUnhandledExceptioinFilter(true)를 사용한다
																	/*
																	// Recv 카운트 처리
																	if(lpOverlapPlus->nConnState == ClientIoRead)
																	{
																	InterlockedExchange((LONG*)&iRecvPacket,iRecvPacket+1);
																	}
																	*/
					NFConnection* lpClientConn = (NFConnection*)lpOverlapPlus->pClientConn;
					lpClientConn->DoIo(lpOverlapPlus, m_pUpdateManager);				// IOCP 처리 핸들링 
				}
			}
			else
			{
				char errorMsg[1024] = { 0, };
				WSAGetLastErrorToTextMessage(errorMsg);

				if (!lpOverlapPlus)
				{
					//LOG_ERROR((L"Critical Error on GetQueuedCompletionStatus()."));
				}
				else
				{
					// 처리 변수 Cast 변환
					NFConnection* lpClientConn = (NFConnection*)lpOverlapPlus->pClientConn;
					// 강제로 Clear해주면 안된다. (데이타가 제대로 초기화 되지 않을수도 있다)
					//					lpClientConn->Clear();
					//					LOG_ERROR(("[%04d] IOCP OverlapPlus Error, Close_Open()호출. SOCKET_ERROR, %d", lpClientConn->GetIndex(), WSAGetLastError()));
					lpClientConn->SetClose_Open(lpOverlapPlus, m_pUpdateManager, true);	// 연결 해제
				}
			}
		}
	}

	//////////////////////////////////////////////////////////////////
	// [1]DESCRIPTION : IOCP처리를 담당하는 메인 스레드 			//
	// [2]PARAMETER : lpvoid - IOCP Handle							//
	// [3]RETURN : bool - 의미 없음				 					//
	// [4]DATE : 2003년 10월 24일									//
	//////////////////////////////////////////////////////////////////
	void NFServerCtrl::Process_MainEx()
	{
		while (IsRun())
		{
			if (IsPause())
			{
				Sleep(1);
				continue;
			}

			Update();
			Sleep(1);
		}
	}

	//////////////////////////////////////////////////////////////////
	// [1]DESCRIPTION : UpateQue처리를 담당하는 메인 스레드 		//
	// [2]PARAMETER : lpvoid - IOCP Handle							//
	// [3]RETURN : bool - 의미 없음				 					//
	// [4]DATE : 2003년 10월 24일									//
	//////////////////////////////////////////////////////////////////
	void NFServerCtrl::Packet_MainEx()
	{
		while (IsRun())
		{
			m_pUpdateManager->Update();
			Sleep(1);
		}
	}
	
	void NFServerCtrl::WSAGetLastErrorToTextMessage(char *pMsg)
	{
		LPVOID lpMsgBuf;
		auto errorCode = WSAGetLastError();

		FormatMessageA(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL, errorCode,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPSTR)&lpMsgBuf, 0, NULL);
		
		//printf("[%s] %s", pMsg, (char *)lpMsgBuf);
		LocalFree(lpMsgBuf);
	}
}