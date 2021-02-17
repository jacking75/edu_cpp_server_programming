// Server.cpp: implementation of the CServer class.
//
//////////////////////////////////////////////////////////////////////


#include <wchar.h>
#include <process.h>
#include "serverprocess.h"
#include "channel.h"
#include "room.h"
#include "database.h"
#include "sock.h"
#include "game.h"
#include "serverutil.h"
#include "Server.h"


SERVERCONTEXT	ServerContext;
ONTRANSFUNC		OnTransFunc[ MAXTRANSFUNC ];
//class CRoom;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CServer::CServer()
{
	
}

CServer::~CServer()
{

}

int CServer::Initialize()
{
	if (!InitServerInfo()) {
		return 0;
	}

	// DB 초기화를 하면서 서버 프로그램에 필요한 모든 정보를 가져온다.
	if (!InitDataBaseLayer()) {
		return 0;
	}

	if (!InitSocketIO()) {
		return 0;
	}

	if (!InitRoomLayer()) {
		return 0;
	}

	if (!InitChannelLayer()) {
		return 0;
	}

	if (!InitProcessLayer()) {
		return 0;
	}

	ServerContext.db->StroageCurUserNumInServer();

#ifdef _LOGFILELEVEL1_
	ServerUtil.ServerStartLog();
#endif
	
	return 1;
}



int CServer::InitServerInfo()
{
	WCHAR			wcModuleName[ MAX_PATH ];	// 서버의 정보가 들어가 있는 ini 파일의 위치
	char			cModuleName[ MAX_PATH ];
	int				length = 0;
	
	// ini 파일이 있는 위치를 지정한다.
	GetModuleFileNameW( GetModuleHandle( NULL ), wcModuleName, MAX_PATH );
	*( wcsrchr( wcModuleName, '\\') + 1 ) = 0;
	wcscat( wcModuleName, L"BaccaratServerConf.ini" );

	GetModuleFileName( GetModuleHandle( NULL ), cModuleName, MAX_PATH );
	*( strrchr( cModuleName, '\\') + 1 ) = 0;
	strcat( cModuleName, "BaccaratServerConf.ini" );

	
	// 서버, 게임 아이디를 저장 할 변수를 NULL로 초기화 한다.
	wmemset( ServerContext.wcServerID, 0, MAXSERVERID );
	wmemset( ServerContext.wcGameID, 0, MAXGAMEID );
	wmemset( ServerContext.wcDBServerIP, 0, MAXDBSERVERIP );
	wmemset( ServerContext.wcDataBase, 0, MAXDATABASE );

	// 서버 아이디를 INI 파일에서 불러온다.
	length = GetPrivateProfileStringW( L"SETTING", L"SERVERID", L"baccarat_001",
					               ServerContext.wcServerID, MAXSERVERID, wcModuleName );
	if( length == 0 ) return 0;

	// 서비스 게임 아이디를 INI 파일에서 불러온다.
	length = GetPrivateProfileStringW( L"SETTING", L"GAMEID", L"baccarat",
									ServerContext.wcGameID, MAXGAMEID, wcModuleName );
	if( length == 0 ) return 0;

	// 게임 DB 서버의 IP를 INI 파일에서 불러온다.
	length = GetPrivateProfileStringW( L"SETTING", L"DB_GAMESERVER_IP", L"211.106.195.38",
									ServerContext.wcDBServerIP, MAXDBSERVERIP, wcModuleName );
	if( length == 0 ) return 0;

	// 게임에서 사용하는 데이타 베이스의 이름을 INI 파일에서 불러온다.
	length = GetPrivateProfileStringW( L"SETTING", L"DB_DATABASE_NAME", L"haogame",
							ServerContext.wcDataBase, MAXDATABASE, wcModuleName );
	if( length == 0 ) return 0;

	ServerContext.bThreadStop = FALSE;


	// 바카라 게임에서 사용하는 레벨에 따른 배팅 금액 셋팅한다.
	ServerContext.BaseBettingMoney.first = GetPrivateProfileInt( "BACCARAT-BASEBETTINGMONEY", "FIRST", 0, cModuleName );
	ServerContext.BaseBettingMoney.middle = GetPrivateProfileInt( "BACCARAT-BASEBETTINGMONEY", "MIDDLE", 0, cModuleName );
	ServerContext.BaseBettingMoney.high = GetPrivateProfileInt( "BACCARAT-BASEBETTINGMONEY", "HIGH", 0, cModuleName );

	// 게임에서 사용하는 한계 시간을 셋팅한다.
	ServerContext.iCheckLifeTime = GetPrivateProfileInt( "GAMETIME", "CHECKLIFTTIME", 90, cModuleName );
	ServerContext.iGameReadyTime = GetPrivateProfileInt( "GAMETIME", "GAMEREADYTIME", 10, cModuleName );
	ServerContext.iGameOnTime    = GetPrivateProfileInt( "GAMETIME", "GAMEONTIME", 10, cModuleName );
	ServerContext.iThinkTime     = GetPrivateProfileInt( "GAMETIME", "THINKTIME", 10, cModuleName );

	InitializeCriticalSection( &ServerContext.csKickUserIndexList );

	// 게임에서 사용하는 비밀방 생성 아이템과 점프 아이템의 인데스 번호를 얻어온다.
	//ServerContext.ItemIndex.iPrivateRoom = GetPrivateProfileInt( "ITEM", "PRIVATEROOM", 3, 
	//																	cModuleName );
	//ServerContext.ItemIndex.iJump = GetPrivateProfileInt( "ITEM", "LEVELJUMP", 4, cModuleName );


	/* 현재 미 사용 
	// ini 파일의 각 섹션의 멤버에서 지정된 값을 가져온다.
	ServerContext.iPortNum = GetPrivateProfileInt( "SETTING", "PORT", 10004, cModuleName );
	ServerContext.iMaxUserNum = GetPrivateProfileInt( "SETTING", "MAXUSER", 32, cModuleName );
	ServerContext.iMaxProcess = GetPrivateProfileInt( "SETTING", "MAXPROCESS", 1, cModuleName );
	ServerContext.iMaxChannelInProcess = GetPrivateProfileInt( "SETTING", "MAXCHANNELINPROCESS",
																16, cModuleName );
	ServerContext.iMaxRoomInChannel = GetPrivateProfileInt( "SETTING", "MAXROOMINCHANNEL", 
																16, cModuleName );
	ServerContext.iMaxUserInRoom = GetPrivateProfileInt( "SETTING", "MAXUSERINROOM", 5, cModuleName );
	ServerContext.iInWorkerTNum = GetPrivateProfileInt( "SETTING", "WORKERTHREAD", 0, cModuleName );
	ServerContext.iInDataBaseTNum = GetPrivateProfileInt( "SETTING", "DATABASETHREAD", 0, cModuleName );

	ServerContext.iMaxChannel = ServerContext.iMaxProcess * ServerContext.iMaxChannelInProcess;
	ServerContext.iMaxRoom = ServerContext.iMaxChannel * ServerContext.iMaxRoomInChannel;
	ServerContext.iMaxUserInChannel = ServerContext.iMaxUserInRoom * ServerContext.iMaxRoomInChannel;
	ServerContext.iCurUserNum = 0;

	// ini 파일에 워크 쓰레드의 크기가 지정 되어 있지 않은 경우 워크쓰레드의 크기를
	// 현재의 머신의 CPU 개숫를 참고로 쓰레드의 숫자를 지정한다.
	if( ServerContext.iInWorkerTNum == 0 )
	{
		GetSystemInfo( &si );

		// 최대 16개는 넘지 못하도록 한다.
		ServerContext.iInWorkerTNum = min( si.dwNumberOfProcessors * 2, 16 );
	}
	*/

	return 1;
}



void CServer::FinalCleanup()
{
	// 쓰레드 작동을 중지 시킨다.
	ServerContext.bThreadStop = TRUE;
	
	CloseServerSocket();
	DeleteCS();

		
	SAFE_DELETE_ARRAY( ServerContext.rm )
	SAFE_DELETE_ARRAY( ServerContext.rn )
	SAFE_DELETE_ARRAY( ServerContext.ch )
	SAFE_DELETE_ARRAY( ServerContext.gameproc )

		
	SAFE_DELETE( ServerContext.db )
	SAFE_DELETE_ARRAY( ServerContext.sc )
	SAFE_DELETE_ARRAY( ServerContext.pn )
	SAFE_DELETE( ServerContext.ps )

#ifdef _LOGFILELEVEL1_
	ServerUtil.ServerEndLog();
#endif
	
}


void CServer::CloseServerSocket()
{
	int iN, iMax;
	
	iMax = ServerContext.iMaxUserNum;

	closesocket( ServerContext.sockListener );

	for( iN=0; iN < iMax; ++iN )
	{
		closesocket( ServerContext.sc[ iN ].sockTcp );
	}

	WSACleanup();
}

void CServer::DeleteCS()
{
	int iN, iMax;

	iMax = ServerContext.iMaxUserNum;
	for( iN = 0; iN < iMax; ++iN )
	{
		DeleteCriticalSection( &ServerContext.sc[ iN ].csSTcp );
	}

	DeleteCriticalSection( &ServerContext.csKickUserIndexList );
}
