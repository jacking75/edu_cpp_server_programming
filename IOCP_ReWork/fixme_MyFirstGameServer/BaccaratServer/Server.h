// Server.h: interface for the CServer class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _SERVER_H_
#define _SERVER_H_

//#include <windows.h>
#include "define.h"


class CProcess;
class CChannel;
class CRoom;
class CDataBase;
class CGame;

class CServer  
{
public:
	int Initialize();
	void FinalCleanup();
	
	CServer();
	virtual ~CServer();

private:
	int InitServerInfo();
	
private:
	void DeleteCS();
	void CloseServerSocket();
	
	
};




// 서버의 소켓 컨텍스트
typedef struct
{
	SOCKET			sockListener;		// 클라이언트의 접속을 받는 소켓
	HANDLE			hIocpWorkTcp,		// IOCP 작업 핸들,
					hIocpAcpt;			// ICOP Accept 핸들

	SOCKETCONTEXT	*sc;

	OBJECTNODE		*pn,				// 유저의 노드
					*rn;				// 룸의 노드

	CProcess        *ps;
	CChannel        *ch;
	CRoom           *rm;
	CDataBase		*db;
	CGame			*gameproc;

	WCHAR			wcServerID[MAXSERVERID];		// 서버 머신의 ID
	WCHAR			wcGameID[MAXGAMEID];			// 서버에서 서비스하는 게임의 ID
	WCHAR			wcDBServerIP[20];				// 게임 DB 서버의 IP
	WCHAR			wcDataBase[20];					// 게임 DB의 데이타베이스 이름
	int				iVersion,						// 클라이언트 버전( 버전 1.0 - 1000 )
					iPortNum;    					// 포트번호
	
	int				iMaxUserNum,			// 최대 접속자 수
					iCurUserNum;			// 현재 접속자 수
	
	char			iInWorkerTNum,			// 최대 워커쓰레드 숫자
					iInDataBaseTNum;		// 최대 데이타베이스 쓰레드 숫자
	BOOL			bThreadStop;			// 스레드 작동을 중지 시킨다.
					
	char			iMaxProcess,			// 최대 프로세스 숫
					iMaxChannelInProcess,	// 프로세스에 관리하는 최대 채널 수
					iMaxChannel,			// 최대 채널 수
					iMaxRoomInChannel,		// 채널당 최대 룸의 수
					iMaxUserInRoom;			// 룸의 최대 유저 수
					
	int				iMaxRoom,				// 서버의 최대 룸 수
					iMaxUserInChannel;		// 채널의 최대 유저 수
						
	char			cChannelHighNum,		// 서버에서의 고수 채널 수
					cCnannelMiddleNum,		// 서버에서의 중수 채널 수
					cChannelLowNum,			// 서버에서의 하수 채널 수
					cLevel_Middle,			// 중수 채널에 들어 갈 수 있는 레벨	
					cLevel_High;			// 고수 채널에 들어 갈 수 있는 레벨

	USERLEVELBOUNDARY	ULBoundary[12];      // 초,중,고 레벨의 최소와 최대의 경계
	BASEBETTINGMONEY	BaseBettingMoney;	// 포카 게임에서의 레벨에 따른 기본 배팅 금액.
	
	time_t	tMeasureLifeTime;	// 게임 프로세스에서 클라이언트 생존 유/무를 체크한 시간.
	time_t	tMeasureRoomTime;	// 게임 프로세스에서 방의 상태를 체크한 시간.

	int		iCheckLifeTime;		// 클라이언트가 살아 있는지 체크 하는 시간.
	int		iGameReadyTime;		// 대기에서 게임 시작 할 때까지의 시간(초 단위)
	int		iGameOnTime;		// 클라이언트에서 게임 시작 준비를 할 수 있는 시간.
	int		iThinkTime;			// 유저 턴에서 최대 결정 시간.

	CRITICAL_SECTION	csKickUserIndexList; // 킥을 시킬 유저의 인덱스 리스트임계영역.
	deque<int>	dKickUserIndexList;			 // 킥을 시킬 유저의 인덱스 리스트.	

}SERVERCONTEXT, *LPSERVERCONTEXT;

extern	SERVERCONTEXT	ServerContext;

#endif // !defined(AFX_SERVER_H__C2166BB5_0356_4499_9A35_981EEF764368__INCLUDED_)
