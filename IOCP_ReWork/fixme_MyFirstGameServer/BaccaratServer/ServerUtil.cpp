// ServerUtil.cpp: implementation of the CServerUtil class.
//
//////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <tchar.h>
#include "ServerUtil.h"


CServerUtil ServerUtil;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CServerUtil::CServerUtil()
{

#ifdef _LOGCONSOLE_
	CreateDebugConsole();
#endif

	SetLogFileName();
}

CServerUtil::~CServerUtil()
{
	fclose( m_fp);

#ifdef _LOGCONSOLE_
	DestoryDebugConsole();
#endif
}

void CServerUtil::SetLogFileName( /*const int index*/ )
{
	time_t starttime;
	tm* CalTime;

	time( &starttime );
	CalTime = localtime( &starttime );

	memset( m_cLogFileName, 0, LOGFILENAMELENGTH );

	sprintf( m_cLogFileName, "%d-%d-%d-%d-%d-%d.log", 
		CalTime->tm_year+1900, CalTime->tm_mon+1, CalTime->tm_mday,
		CalTime->tm_hour, CalTime->tm_min, CalTime->tm_sec );

	m_fp = fopen( m_cLogFileName, "a" );
}


void CServerUtil::LogPrint(const char* msg, ... )
{
	char	buff[ MAXLOGBUFSIZE ];
	va_list	vl;
	

	va_start( vl, msg );
	vsprintf( buff, msg, vl );
	va_end( vl );
	
	if( m_fp == NULL)
		return;
	
	fprintf( m_fp, buff);
	fflush( m_fp );
}


void CServerUtil::ServerStartLog()
{
	char	buff[ MAXLOGBUFSIZE ];

	time( &m_LogTime );
	m_LogCalTime = localtime( &m_LogTime );

	sprintf( buff, "< %d,%dY-%dM-%dD-%dh-%dm-%ds ServerStart >\n",
		m_LogTime, m_LogCalTime->tm_year+1900, m_LogCalTime->tm_mon+1,
		m_LogCalTime->tm_mday,m_LogCalTime->tm_hour, m_LogCalTime->tm_min,
		m_LogCalTime->tm_sec );

	if( m_fp == NULL)
		return;

	fprintf( m_fp, buff);
	fflush( m_fp );
}

void CServerUtil::ServerEndLog()
{
	char	buff[ MAXLOGBUFSIZE ];

	time( &m_LogTime );
	m_LogCalTime = localtime( &m_LogTime );

	sprintf( buff, "< %d,%dY-%dM-%dD-%dh-%dm-%ds ServerEnd >\n",
		m_LogTime, m_LogCalTime->tm_year+1900, m_LogCalTime->tm_mon+1,
		m_LogCalTime->tm_mday,m_LogCalTime->tm_hour, m_LogCalTime->tm_min,
		m_LogCalTime->tm_sec );

	if( m_fp == NULL)
		return;

	fprintf( m_fp, buff);
	fflush( m_fp );
}

void CServerUtil::JoinChannelLog( int iChannel, int iCurUserNum, char* strID )
{
	char	buff[ MAXLOGBUFSIZE ];

	time( &m_LogTime );
	m_LogCalTime = localtime( &m_LogTime );

	sprintf( buff, "< %d,%dM-%dD-%dh-%dm-%ds,Type(%d),Channel(%d), CurUserNum(%d) : JoinChannel - ID(%s) >\n",
		m_LogTime, m_LogCalTime->tm_mon+1,m_LogCalTime->tm_mday,m_LogCalTime->tm_hour,
		m_LogCalTime->tm_min,m_LogCalTime->tm_sec,
		CHANNELLOG, iChannel, iCurUserNum, strID );

	if( m_fp == NULL)
		return;

	fprintf( m_fp, buff);
	fflush( m_fp );
}

void CServerUtil::ExitChannelLog( int iChannel, char* strID )
{
	char	buff[ MAXLOGBUFSIZE ];

	time( &m_LogTime );
	m_LogCalTime = localtime( &m_LogTime );

	sprintf( buff, "< %d,%dM-%dD-%dh-%dm-%ds,Type(%d),Channel(%d), : ExitChannel - ID(%s) >\n",
		m_LogTime, m_LogCalTime->tm_mon+1,m_LogCalTime->tm_mday,m_LogCalTime->tm_hour,
		m_LogCalTime->tm_min,m_LogCalTime->tm_sec,
		CHANNELLOG, iChannel, strID );

	if( m_fp == NULL)
		return;

	fprintf( m_fp, buff);
	fflush( m_fp );
}

void CServerUtil::EnterRoomLog( int iChannel, int iRoom, int iCurUserNum, BOOL bCreate, char* strID )
{
	char	buff[ MAXLOGBUFSIZE ];

	time( &m_LogTime );
	m_LogCalTime = localtime( &m_LogTime );

	if( bCreate == TRUE )
	{
		sprintf( buff, "< %d,%dM-%dD-%dh-%dm-%ds,Type(%d),Channel(%d),Room(%d), CurUserNum(%d) : CreateRoom - ID(%s) >\n",
			m_LogTime, m_LogCalTime->tm_mon+1,m_LogCalTime->tm_mday,m_LogCalTime->tm_hour,
			m_LogCalTime->tm_min,m_LogCalTime->tm_sec,
			ROOMLOG, iChannel, iRoom, iCurUserNum, strID );
	}
	else
	{
		sprintf( buff, "< %d,%dM-%dD-%dh-%dm-%ds,Type(%d),Channel(%d),Room(%d), CurUserNum(%d) : JoinRoom - ID(%s) >\n",
			m_LogTime, m_LogCalTime->tm_mon+1,m_LogCalTime->tm_mday,m_LogCalTime->tm_hour,
			m_LogCalTime->tm_min,m_LogCalTime->tm_sec,
			ROOMLOG, iChannel, iRoom, iCurUserNum, strID );
	}

	if( m_fp == NULL)
		return;

	fprintf( m_fp, buff);
	fflush( m_fp );
}

void CServerUtil::ExitRoomLog( int type, int Channel, int Room, int State, char* Id )
{
	char	buff[ MAXLOGBUFSIZE ];

	time( &m_LogTime );
	m_LogCalTime = localtime( &m_LogTime );

	sprintf( buff, "< %d,%dM-%dD-%dh-%dm-%ds,Type(%d),Channel(%d),Room(%d) : ExitRoom - State(%d),ID(%s) >\n",
		m_LogTime, m_LogCalTime->tm_mon+1,m_LogCalTime->tm_mday,m_LogCalTime->tm_hour,
		m_LogCalTime->tm_min,m_LogCalTime->tm_sec,
		type, Channel, Room, State, Id );

	if( m_fp == NULL)
		return;

	fprintf( m_fp, buff);
	fflush( m_fp );
}

void CServerUtil::LogInFailed( char* strIP, int result )
{
	char	buff[ MAXLOGBUFSIZE ];

	time( &m_LogTime );
	m_LogCalTime = localtime( &m_LogTime );

	sprintf( buff, "< %d,%dM-%dD-%dh-%dm-%ds,Type(%d) : Logging Failed - IP(%s), Result(%d) >\n",
		m_LogTime, m_LogCalTime->tm_mon+1,m_LogCalTime->tm_mday,m_LogCalTime->tm_hour,
		m_LogCalTime->tm_min,m_LogCalTime->tm_sec,
		USERLOG, strIP, result );

	if( m_fp == NULL)
		return;

	fprintf( m_fp, buff);
	fflush( m_fp );
}

void CServerUtil::LogInSuccess( char* strID )
{
	char	buff[ MAXLOGBUFSIZE ];

	time( &m_LogTime );
	m_LogCalTime = localtime( &m_LogTime );

	sprintf( buff, "< %d,%dM-%dD-%dh-%dm-%ds,Type(%d) : Logging Success - ID(%s) >\n",
		m_LogTime, m_LogCalTime->tm_mon+1,m_LogCalTime->tm_mday,m_LogCalTime->tm_hour,
		m_LogCalTime->tm_min,m_LogCalTime->tm_sec,
		USERLOG, strID );

	if( m_fp == NULL)
		return;

	fprintf( m_fp, buff);
	fflush( m_fp );
}

void CServerUtil::LogOut( char* strID )
{
	char	buff[ MAXLOGBUFSIZE ];

	time( &m_LogTime );
	m_LogCalTime = localtime( &m_LogTime );

	sprintf( buff, "< %d,%dM-%dD-%dh-%dm-%ds,Type(%d) : Log Out - ID(%s) >\n",
		m_LogTime, m_LogCalTime->tm_mon+1,m_LogCalTime->tm_mday,m_LogCalTime->tm_hour,
		m_LogCalTime->tm_min,m_LogCalTime->tm_sec,
		USERLOG, strID );

	if( m_fp == NULL)
		return;

	fprintf( m_fp, buff);
	fflush( m_fp );
}

void CServerUtil::UserKickLog( char* strID, int State )
{
	char	buff[ MAXLOGBUFSIZE ];

	time( &m_LogTime );
	m_LogCalTime = localtime( &m_LogTime );

	sprintf( buff, "< %d,%dM-%dD-%dh-%dm-%ds,Type(%d) : UserKick- State(%d),ID(%s) >\n",
		m_LogTime, m_LogCalTime->tm_mon+1,m_LogCalTime->tm_mday,m_LogCalTime->tm_hour,
		m_LogCalTime->tm_min,m_LogCalTime->tm_sec,
		USERLOG, State, strID );

	if( m_fp == NULL)
		return;

	fprintf( m_fp, buff);
	fflush( m_fp );
}

void CServerUtil::UserAgencyLog( char* strID, int State )
{
	char	buff[ MAXLOGBUFSIZE ];

	time( &m_LogTime );
	m_LogCalTime = localtime( &m_LogTime );

	sprintf( buff, "< %d,%dM-%dD-%dh-%dm-%ds,Type(%d) : UserAgency- State(%d),ID(%s) >\n",
		m_LogTime, m_LogCalTime->tm_mon+1,m_LogCalTime->tm_mday,m_LogCalTime->tm_hour,
		m_LogCalTime->tm_min,m_LogCalTime->tm_sec,
		USERLOG, State, strID );

	if( m_fp == NULL)
		return;

	fprintf( m_fp, buff);
	fflush( m_fp );
}

void CServerUtil::UserSaveLog( int iWin, int iLose, int DisCon, __int64 iMoney, int iLevel, char* strID )
{
	char	buff[ MAXLOGBUFSIZE ];

	time( &m_LogTime );
	m_LogCalTime = localtime( &m_LogTime );

	sprintf( buff, "< %d,%dM-%dD-%dh-%dm-%ds,Type(%d) : Win(%d), Lose(%d), DisCon(%d), Money(%I64d), Level(%d) ,ID(%s) >\n",
		m_LogTime, m_LogCalTime->tm_mon+1,m_LogCalTime->tm_mday,m_LogCalTime->tm_hour,
		m_LogCalTime->tm_min,m_LogCalTime->tm_sec,
		USERLOG, iWin, iLose, DisCon, iMoney, iLevel, strID );

	if( m_fp == NULL)
		return;

	fprintf( m_fp, buff);
	fflush( m_fp );
}

void CServerUtil::RoomLog( int iChannel, int iRoom, const char* strMsg )
{
	char	buff[ MAXLOGBUFSIZE ];

	time( &m_LogTime );
	m_LogCalTime = localtime( &m_LogTime );

	sprintf( buff, "< %d,%dM-%dD-%dh-%dm-%ds,Type(%d) : Channel(%d), Room(%d) - %s >\n",
		m_LogTime, m_LogCalTime->tm_mon+1,m_LogCalTime->tm_mday,m_LogCalTime->tm_hour,
		m_LogCalTime->tm_min,m_LogCalTime->tm_sec,
		ROOMLOG, iChannel, iRoom, strMsg );

	if( m_fp == NULL)
		return;

	fprintf( m_fp, buff);
	fflush( m_fp );
}

void CServerUtil::DBErrorLog( const char* ErrMsg, int iResult )
{
	char	buff[ MAXLOGBUFSIZE ];

	time( &m_LogTime );
	m_LogCalTime = localtime( &m_LogTime );

	sprintf( buff, "< %d,%dM-%dD-%dh-%dm-%ds,Type(%d) : DBErr : %s - %d >\n",
		m_LogTime, m_LogCalTime->tm_mon+1,m_LogCalTime->tm_mday,m_LogCalTime->tm_hour,
		m_LogCalTime->tm_min,m_LogCalTime->tm_sec, ERRORLOG, ErrMsg, iResult );

	if( m_fp == NULL)
		return;

	fprintf( m_fp, buff);
	fflush( m_fp );
}

void CServerUtil::ErrorLog( int codeline )
{
}



void CServerUtil::ReInitSocketContextErrLog( int result )
{
	char	buff[ MAXLOGBUFSIZE ];

	time( &m_LogTime );
	m_LogCalTime = localtime( &m_LogTime );

	sprintf( buff, "< %d,%dM-%dD-%dh-%dm-%ds,Type(%d) : ReInitSockContext_Failed : %d >\n",
		m_LogTime, m_LogCalTime->tm_mon+1,m_LogCalTime->tm_mday,m_LogCalTime->tm_hour,
		m_LogCalTime->tm_min,m_LogCalTime->tm_sec, ERRORLOG, result );

	if( m_fp == NULL)
		return;

	fprintf( m_fp, buff);
	fflush( m_fp );
}

void CServerUtil::PostTcpRecvErrLog( char* achID, int iResult )
{
	char	buff[ MAXLOGBUFSIZE ];

	time( &m_LogTime );
	m_LogCalTime = localtime( &m_LogTime );

	wsprintf( buff, "< %d,%dM-%dD-%dh-%dm-%ds,Type(%d) : PostTcpRecv Failed(%d) : ID(%s) >\n",
		m_LogTime, m_LogCalTime->tm_mon+1,m_LogCalTime->tm_mday,m_LogCalTime->tm_hour,
		m_LogCalTime->tm_min,m_LogCalTime->tm_sec, ERRORLOG, iResult, achID );

	if( m_fp == NULL)
		return;

	fprintf( m_fp, buff);
	fflush( m_fp );
}

void CServerUtil::PostTcpSendRestErrLog( char* achID, int iResult )
{
	char	buff[ MAXLOGBUFSIZE ];

	time( &m_LogTime );
	m_LogCalTime = localtime( &m_LogTime );

	sprintf( buff, "< %d,%dM-%dD-%dh-%dm-%ds,Type(%d) : PostTcpSendRest Error(%d) - ID(%s) >\n",
		m_LogTime, m_LogCalTime->tm_mon+1,m_LogCalTime->tm_mday,m_LogCalTime->tm_hour,
		m_LogCalTime->tm_min,m_LogCalTime->tm_sec, ERRORLOG, iResult, achID );

	if( m_fp == NULL)
		return;

	fprintf( m_fp, buff);
	fflush( m_fp );
}

void CServerUtil::PostTcpSendErrLog( char* achID, int iResult )
{
	char	buff[ MAXLOGBUFSIZE ];

	time( &m_LogTime );
	m_LogCalTime = localtime( &m_LogTime );

	sprintf( buff, "< %d,%dM-%dD-%dh-%dm-%ds,Type(%d) : tcp sendbuf overflow!(%d) - ID(%s) >\n",
		m_LogTime, m_LogCalTime->tm_mon+1,m_LogCalTime->tm_mday,m_LogCalTime->tm_hour,
		m_LogCalTime->tm_min,m_LogCalTime->tm_sec, ERRORLOG, iResult, achID );

	if( m_fp == NULL)
		return;

	fprintf( m_fp, buff);
	fflush( m_fp );
}

void CServerUtil::NowTcpSendErrLog( char* achID, int iResult )
{
	char	buff[ MAXLOGBUFSIZE ];

	time( &m_LogTime );
	m_LogCalTime = localtime( &m_LogTime );

	sprintf( buff, "< %d,%dM-%dD-%dh-%dm-%ds,Type(%d) : NowTcpSend Failed(%d) - ID(%s) >\n",
		m_LogTime, m_LogCalTime->tm_mon+1,m_LogCalTime->tm_mday,m_LogCalTime->tm_hour,
		m_LogCalTime->tm_min,m_LogCalTime->tm_sec, ERRORLOG, iResult, achID );

	if( m_fp == NULL)
		return;

	fprintf( m_fp, buff);
	fflush( m_fp );
}



void CServerUtil::NotifyRoomStateLog( int Channel, int Room, int State )
{
	char	buff[ MAXLOGBUFSIZE ];

	time( &m_LogTime );
	m_LogCalTime = localtime( &m_LogTime );

	sprintf( buff, "< %d,%dM-%dD-%dh-%dm-%ds,Type(%d),Channel(%d),Room(%d) : NotifyRoomState - State(%d) >\n",
		m_LogTime, m_LogCalTime->tm_mon+1,m_LogCalTime->tm_mday,m_LogCalTime->tm_hour,
		m_LogCalTime->tm_min,m_LogCalTime->tm_sec,
		GAMELOG, Channel, Room, State );

	if( m_fp == NULL)
		return;

	fprintf( m_fp, buff);
	fflush( m_fp );
}

void CServerUtil::BeginGameInRoomLog( int Channel, int Room, int UserNum )
{
	char	buff[ MAXLOGBUFSIZE ];

	time( &m_LogTime );
	m_LogCalTime = localtime( &m_LogTime );

	sprintf( buff, "< %d,%dM-%dD-%dh-%dm-%ds,Type(%d),Channel(%d),Room(%d) : BeginGameInRoom - TotalPlayerNum(%d) >\n",
		m_LogTime, m_LogCalTime->tm_mon+1,m_LogCalTime->tm_mday,m_LogCalTime->tm_hour,
		m_LogCalTime->tm_min, m_LogCalTime->tm_sec, GAMELOG, Channel, Room, UserNum );

	if( m_fp == NULL)
		return;

	fprintf( m_fp, buff);
	fflush( m_fp );
}

void CServerUtil::GameLog( int Channel, int Room, const char* Msg )
{
	char	buff[ MAXLOGBUFSIZE ];

	time( &m_LogTime );
	m_LogCalTime = localtime( &m_LogTime );

	sprintf( buff, "< %d,%dM-%dD-%dh-%dm-%ds,Type(%d),Channel(%d),Room(%d) : %s >\n",
		m_LogTime, m_LogCalTime->tm_mon+1,m_LogCalTime->tm_mday,m_LogCalTime->tm_hour,
		m_LogCalTime->tm_min, m_LogCalTime->tm_sec, GAMELOG, Channel, Room, Msg );

	if( m_fp == NULL)
		return;

	fprintf( m_fp, buff);
	fflush( m_fp );
}

void CServerUtil::GameCrack( int Channel, int Room, char* ID )
{
	char	buff[ MAXLOGBUFSIZE ];

	time( &m_LogTime );
	m_LogCalTime = localtime( &m_LogTime );

	sprintf( buff, "< %d,%dM-%dD-%dh-%dm-%ds,Type(%d),Channel(%d),Room(%d) : Cracker ID(%s) >\n",
		m_LogTime, m_LogCalTime->tm_mon+1,m_LogCalTime->tm_mday,m_LogCalTime->tm_hour,
		m_LogCalTime->tm_min, m_LogCalTime->tm_sec, GAMELOG, Channel, Room, ID );

	if( m_fp == NULL)
		return;

	fprintf( m_fp, buff);
	fflush( m_fp );
}

void CServerUtil::GameResultLog( int Channel, int Room )
{
	char	buff[ MAXLOGBUFSIZE ];

	time( &m_LogTime );
	m_LogCalTime = localtime( &m_LogTime );

	wsprintf( buff, "< %d,%dM-%dD-%dh-%dm-%ds,Type(%d),Channel(%d),Room(%d) : Game Result >\n",
		m_LogTime, m_LogCalTime->tm_mon+1,m_LogCalTime->tm_mday,m_LogCalTime->tm_hour,
		m_LogCalTime->tm_min, m_LogCalTime->tm_sec, GAMELOG, Channel, Room );

	if( m_fp == NULL)
		return;

	fprintf( m_fp, buff);
	fflush( m_fp );
}

void CServerUtil::GameErrorLog( int Channel, int Room, const char* ErrMsg )
{
	char	buff[ MAXLOGBUFSIZE ];

	time( &m_LogTime );
	m_LogCalTime = localtime( &m_LogTime );

	sprintf( buff, "< %d,%dM-%dD-%dh-%dm-%ds,Type(%d),Channel(%d),Room(%d) : Err(%s) >\n",
		m_LogTime, m_LogCalTime->tm_mon+1,m_LogCalTime->tm_mday,m_LogCalTime->tm_hour,
		m_LogCalTime->tm_min,m_LogCalTime->tm_sec, GAME_ERRORLOG, Channel, Room, ErrMsg );

	if( m_fp == NULL)
		return;

	fprintf( m_fp, buff);
	fflush( m_fp );
}


void CServerUtil::ConsoleOutput(char *fm, ...)
{
	va_list		argptr;
	char		cBuf[ 512 ];
	int			iCnt;
	DWORD		dwWritten;

	if( !m_isAllocated ) return;

	va_start( argptr, fm );
	iCnt = vsprintf( cBuf, fm, argptr );
	va_end( argptr );

	WriteConsole( m_hConsoleOutput, cBuf, iCnt, &dwWritten, NULL );
}

void CServerUtil::CreateDebugConsole()
{
	if( !AllocConsole() )
	{
		MessageBox( NULL, "콘솔을 만들수가 없습니다.", "Error", MB_OK );
		m_isAllocated = FALSE;
	}
	else
	{
		m_hConsoleOutput = GetStdHandle( STD_OUTPUT_HANDLE );
		m_isAllocated = TRUE;
	}
}

void CServerUtil::DestoryDebugConsole()
{
	if( !FreeConsole() )
	{
		MessageBox( NULL, "콘솔 제거 실패 !!!", "Error", MB_OK );
	}
}
