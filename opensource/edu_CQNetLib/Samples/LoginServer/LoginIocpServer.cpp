#include "../../CQNetLib/commonDef.h"
#include "../../CQNetLib/log.h"
#include "../Common/protocol.h"
#include "UserManager.h"
#include "ProcessClientPacket.h"
#include "ProcessServerPacket.h"
#include "LoginIocpServer.h"


#define GAME_LOG "GAME_LOG"
#define BASE_SERVER "BASE_SERVER"
#define CONNECT_LOGINDBSERVER "CONNECT_LOGINDBSERVER"
#define GAMESERVER_INFO	"GAMESERVER_INFO"
#define INIFILE_NAME ".\\LoginServer.ini"



LoginIocpServer::LoginIocpServer(void)
{
	m_pTickThread = new TickThread;
	m_dwPrivateKey = 0;
	m_nMaxGameServerConnCnt = 0;
	m_nMaxConnectionCnt = 0;
	ZeroMemory( m_nClientPacketSizes , sizeof( INT32  ) * MAX_PROCESSFUNC );

	m_bIsService = false;

	InitProcessFunc();

	m_GameServerInfos.clear();
}

LoginIocpServer::~LoginIocpServer(void)
{
	//서버 종료
	ServerOff();
	
	if( m_pTickThread )
	{
		m_pTickThread->DestroyThread();
		SAFE_DELETE( m_pTickThread );
	}
}

/*
Discription	: 패킷처리 함수 설정.
*/
void LoginIocpServer::InitProcessFunc()
{
	//클라이언트에서 받은 패킷
	m_FuncProcessClient[ CTL_LOGIN_RQ ].funcProcessClientPacket = ProcessClientPacket::funcCTL_PACKET_LOGIN_RQ;
	m_FuncProcessClient[ CTL_LOGIN_QUIT_FORCE_RQ ].funcProcessClientPacket = ProcessClientPacket::funcCTL_PACKET_LOGIN_QUIT_FORCE_RQ;
	m_FuncProcessClient[ CTL_DEV_LOGIN_CLOSE_RQ ].funcProcessClientPacket = ProcessClientPacket::funcCTL_PACKET_DEV_LOGIN_CLOSE_RQ;

	//클라이언트 패킷 사이즈 넣는다. 나중에 패킷 왔을때 비교하기 위해서.
	m_nClientPacketSizes[ CTL_LOGIN_RQ ] = sizeof( CTL_PACKET_LOGIN_RQ );
	m_nClientPacketSizes[ CTL_LOGIN_QUIT_FORCE_RQ ] = sizeof( CTL_PACKET_LOGIN_QUIT_FORCE_RQ );
	m_nClientPacketSizes[ CTL_DEV_LOGIN_CLOSE_RQ ] = sizeof( CTL_PACKET_DEV_LOGIN_CLOSE_RQ );


	//로긴 디비 서버에서 받은 패킷
	m_FuncProcessServer[ STD_LOGININFO_AQ ].funcProcessServerPacket = ProcessServerPacket::funcSTD_PACKET_LOGININFO_AQ;   


	//게임 서버에서 받은 패킷
	m_FuncProcessServer[ GTL_GAMESERVER_BASIC_INFO_CN ].funcProcessServerPacket = ProcessServerPacket::funcGTL_PACKET_GAMESERVER_BASIC_INFO_RQ;
	m_FuncProcessServer[ GTL_CLIENT_AUTH_CONFIRM_RQ ].funcProcessServerPacket = ProcessServerPacket::funcGTL_PACKET_CLIENT_AUTH_CONFIRM_RQ;
	m_FuncProcessServer[ GTL_CLIENT_GAME_AUTH_MSG ].funcProcessServerPacket = ProcessServerPacket::funcGTL_PACKET_CLIENT_GAME_AUTH_MSG;
	m_FuncProcessServer[ GTL_LOGOUT_CLIENT_MSG ].funcProcessServerPacket = ProcessServerPacket::funcLTG_PACKET_LOGOUT_CLIENT_MSG;
	m_FuncProcessServer[ GTL_CHANNEL_LIST_RQ ].funcProcessServerPacket = ProcessServerPacket::funcGTL_PACKET_CHANNEL_LIST_RQ;
	m_FuncProcessServer[ GTL_CHANGE_CHANNEL_MSG ].funcProcessServerPacket = ProcessServerPacket::funcGTL_PACKET_CHANGE_CHANNEL_MSG;
	m_FuncProcessServer[ GTL_LOBBY_CUR_USER_COUNT_MSG ].funcProcessServerPacket = ProcessServerPacket::funcGTL_PACKET_LOBBY_CUR_USER_COUNT_MSG;
}

/*
Discription	: client가 접속 수락이 되었을 때 호출되는 함수.
*/
bool LoginIocpServer::OnAccept( OUT CQNetLib::Session *pConnection )
{
    if(CQNetLib::CT_CLIENT == pConnection->GetConnectionType() )
	{
		User* pUser = (User*)pConnection;
		g_pUserManager->AddUser_Aysc( pUser );
		
		LOG(CQNetLib::LOG_INFO_HIGH ,
		"SYSTEM | cLoginIocpServer::OnAccept() | [CLIENT] IP[%s] Socket[%d] ConnKey(%d) 접속 UserCnt[%d]",
			pConnection->GetConnectionIp(), pConnection->GetSocket() ,pUser->GetConnKey(),
															g_pUserManager->GetUserCnt() );
	}
	else if( CQNetLib::CT_GAMESERVER == pConnection->GetConnectionType() )
	{
		//g_pGameServerManager->AddGameServer( (cGameServer*)pConnection);
		//LOG(CQNetLib::LOG_INFO_HIGH, "SYSTEM | cLoginIocpServer::OnAccept() | [GAMESERVER] IP[%s] Socket[%d] 접속 GameServerCnt[%d]", 	pConnection->GetConnectionIp(), lpConnection->GetSocket(), g_pGameServerManager->GetGameServerCnt() );
	}

	pConnection->SetServerTick( m_pTickThread->GetTickCount() );
	
	
	return true;
}

/*
Discription	: client와 연결이 종료되었을 때 호출되는 함수.
*/
void LoginIocpServer::OnClose(__deref_in CQNetLib::Session* pConnection)
{
	//cliet와 연결 끊김
	if( pConnection->GetConnectionType() == CQNetLib::CT_CLIENT )
	{
		g_pUserManager->RemoveUser_Aysc( (User*)pConnection );

		LOG(CQNetLib::LOG_INFO_HIGH ,
			"SYSTEM | cQLoginIocpServer::OnClose() | (CLOSE_CLIENT) SOCKET(%d) IP(%s) Current Connection Cnt(%d) 클라이언트 연결 종료"
			, pConnection->GetSocket() , pConnection->GetConnectionIp() , g_pUserManager->GetUserCnt() );
	}
	//server간 연결이 끊긴거라면..
	else
	{
		LOG(CQNetLib::LOG_ERROR_HIGH ,
			"SYSTEM | cQLoginIocpServer::OnClose() | (CLOSE_SERVER) SERVERTYPE(%d) IP(%s) SOCKET(%d)와의 서버연결 종료."
			, pConnection->GetConnectionType()
			, pConnection->GetConnectionIp(), pConnection->GetSocket()  );
				
		if( pConnection->GetConnectionType() == CQNetLib::CT_GAMESERVER )
		{
			// 2008. 08.20 최흥배
			// AddGameServerInfo 에서 다음에 또 추가 될 때는 덮어쉬우므로 여기서 삭제를 하지 않는다.
			// 게임서버 정보를 제거한다.
			//RemoveGameServerInfo( lpConnection->GetIndex() );
			
			//g_pGameServerManager->RemoveGameServer( (cGameServer*)lpConnection );
			RemoveGameServerInfo( pConnection->GetIndex() );
		}
	}
}

/*
Discription	: client에서 packet이 도착했을 때 순서 성 있게 처리되어지는 패킷처리.
*/
bool LoginIocpServer::OnRecv(__deref_in CQNetLib::Session* pConnection,  __in DWORD dwSize, __deref_in char* pRecvedMsg)
{
	unsigned short usType;
	CopyMemory( &usType , pRecvedMsg + PACKET_SIZE_LENGTH , PACKET_TYPE_LENGTH );
	auto CType = pConnection->GetConnectionType();

	//Client에서 도착한 패킷이라면
	if( CType == CQNetLib::CT_CLIENT )
	{
		User* pUser = (User*)pConnection;
		if( usType < 0 || usType > MAX_PROCESSFUNC || NULL == m_FuncProcessClient[ usType ].funcProcessClientPacket )
		{
			LOG(CQNetLib::LOG_ERROR_NORMAL , "SYSTEM | cLoginIocpServer::OnRecv() | 정의되지 않은 클라이언트 패킷(%d) UKey(%d) Socket(%d)",
				usType , pUser->GetUKey() , pUser->GetSocket() );
			return true;
		}
		//받은 패킷 사이즈 확인
		if( m_nClientPacketSizes[ usType ] != static_cast<INT32 >(dwSize) )
		{
			LOG(CQNetLib::LOG_ERROR_NORMAL , "SYSTEM | cLoginIocpServer::OnRecv() | 패킷 사이즈 오류 , SOCKET ID(%d) TYPE(%d) 원래 패킷(%d) 받은 패킷(%d)",
				pUser->GetSocket() , usType , m_nClientPacketSizes[ usType ] , dwSize );
			//연결을 종료 시킨다.
			CloseConnection( pConnection );
			return true;
		}

		if (pUser->GetConnKey() == 0) {
			return true;
		}

		pUser->SetServerTick( m_pTickThread->GetTickCount() );
		m_FuncProcessClient[ usType ].funcProcessClientPacket( pUser , dwSize , pRecvedMsg );
	}
	//Server에서 도착한 패킷이라면
	else
	{
		if( usType < 0 || usType > MAX_PROCESSFUNC || NULL == m_FuncProcessServer[ usType ].funcProcessServerPacket )
		{
			LOG(CQNetLib::LOG_ERROR_NORMAL , "SYSTEM | cLoginIocpServer::OnRecv() | 정의되지 않은 서버 패킷(%d) ServerType(%d)",
				usType  , (INT32 )pConnection->GetConnectionType() );
			return true;
		}
		m_FuncProcessServer[ usType ].funcProcessServerPacket( pConnection , dwSize , pRecvedMsg );
	}
	return true;
}

/*
Discription	: client에서 packet이 도착했을 때 순서 성 없이 곧바로 처리 되는 패킷처리.
*/
bool LoginIocpServer::OnRecvImmediately(__deref_in CQNetLib::Session* pConnection, __in DWORD dwSize, __deref_in char* pRecvedMsg)
{
	//패킷이 처리되면 return true; 처리 되지 않았다면 return false;
	return false;
}

bool LoginIocpServer::OnSystemMsg( __deref_in void* pOwnerKey, __in DWORD dwMsgType, __in LPARAM lParam ) 
{ 
	eLoginServerSystemMsg typeSystemMsg = (eLoginServerSystemMsg)dwMsgType;
	switch( typeSystemMsg )
	{
	case SYSTEM_REMOVE_AUTH_USER:
		{
			DWORD dwUKey = (DWORD)pOwnerKey;
			g_pUserManager->RemoveAuthUser( true, false, dwUKey );
		}
		break;
		
	case SYSTEM_CHECK_AUTH_USER:
		{
			g_pUserManager->CheckAuthUsers();
		}
		break;

	 default:
		{
			LOG(CQNetLib::LOG_ERROR_NORMAL , "SYSTEM | cLoginIocpServer::OnSystemMsg() | 정의되지 않은 시스템 메시지(%d)",
				typeSystemMsg );
		}
		break;
	}

	return true;
}

/*
Description	 : 패킷처리 쓰레드에 패킷을 등록한다.
TickThread와 패킷처리 스레드 간애 동기화가 필요 패킷 스레드쪽에서 처리하도록 하기
위해 패킷처리 스레드쪽으로 패킷을 등록 하도록 한다.
*/
void LoginIocpServer::ProcessSystemMsg( void* pOwnerKey , DWORD dwMsgType , LPARAM lParam )
{
	auto lpProcessPacket = GetProcessPacket(CQNetLib::OP_SYSTEM , (LPARAM)dwMsgType , lParam );
	if( NULL == lpProcessPacket ) {
		return;
	}

	if( 0 == PostQueuedCompletionStatus( m_hProcessIOCP, 0, (ULONG_PTR)pOwnerKey, (LPOVERLAPPED)lpProcessPacket ) )
	{
		ClearProcessPacket( lpProcessPacket );

		LOG(CQNetLib::LOG_ERROR_NORMAL, "SYSTEM | cLoginIocpServer::ProcessSystemMsg() | PostQueuedCompletionStatus Failed : [%d], pOwnerKey[%d]", GetLastError() , pOwnerKey );
	}
}

/*
Discription	: 서버 시작.
*/
bool LoginIocpServer::ServerStart()
{
	CQNetLib::INITCONFIG initConfig;
	
	char		szOutStr[1024];
	CTime time = CTime::GetCurrentTime();

	if( -1 == GetINIString( m_szLogFileName , BASE_SERVER  , "LOGFILE"  ,  100 , INIFILE_NAME ) )
		return false;
	if( -1 == ( initConfig.nProcessPacketCnt = GetINIInt( BASE_SERVER  , "PROCESS_PACKET_CNT"  , INIFILE_NAME ) ) )
		return false;
	if( -1 == ( initConfig.nSendBufCnt = GetINIInt( BASE_SERVER  , "SEND_BUFFER_CNT"  , INIFILE_NAME ) ) )
		return false;
	if( -1 == ( initConfig.nRecvBufCnt = GetINIInt( BASE_SERVER  , "RECV_BUFFER_CNT"  , INIFILE_NAME ) ) )
		return false;
	if( -1 == ( initConfig.nSendBufSize = GetINIInt( BASE_SERVER  , "SEND_BUFFER_SIZE"  , INIFILE_NAME ) ) )
		return false;
	if( -1 == ( initConfig.nRecvBufSize = GetINIInt( BASE_SERVER  , "RECV_BUFFER_SIZE"  , INIFILE_NAME ) ) )
		return false;
	if( -1 == ( m_nMaxConnectionCnt = GetINIInt( BASE_SERVER  , "MAX_CONNECTION_CNT"  , INIFILE_NAME ) ) )
		return false;
	if( -1 == ( m_nMaxGameServerConnCnt = GetINIInt( BASE_SERVER  , "MAX_SERVERCONN_CNT"  , INIFILE_NAME ) ) )
		return false;
	if( -1 == ( initConfig.nClientPort = GetINIInt( BASE_SERVER  , "START_CLIENT_PORT"  , INIFILE_NAME ) ) )
		return false;
	if( -1 == ( initConfig.nServerPort = GetINIInt( BASE_SERVER  , "START_SERVER_PORT"  , INIFILE_NAME ) ) )
		return false;
	if( -1 == ( initConfig.nWorkerThreadCnt = GetINIInt( BASE_SERVER  , "IO_WORKER_THREAD"  , INIFILE_NAME ) ) )
		return false;
	if( -1 == ( initConfig.nProcessThreadCnt = GetINIInt( BASE_SERVER  , "PROCESS_WORKER_THREAD"  , INIFILE_NAME ) ) )
		return false;
	if( -1 == ( initConfig.nLogInfoLevelWindow = GetINIInt( GAME_LOG  , "LOG_INFO_WINDOW"  , INIFILE_NAME ) ) )
		return false;
	if( -1 == ( initConfig.nLogErrorLevelWindow = GetINIInt( GAME_LOG  , "LOG_ERROR_WINDOW"  , INIFILE_NAME ) ) )
		return false;
	if( -1 == ( initConfig.nLogInfoLevelFile = GetINIInt( GAME_LOG  , "LOG_INFO_FILE"  , INIFILE_NAME ) ) )
		return false;
	if( -1 == ( initConfig.nLogErrorLevelFile = GetINIInt( GAME_LOG  , "LOG_ERROR_FILE"  , INIFILE_NAME ) ) )
		return false;
	if( -1 == ( initConfig.nLogInfoLevelDebugWnd = GetINIInt( GAME_LOG  , "LOG_INFO_DEBUGWND"  , INIFILE_NAME ) ) )
		return false;
	if( -1 == ( initConfig.nLogErrorLevelDebugWnd = GetINIInt( GAME_LOG  , "LOG_ERROR_DEBUGWND"  , INIFILE_NAME ) ) )
		return false;

	//로그 초기화
	if( !CHECK_RANGE( initConfig.nLogInfoLevelDebugWnd , 1 , MAX_LOGTYPE ) ||
		!CHECK_RANGE( initConfig.nLogInfoLevelWindow , 1 , MAX_LOGTYPE ) ||
		!CHECK_RANGE( initConfig.nLogInfoLevelFile , 1 , MAX_LOGTYPE ) ||
		!CHECK_RANGE( initConfig.nLogErrorLevelDebugWnd , 1 , MAX_LOGTYPE ) ||
		!CHECK_RANGE( initConfig.nLogErrorLevelWindow , 1 , MAX_LOGTYPE ) ||
		!CHECK_RANGE( initConfig.nLogErrorLevelFile , 1 , MAX_LOGTYPE ) )
	{
		MessageBox( NULL , "INI의 로그 레벨이 1~4범위안에 있어야 합니다." , "에러", MB_OK );
		return false;
	}

	sLogConfig LogConfig;
	strncpy( LogConfig.s_szLogFileName , m_szLogFileName , MAX_FILENAME_LENGTH );
	LogConfig.s_nLogInfoLevelByTypes[STORAGE_DEBUGWND ] = initConfig.nLogInfoLevelDebugWnd;
	LogConfig.s_nLogInfoLevelByTypes[ STORAGE_WINDOW ] = initConfig.nLogInfoLevelWindow;//LOG_INFO_HIGH | LOG_INFO_CRITICAL | LOG_ERROR_ALL;
    LogConfig.s_nLogInfoLevelByTypes[ STORAGE_FILE ] = initConfig.nLogInfoLevelFile;
	LogConfig.s_nLogErrorLevelByTypes[ STORAGE_DEBUGWND ] = initConfig.nLogErrorLevelDebugWnd;
	LogConfig.s_nLogErrorLevelByTypes[ STORAGE_WINDOW ] = initConfig.nLogErrorLevelWindow;//LOG_INFO_HIGH | LOG_INFO_CRITICAL | LOG_ERROR_ALL;
    LogConfig.s_nLogErrorLevelByTypes[ STORAGE_FILE ] = initConfig.nLogErrorLevelFile;
	LogConfig.s_hWnd = AfxGetMainWnd()->m_hWnd;
	INIT_LOG( LogConfig );
	
	LOG( LOG_INFO_CRITICAL , "SYSTEM | 로그 시스템 시작 | ====================================================================== ");
	LOG( LOG_INFO_CRITICAL , "SYSTEM | cLoginIocpServer::ServerStart() | [Base_LogFileName] %s", m_szLogFileName );
	LOG( LOG_INFO_CRITICAL , "SYSTEM | cLoginIocpServer::ServerStart() | [Base_ProcessPacket] ProcessPacket 개수 : %d", initConfig.nProcessPacketCnt );
	LOG( LOG_INFO_CRITICAL , "SYSTEM | cLoginIocpServer::ServerStart() | [Base_Buffer] Send Buffer 개수 : %d ", initConfig.nSendBufCnt );
	LOG( LOG_INFO_CRITICAL , "SYSTEM | cLoginIocpServer::ServerStart() | [Base_Buffer] Recv Buffer 개수 : %d ", initConfig.nRecvBufCnt );
	LOG( LOG_INFO_CRITICAL , "SYSTEM | cLoginIocpServer::ServerStart() | [Base_Buffer] Send Buffer 크기 : %d ", initConfig.nSendBufSize );
	LOG( LOG_INFO_CRITICAL , "SYSTEM | cLoginIocpServer::ServerStart() | [Base_Buffer] Recv Buffer 크기 : %d ", initConfig.nRecvBufSize );
	LOG( LOG_INFO_CRITICAL , "SYSTEM | cLoginIocpServer::ServerStart() | [Base_Buffer] Send 총 할당 버퍼: %d ", initConfig.nSendBufSize * initConfig.nSendBufCnt );
	LOG( LOG_INFO_CRITICAL , "SYSTEM | cLoginIocpServer::ServerStart() | [Base_Buffer] Recv 총 할당 버퍼: %d ", initConfig.nRecvBufSize * initConfig.nRecvBufCnt );
	LOG( LOG_INFO_CRITICAL , "SYSTEM | cLoginIocpServer::ServerStart() | [Base_Connection] 접속할수 있는 최대 수 : %d", m_nMaxConnectionCnt );
	LOG( LOG_INFO_CRITICAL , "SYSTEM | cLoginIocpServer::ServerStart() | [Base_Connection] Client Binding Port : %d", initConfig.nClientPort );
	LOG( LOG_INFO_CRITICAL , "SYSTEM | cLoginIocpServer::ServerStart() | [Base_Connection] Server Binding Port : %d", initConfig.nServerPort );
	LOG( LOG_INFO_CRITICAL , "SYSTEM | cLoginIocpServer::ServerStart() | [Base_Thread] I/O WorkerThread Cnt : %d", initConfig.nWorkerThreadCnt );
	LOG( LOG_INFO_CRITICAL , "SYSTEM | cLoginIocpServer::ServerStart() | [Base_Thread] Process WorkerThread Cnt : %d", initConfig.nProcessThreadCnt );
	LOG( LOG_INFO_CRITICAL , "SYSTEM | cLoginIocpServer::ServerStart() | [Base_Log] Debug Window Info Log Level : %s", szLogInfoType_StringTable[ initConfig.nLogInfoLevelDebugWnd ] );
	LOG( LOG_INFO_CRITICAL , "SYSTEM | cLoginIocpServer::ServerStart() | [Base_Log] Debug Window Error Log Level : %s", szLogInfoType_StringTable[ initConfig.nLogErrorLevelDebugWnd + LOG_ERROR_STARTIDX ] );
	LOG( LOG_INFO_CRITICAL , "SYSTEM | cLoginIocpServer::ServerStart() | [Base_Log] MFC Window Info Log Level : %s", szLogInfoType_StringTable[ initConfig.nLogInfoLevelWindow ] );
	LOG( LOG_INFO_CRITICAL , "SYSTEM | cLoginIocpServer::ServerStart() | [Base_Log] MFC Window Error Log Level : %s", szLogInfoType_StringTable[ initConfig.nLogErrorLevelWindow + LOG_ERROR_STARTIDX ] );
	LOG( LOG_INFO_CRITICAL , "SYSTEM | cLoginIocpServer::ServerStart() | [Base_Log] File Info Log Level : %s", szLogInfoType_StringTable[ initConfig.nLogInfoLevelFile ] );
	LOG( LOG_INFO_CRITICAL , "SYSTEM | cLoginIocpServer::ServerStart() | [Base_Log] File Error Log Level : %s", szLogInfoType_StringTable[ initConfig.nLogErrorLevelFile + LOG_ERROR_STARTIDX ] );
	
	LOG( LOG_INFO_CRITICAL , "SYSTEM | cLoginIocpServer::ServerStart() | LoginServer Start.." );
	LOG( LOG_INFO_CRITICAL , "SYSTEM | cLoginIocpServer::ServerStart() | 서버 시작 시간 :  %d년%d월%d일%d시%d분%d초",
		time.GetYear(),time.GetMonth(),time.GetDay(), time.GetHour(),time.GetMinute(),time.GetSecond() );

	sprintf(szOutStr, "LoginServer [Start - %d년%d월%d일%d시%d분%d초]"
		,time.GetYear(),time.GetMonth(),time.GetDay(), time.GetHour(),time.GetMinute(),time.GetSecond());

	
	theApp.m_pMainWnd->SetWindowText( _T( szOutStr ) );
	

	if( false == cIocpServer::ServerStart( initConfig ) )
		return false;
	if( false == g_pUserManager->CreateUser( initConfig , m_nMaxConnectionCnt ) )
		return false;

	// 서버연결에대한 정보를 얻어온다.
	ReadGameServerInfo( initConfig );
	// 서버연결 포트 설정
	initConfig.sockListener = GetServerListenSocket();
	if( false == g_pGameServerManager->CreateGameServer( initConfig , m_nMaxGameServerConnCnt ) )
		return false;

	//0.5 tick으로 thread 시작
	m_pTickThread->CreateThread( CHECKTIME_SERVERTICK );
	m_pTickThread->Run();
	
	SetCurTime();

	//서버 연결 관리자 시작
	m_ServerConnectionManager.ConnectToServer(CT_LOGINDBSERVER);
	m_ServerConnectionManager.Start(5000);

	CreateServerConnection();
	g_pServerConnectionManager->Start( 5000 );

	GetBridgeCimmuni()->SetAppServerWnd( theApp.m_pMainWnd->m_hWnd );

	TEST();

	return true;
}

/*
Discription	:  게임 서버 접속정보를 ini파일에서 읽어온다.
*/
bool LoginIocpServer::ReadGameServerInfo( __inout INITCONFIG& initConfig )
{
	//접속할 dbserver에 대한 정보를 얻어온다.
	if( -1 == ( initConfig.nProcessPacketCnt = GetINIInt( GAMESERVER_INFO  , "PROCESS_PACKET_CNT"  , INIFILE_NAME ) ) )
		return false;
	if( -1 == ( initConfig.nSendBufCnt = GetINIInt( GAMESERVER_INFO  , "SEND_BUFFER_CNT"  , INIFILE_NAME ) ) )
		return false;
	if( -1 == ( initConfig.nRecvBufCnt = GetINIInt( GAMESERVER_INFO  , "RECV_BUFFER_CNT"  , INIFILE_NAME ) ) )
		return false;
	if( -1 == ( initConfig.nSendBufSize = GetINIInt( GAMESERVER_INFO  , "SEND_BUFFER_SIZE"  , INIFILE_NAME ) ) )
		return false;
	if( -1 == ( initConfig.nRecvBufSize = GetINIInt( GAMESERVER_INFO  , "RECV_BUFFER_SIZE"  , INIFILE_NAME ) ) )
		return false;
	
	LOG( LOG_INFO_CRITICAL , "SYSTEM | cLoginIocpServer::ReadGameServerInfo() | [GameServerConn_ProcessPacket] ProcessPacket 개수 : %d", initConfig.nProcessPacketCnt );
	LOG( LOG_INFO_CRITICAL , "SYSTEM | cLoginIocpServer::ReadGameServerInfo() | [GameServerConn_Buffer] Send Buffer 갯수 : %d ", initConfig.nSendBufCnt );
	LOG( LOG_INFO_CRITICAL , "SYSTEM | cLoginIocpServer::ReadGameServerInfo() | [GameServerConn_Buffer] Recv Buffer 갯수 : %d ", initConfig.nRecvBufCnt );
	LOG( LOG_INFO_CRITICAL , "SYSTEM | cLoginIocpServer::ReadGameServerInfo() | [GameServerConn_Buffer] Send Buffer 크기 : %d ", initConfig.nSendBufSize );
	LOG( LOG_INFO_CRITICAL , "SYSTEM | cLoginIocpServer::ReadGameServerInfo() | [GameServerConn_Buffer] Recv Buffer 크기 : %d ", initConfig.nRecvBufSize );
	LOG( LOG_INFO_CRITICAL , "SYSTEM | cLoginIocpServer::ReadGameServerInfo() | [GameServerConn_Buffer] Send 총 할당 버퍼: %d ", initConfig.nSendBufSize * initConfig.nSendBufCnt );
	LOG( LOG_INFO_CRITICAL , "SYSTEM | cLoginIocpServer::ReadGameServerInfo() | [GameServerConn_Buffer] Recv 총 할당 버퍼: %d ", initConfig.nRecvBufSize * initConfig.nRecvBufCnt );
	return true;

}

/*
Discription	: INI에서 서버 연결 정보를 읽어와 서버 연결을 생성한다.
*/
bool LoginIocpServer::CreateServerConnection()
{
	INITCONFIG initConfig;
	
	//접속할 LoginDBServer에 대한 정보를 얻어온다.
	if( -1 == ( initConfig.nProcessPacketCnt = GetINIInt( CONNECT_LOGINDBSERVER  , "PROCESS_PACKET_CNT"  , INIFILE_NAME ) ) )
		return false;
	if( -1 == ( initConfig.nSendBufCnt = GetINIInt( CONNECT_LOGINDBSERVER  , "SEND_BUFFER_CNT"  , INIFILE_NAME ) ) )
		return false;
	if( -1 == ( initConfig.nRecvBufCnt = GetINIInt( CONNECT_LOGINDBSERVER  , "RECV_BUFFER_CNT"  , INIFILE_NAME ) ) )
		return false;
	if( -1 == ( initConfig.nSendBufSize = GetINIInt( CONNECT_LOGINDBSERVER  , "SEND_BUFFER_SIZE"  , INIFILE_NAME ) ) )
		return false;
	if( -1 == ( initConfig.nRecvBufSize = GetINIInt( CONNECT_LOGINDBSERVER  , "RECV_BUFFER_SIZE"  , INIFILE_NAME ) ) )
		return false;
	//if( -1 == ( initConfig.nConnectionPool = GetINIInt( CONNECT_LOGINDBSERVER  , "CONNECTION_POOL"  , INIFILE_NAME ) ) )
	//	return false;
	if( -1 == GetINIString( initConfig.szIP , CONNECT_LOGINDBSERVER  , "CONNECT_IP"  ,  100 , INIFILE_NAME ) )
		return false;
	if( -1 == ( initConfig.nServerPort = GetINIInt( CONNECT_LOGINDBSERVER  , "CONNECT_PORT"  , INIFILE_NAME ) ) )
		return false;
	initConfig.ConnectionType = CT_LOGINDBSERVER;
	LOG( LOG_INFO_CRITICAL , "SYSTEM | cLoginIocpServer::ConnectToLoginDBServer() | [LoginDBServerConn_ProcessPacket] ProcessPacket 개수 : %d", initConfig.nProcessPacketCnt );
	LOG( LOG_INFO_CRITICAL , "SYSTEM | cLoginIocpServer::ConnectToLoginDBServer() | [LoginDBServerConn_Buffer] Send Buffer 갯수 : %d ", initConfig.nSendBufCnt );
	LOG( LOG_INFO_CRITICAL , "SYSTEM | cLoginIocpServer::ConnectToLoginDBServer() | [LoginDBServerConn_Buffer] Recv Buffer 갯수 : %d ", initConfig.nRecvBufCnt );
	LOG( LOG_INFO_CRITICAL , "SYSTEM | cLoginIocpServer::ConnectToLoginDBServer() | [LoginDBServerConn_Buffer] Send Buffer 크기 : %d ", initConfig.nSendBufSize );
	LOG( LOG_INFO_CRITICAL , "SYSTEM | cLoginIocpServer::ConnectToLoginDBServer() | [LoginDBServerConn_Buffer] Recv Buffer 크기 : %d ", initConfig.nRecvBufSize );
	LOG( LOG_INFO_CRITICAL , "SYSTEM | cLoginIocpServer::ConnectToLoginDBServer() | [LoginDBServerConn_Buffer] Send 총 할당 버퍼: %d ", initConfig.nSendBufSize * initConfig.nSendBufCnt );
	LOG( LOG_INFO_CRITICAL , "SYSTEM | cLoginIocpServer::ConnectToLoginDBServer() | [LoginDBServerConn_Buffer] Recv 총 할당 버퍼: %d ", initConfig.nRecvBufSize * initConfig.nRecvBufCnt );
	LOG( LOG_INFO_CRITICAL , "SYSTEM | cLoginIocpServer::ConnectToLoginDBServer() | [LoginDBServerConn_Connection] Connect Ip : %s", initConfig.szIP );
	LOG( LOG_INFO_CRITICAL , "SYSTEM | cLoginIocpServer::ConnectToLoginDBServer() | [LoginDBServerConn_Connection] Connect Port : %d", initConfig.nServerPort );
	
	// 로그인 DB 서버에 접속 할 정보를 설정한다. 
	//* 여기서 연결되는 것은 아니다. 실제 연결은 메뉴의 'Connect -> LoginDBServer'를 선택해야 진짜 연결된다.
	cConnection LoginDBServerConn;
	LoginDBServerConn.SetInitConfig( initConfig );
	LoginDBServerConn.SetConnectionType( initConfig.ConnectionType );
	if( false == g_pServerConnectionManager->CreateConnection( &LoginDBServerConn ) )
	{
		LOG( LOG_ERROR_CRITICAL, 
			"SYSTEM | cLoginIocpServer::CreateServerConnection() | Connect LoginDBServer Failed : IP[%s], Port[%d]" 
			,initConfig.szIP, initConfig.nServerPort );
	}

	return true;
}

/*
Discription	: 게임서버의 정보를 저장한다.
Parameters	:	tGameServerInfo		- 게임서버의 정보
*/
void LoginIocpServer::AddGameServerInfo( IN LOGIN_GAMESERVER_INFO tGameServerInfo )
{
	INT32  iGameServerCount = static_cast<INT32 >(m_GameServerInfos.size());
	for( INT32  i = 0; i < iGameServerCount; ++i )
	{
		// 기존에 등록 되어 있다면 기존의 것은 제거 한다.
		if( tGameServerInfo.sSeverIndex == m_GameServerInfos[i].sSeverIndex )
		{
			vector<LOGIN_GAMESERVER_INFO>::iterator iterPos = m_GameServerInfos.begin() + i;
			m_GameServerInfos.erase(iterPos);

			LOG( LOG_INFO_CRITICAL , "SYSTEM | cLoginIocpServer::AddGameServerInfo() | 게임서버 중복으로 기존 것은 제거 하였습니다.: Index(%d)", 
								tGameServerInfo.sSeverIndex );
			break;
		}
	}

	m_GameServerInfos.push_back(tGameServerInfo);
}

// 게임서버는 연결 중인가?
bool LoginIocpServer::IsConnectedGameServer( LOGIN_GAMESERVER_INFO& tGameServer )
{
	if( NULL == tGameServer.pConnection || INVALID_SOCKET == tGameServer.pConnection->GetSocket()) 
	{
		return false;
	}

	return true;
}

/*
Description	: 게임서버 정보를 제거한다.
Parameters	:	iConnectionIndex	- cConnnection 객체의 index
*/
void LoginIocpServer::RemoveGameServerInfo( __in INT32  iConnectionIndex )
{
	bool bDoRemove = false;
	vector<LOGIN_GAMESERVER_INFO>::iterator iterPos;

	INT32  iGameServerCount = static_cast<INT32 >(m_GameServerInfos.size());
	for( INT32  i = 0; i < iGameServerCount; ++i )
	{
		if( iConnectionIndex == m_GameServerInfos[i].pConnection->GetIndex() )
		{
			LOG( LOG_INFO_HIGH, "SYSTEM | cLoginIocpServer::RemoveGameServerInfo() | ConnectionIndex(%d), ServerIndex(%d)", 
																			iConnectionIndex, m_GameServerInfos[i].sSeverIndex );
			bDoRemove = true;
			iterPos = m_GameServerInfos.begin() + i;
			break;
		}
	}


	if( bDoRemove )
	{
		m_GameServerInfos.erase(iterPos);
	}
}

/*
Discription	:게임서버 인덱스로 연결 중인 게임서버 컨넥트 반환.
Parameters	:	iConnectionIndex	- cConnnection 객체의 index
Return		: 로그인 서버에 연결된 게임서버의 cConnection 객체를 반환.
*/
cConnection* LoginIocpServer::GetGameServerConnection( __in INT32  iGameServerIndex )
{
	INT32  iGameServerCount = static_cast<INT32 >(m_GameServerInfos.size());
	for( INT32  i = 0; i < iGameServerCount; ++i )
	{
		if( iGameServerIndex == m_GameServerInfos[i].sSeverIndex )
		{
			if( false == IsConnectedGameServer( m_GameServerInfos[i] ) ) 
			{
				return NULL;
			}

			return m_GameServerInfos[i].pConnection;
		}
	}

	return NULL;
}

/*
Discription	: 유저의 게임서버 인증 완료 및 인증만료 처리를 한다.
Parameters	:	iConnectIndex		- cConnection 객체의 인덱스
			    bLogIn				- 로그인 시(true), 로그아웃 시(false)
			    dwUKey				- 유저 키
			    iCharCd				- 캐릭터 코드
			    sGameServerIndex	- 게임서버 인덱스 
			    sChannelNum			- 채널 번호
*/
void LoginIocpServer::SetUserGameServerAuth( __in INT32  iConnectIndex, __in bool bLogIn, __in UINT64 i64UKey, __in UINT64 i64CharCd, 
											 __in short sGameServerIndex )
{
	/*
	bool bSucceed = false;
	INT32  iGameServerCount = static_cast<INT32 >( m_GameServerInfos.size() );
	for( INT32  i = 0; i < iGameServerCount; ++i )
	{
		if( sGameServerIndex == m_GameServerInfos[i].sSeverIndex )
		{
			if( sChannelNum < 0 || sChannelNum >= m_GameServerInfos[i].sChannelCount ) {
				break;
			}

			bSucceed = true;
			
			if( bLogIn ) 
			{
				++m_GameServerInfos[i].asCurChannelUserCount[sChannelNum];
			} 
			else 
			{
				if( m_GameServerInfos[i].asCurChannelUserCount[sChannelNum] > 0 ) {
					--m_GameServerInfos[i].asCurChannelUserCount[sChannelNum];
				}
			}

			LOG( LOG_INFO_LOW, "SYSTEM | cLoginIocpServer::SetUserGameServerAuth() | UKey(%I64d), ChannelNum(%d), ChannelUserCount(%d)", 
							i64UKey, sChannelNum, m_GameServerInfos[i].asCurChannelUserCount[sChannelNum] );
			break;
		}
	}

	if( false == bSucceed )
	{
		LOG( LOG_ERROR_HIGH, "SYSTEM | cLoginIocpServer::SetUserGameServerAuth() | ServerIndex(%d), ChannelNum(%d)",
			sGameServerIndex, sChannelNum );
	}
	*/

	
	if( bLogIn )
	{
		AuthenticUser* ptAuthUser = g_pUserManager->GetAuthUser(i64UKey);
		if( NULL != ptAuthUser ) 
		{
			ptAuthUser->pkConnection = NULL;
			ptAuthUser->bGameServerConnect = true;
			ptAuthUser->iGameServerConnectIndex = iConnectIndex;
			ptAuthUser->sGameServerIndex = sGameServerIndex;
			ptAuthUser->i64CharCd = i64CharCd;
			ptAuthUser->uiChangeGamseServerSecondTime = 0;
		}
		else 
		{
			LOG( LOG_ERROR_HIGH, "SYSTEM | cLoginIocpServer::SetUserGameServerAuth() | UKey(%I64d)", i64UKey );
		}
	}
	else
	{
		g_pUserManager->RemoveAuthUser( false, false, i64UKey );
	}
}

// 게임 서버 채널의 유저 수
void LoginIocpServer::ChannelUserCount( __in short sGameServerIndex, __in short sChannelNum, __in short UserCount )
{
	INT32  iGameServerCount = static_cast<INT32 >( m_GameServerInfos.size() );
	for( INT32  i = 0; i < iGameServerCount; ++i )
	{
		if( sGameServerIndex == m_GameServerInfos[i].sSeverIndex )
		{
			if( sChannelNum < 0 || sChannelNum >= m_GameServerInfos[i].sChannelCount ) {
				LOG( LOG_ERROR_HIGH, "SYSTEM | cLoginIocpServer::ChannelUserCount() | GameServerIndex(%d), ChannelNum(%d), UserCount(%d)", 
							sGameServerIndex, sChannelNum, UserCount );	
				break;
			}

			m_GameServerInfos[i].asCurChannelUserCount[sChannelNum] = UserCount;
			
			LOG( LOG_INFO_LOW, "SYSTEM | cLoginIocpServer::ChannelUserCount() | GameServerIndex(%d), ChannelNum(%d), UserCount(%d)", 
							sGameServerIndex, sChannelNum, UserCount );
			break;
		}
	}
}

/*
Description	: 인증된 유저가 다른 게임서버에 접속 시작 시간을 설정.
Parameters	:	dwUKey			- 유저 키
*/
void LoginIocpServer::SetAuthUserChangeGameServerSecondTime( __in UINT64 i64UKey )
{
	AuthenticUser* ptAuthUser = g_pUserManager->GetAuthUser(i64UKey);
	if( NULL != ptAuthUser ) 
	{
		UINT32  uiSecondTime = GetCurSecondTime();
		ptAuthUser->uiChangeGamseServerSecondTime = uiSecondTime;
LOG( LOG_INFO_LOW, "SYSTEM | cLoginIocpServer::SetAuthUserChangeGameServerSecondTime() | UKey(%d), SecondTime(%I64d)", i64UKey, uiSecondTime );
	}
	else 
	{
		LOG( LOG_ERROR_HIGH, "SYSTEM | cLoginIocpServer::SetAuthUserChangeGameServerSecondTime() | UKey(%I64d)", i64UKey );
	}
}

/*
Description	: 채널의 정보를 클라이언트에 통보 한다.
Parameters	:	pkConnection		- 유저 객체
*/
void LoginIocpServer::SendChannelInfo( __in short PacketInxex, __deref_in cConnection* pkConnection )
{
	g_pVBuffer->Init();
	g_pVBuffer->SetShort( PacketInxex );

	short sChannelCount = 0;
	char* pcChannelCnt = g_pVBuffer->GetCurMark();
	g_pVBuffer->SetShort( sChannelCount );

	INT32  iServerChannelCount = 0;
	short sServerIndex = 0;
	INT32  iGameServerCount = static_cast<INT32 >(m_GameServerInfos.size());
	for( INT32  iServer = 0; iServer < iGameServerCount; ++iServer )
	{
		// 연결이 끊어진 서버라면 무시한다.
		if( false == IsConnectedGameServer( m_GameServerInfos[iServer] ) ) 
		{
			LOG( LOG_INFO_LOW, "LOGINSERVER | cLoginIocpServer::SendChannelInfo() | Close GameServer(%d, %s)", m_GameServerInfos[iServer].sSeverIndex, m_GameServerInfos[iServer].acIP);
			continue;
		}

		sServerIndex = m_GameServerInfos[iServer].sSeverIndex;
		iServerChannelCount = m_GameServerInfos[iServer].sChannelCount;
		for( INT32  iChannel = 0; iChannel < iServerChannelCount; ++iChannel )
		{
			g_pVBuffer->SetShort( sServerIndex );
			g_pVBuffer->SetShort( m_GameServerInfos[iServer].asMaxChannelUserCount[iChannel] - MAX_RESERVED_CHANNEL_USER_COUNT );
			g_pVBuffer->SetShort( m_GameServerInfos[iServer].asCurChannelUserCount[iChannel] );
			
			++sChannelCount;
		}
	}

	CopyMemory( pcChannelCnt, &sChannelCount, sizeof( short ) );


	// 서버 주소 정보
	short sServerCount = 0;
	char* pcServerCount = g_pVBuffer->GetCurMark();
	g_pVBuffer->SetShort( sServerCount );
	for( INT32  iServer = 0; iServer < iGameServerCount; ++iServer )
	{
		// 연결이 끊어진 서버라면 무시한다.
		if( false == IsConnectedGameServer( m_GameServerInfos[iServer] ) ) 
		{
			continue;
		}

		sServerIndex = m_GameServerInfos[iServer].sSeverIndex;

		g_pVBuffer->SetShort( sServerIndex );
		g_pVBuffer->SetString( m_GameServerInfos[iServer].acIP );
		g_pVBuffer->SetShort( m_GameServerInfos[iServer].sPort );

		++sServerCount;
	}

	CopyMemory( pcServerCount, &sServerCount, sizeof( short ) );

	pkConnection->SendVBuffer();
}

/*
Discription	: 채널의 정보를 게임서버에 통보 한다.
Parameters	:	pkConnection		- cConnection 객체(게임서버)
			    iCharCd				- 캐릭터 코드
			    sCurChannelNum		- 채널 번호
			    sSeqCdInLobby		- 로비에서의 시퀸스 번호
*/
void LoginIocpServer::SendChannelInfo( __deref_in CQNetLib::Session* pkConnection, __in UINT64 i64CharCd, __in short sCurChannelNum, __in short sSeqCdInLobby )
{
	g_pVBuffer->Init();
	g_pVBuffer->SetShort( CLTG_CHANNEL_LIST_AQ );
	g_pVBuffer->SetUInteger64( i64CharCd );
	g_pVBuffer->SetShort( sCurChannelNum );
	g_pVBuffer->SetShort( sSeqCdInLobby );

	short sChannelCount = 0;
	char* pcChannelCnt = g_pVBuffer->GetCurMark();
	g_pVBuffer->SetShort( sChannelCount );

	// 채널 정보
	INT32  iServerChannelCount = 0;
	short sServerIndex = 0;
	INT32  iGameServerCount = static_cast<INT32 >(m_GameServerInfos.size());
	for( INT32  iServer = 0; iServer < iGameServerCount; ++iServer )
	{
		// 연결이 끊어진 서버라면 무시한다.
		if( false == IsConnectedGameServer( m_GameServerInfos[iServer] ) ) 
		{
			continue;
		}

		sServerIndex = m_GameServerInfos[iServer].sSeverIndex;
		iServerChannelCount = m_GameServerInfos[iServer].sChannelCount;
		for( INT32  iChannel = 0; iChannel < iServerChannelCount; ++iChannel )
		{
			g_pVBuffer->SetShort( sServerIndex );
			g_pVBuffer->SetShort( m_GameServerInfos[iServer].asMaxChannelUserCount[iChannel] - MAX_RESERVED_CHANNEL_USER_COUNT );
			g_pVBuffer->SetShort( m_GameServerInfos[iServer].asCurChannelUserCount[iChannel] );
			
			++sChannelCount;
		}
	}

	CopyMemory( pcChannelCnt , &sChannelCount , sizeof( short ) );
	pkConnection->SendVBuffer();
}

/*
Discription	: 현재 시간을 설정한다.   * cTickThread에서 지정된 시간마다 시간을 갱신한다.
*/
void LoginIocpServer::SetCurTime()
{
	GetLocalTime(&m_tCurTime);
}


