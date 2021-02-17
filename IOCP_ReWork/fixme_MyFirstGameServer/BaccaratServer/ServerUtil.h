// ServerUtil.h: interface for the CServerUtil class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _SERVERUTIL_H_
#define _SERVERUTIL_H_

#include <time.h>
#include <stdio.h>

//#define _LOGCONSOLE_	// 이 플래그를 정의 되어 있어야지 콘솔 창이 생긴다.

#define MAXLOGBUFSIZE		1024
#define LOGFILENAMELENGTH   64

#define USERLOG			1
#define PROCESSLOG		2
#define CHANNELLOG		3
#define ROOMLOG			4
#define GAMELOG			5
#define GAME_ERRORLOG	6
#define ERRORLOG		7
#define SOCKET_ERRORLOG 8


class CServerUtil  
{
public:
	CServerUtil();
	virtual ~CServerUtil();

	void ConsoleOutput( char* fm, ... );
	
	void SetLogFileName( /*const int index*/ );
	void LogPrint( const char* msg, ... );
	
	// 공통적으로 사용할 로그 함수
	void ServerStartLog();
	void ServerEndLog();
	void JoinChannelLog( int iChannel, int iCurUserNum, char* strID );
	void ExitChannelLog( int iChannel, char* strID );
	void EnterRoomLog( int iChannel, int iRoom, int iCurUserNum, BOOL bCreate, char* strID );
	void ExitRoomLog( int type, int Channel, int Room, int State, char* Id );
	void LogInFailed( char* strIP, int result );
	void LogInSuccess( char* strID );
	void LogOut( char* strID );
	void UserKickLog( char* strID, int State );
	void UserAgencyLog( char* strID, int State );
	void UserSaveLog( int iWin, int iLose, int DisCon, __int64 iMoney, int iLevel, char* strID );
	void RoomLog( int iChannel, int iRoom, const char* strMsg );
	void DBErrorLog( const char* ErrMsg, int iResult );
	void ErrorLog( int codeline );
	//////////////////////////////////////////////////////////////////////////
	

	// 소켓 에러 로그
	void ReInitSocketContextErrLog( int result );
	void PostTcpRecvErrLog( char* achID, int iResult );
	void PostTcpSendRestErrLog( char* achID, int iResult );
	void PostTcpSendErrLog( char* achID, int iResult );
	void NowTcpSendErrLog( char* achID, int iResult );
	//////////////////////////////////////////////////////////////////////////


	// 각 게임마다 사용할 로그 함수
	void NotifyRoomStateLog( int Channel, int Room, int State );
	void BeginGameInRoomLog( int Channel, int Room, int UserNum );
	void GameLog( int Channel, int Room, const char* Msg );
	void GameCrack( int Channel, int Room, char* ID );
	void GameResultLog( int Channel, int Room );
	void GameErrorLog( int Channel, int Room, const char* ErrMsg );
	//////////////////////////////////////////////////////////////////////////
	


	time_t	m_LogTime;		// 로그 파일에서 초 단위로 사용.
	tm*		m_LogCalTime;	// 로그 파일에서 달력 단위로 사용.

private:
	FILE*	m_fp;
	void DestoryDebugConsole();
	void CreateDebugConsole();
	BOOL m_isAllocated;
	HANDLE m_hConsoleOutput;

	char m_cLogFileName[LOGFILENAMELENGTH];
};

extern CServerUtil	ServerUtil;
#endif // !defined(AFX_SERVERUTIL_H__9E72DC7E_D993_4587_920E_CA42B41FB6A6__INCLUDED_)
