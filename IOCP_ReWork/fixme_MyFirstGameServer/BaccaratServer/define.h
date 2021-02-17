#ifndef	_DEFINE_H_
#define _DEFINE_H_

#define _WIN32_WINNT 0x0500
#include <windows.h>
#include <time.h>
#include <deque>

using namespace std;


#define _LOGFILELEVEL1_
#define _LOGFILELEVEL2_
#define _LOGFILELEVEL3_
#define _LOGFILELEVEL4_

//#define _LOGCONSOLE_	// 밑의 플래그를 정의 할려면 꼭 이 플래그가 정의 되야 된다.
//#define _LOGLEVEL1_
//#define _LOGPROCESS_
//#define _LOGDATABASE_
//#define _LOGCHANNEL_
//#define _LOGROOM_
//#define _LOGGAME_

#define	SAFE_DELETE(p) { if(p) { delete p; (p) = NULL; } }
#define	SAFE_DELETE_ARRAY(p) { if(p) { delete[] p; (p) = NULL; } }


#define	MAXSENDPACKSIZE	512			// 보내는 패킷의 최대 크기
#define MAXRECVPACKSIZE	512			// 받는 패킷의 최대 크기
#define RINGBUFSIZE		16384		// 링버퍼의 최대 크기
#define	MAXTRANSFUNC	256			// 패킷 처리 함수의 배열 크기
#define	HEADERSIZE		4			// 패킷의 헤더 크기


#define ROOMPERPACKET	10
#define USERPERPACKET	12


#define LIFETIMECOUNT	90			// 유저가 LIFETIMECOUNT초 이후 까지 패킷을 보내지 않으면 비정상 종료로 본다.
									// 뒤에 30으로 바꾸기.
#define	DEFAULTPROCESS	0
#define DEFAULTCHANNEL	0
#define NOTLINKED		-1

#define NOTLOGINUSER    -1			// 유저의 종료 요청이 비 정상적임을 나타낸다.


#define	NOTINITIALIZED	0
#define	INITIALIZED		1

#define SOCKETCLOSE		0
#define SOCKETCONTINUE	1


#define	MAXSERVERID		20			// 서버 아이디의 최대 길이
#define	MAXGAMEID		20			// 서비스 게임 아이디의 최대 길이
#define MAXDBSERVERIP	30			// 게임 DB 서버의 IP 주소 길이	
#define MAXDATABASE		30			// 게임 DB 서버의 데이타 베이스 이름
#define	MAXIDLENGTH		13			// ID의 최대 길이
#define MAXPWDLENGTH	13			// 패스워드의 최대 길이
#define MAXQUERYLENGTH	512			// SQL 쿼리의 최대 길이
#define MAXCHATPACKETLENGTH	110		// 채팅에서 최대 헤드를 포함한 패킷 크기
#define MAXROOMTITLE	20			// 방 제목의 최대 길이
#define	MAXROOMPWD		15			// 방의 패스워드의 최대 길이



#define USERWIN			1			// 유저가 이겼음
#define USERLOSE		2			// 유저가 패배함
#define USERDISCONNECT	3			// 유저의 접속이 비 정상적으로 종료

#define	MINIMUMUSERLEVEL	0		// 유저 레벨 중 제일 아래
#define	MAXIMUMUSERLEVEL	11		// 유저 레벨 중 제일 위

#define SQLPASSWD			12
#define SQLNUM				4
#define	SQLGAME_ID			12
#define	SQLRESULT_WIN		4
#define	SQLRESULT_LOSE		4
#define	SQLDISCONNECT		4
#define	SQLSCORE			8
#define	SQLITEMCODE			12


#define MAXAVATARLAYER		16		// 16레이어
#define LAYER_LENGTH		2		// 아바타 레이어 하나의 크기(2바이트)


#define SELECTAREANUM		3		// 배팅 가능 영역 수.
#define _BANKER				0
#define _PLAYER				1
#define _TIE				2

#define MAXBETTINGMULTYPLE	5		// 최고 배팅 단계
#define MINBETTINGMULTYPLE	1		// 최소(기본) 배팅 단계

#define	LEVEL_LOW			0		// 등급 - 하수
#define LEVEL_MIDDLE		1		// 등급 - 중수
#define LEVEL_HIGH			2		// 등습 - 고수

#define MAXMONEY			9999999999		// 플레이어가 가질 수 있는 최고 금액

#define PLAYERWIN_MULTIPLE	2		// 플레이어쪽이 이긴 경우에 받는 금액의 배수
#define BANKERWIN_MULTIPLE  2		// 뱅커쪽이 이긴 경우에 받는 금액의 배수
#define TIEWIN_MULTIPLE		8		// 타이쪽이 이긴 경우에 받는 금액의 배수
#define COMMISSION			0.05	// 뱅커에 걸어 이긴 경우의 공제 수수료.


#define MAXIMUMCARDNUM			52	// 카드 장수
#define	TOTALMAXCARDNUM			52	// 총 카드 숫자 
#define NOTUSECARD				-1	// 사용하지 않는 카드
#define BACCARAT_HOLDCARDNUM	3	// 훌라 게임에서 유저가 가질 수 있는 총 카드 수.
#define	EIGHTCARD				8	
#define NINECARD				9 
#define JACKCARD				11  

// 바카라에서 사용하는 카드 이미지 배열에 따른 숫자 값
const char CardToNumber[ MAXIMUMCARDNUM ] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 0, 0, 0,
											  1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 0, 0, 0,
											  1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 0, 0, 0,
											  1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 0, 0, 0,
											};


#define MAXPLAYER				5		// 방의 게임 플레이어의 인덱스를 저장할 배열의 크기이다.
#define	MINPLAYER				2

//define	GAMEREADYTIME			15		// 대기에서 게임 시작 할 때까지의 시간(초 단위)
//#define	GAMEONTIME				15		// 클라이언트에서 게임 시작 준비를 할 수 있는 시간. 
//#define	THINKTIME				10		// 유저 턴에서 최대 결정 시간.

enum
{
	RECVEOVTCP = 0,
	SENDEOVTCP,
};


// 게임의 등급의 경계 값
typedef struct
{
	__int64	LevelFrom;
	__int64 LevelTo;
}USERLEVELBOUNDARY;


// 아이템별 인식번호 
typedef struct
{
	int	iPrivateRoom;
	int	iJump;
}ITEMINDEX;



// 바카라에서에서의 레벨에 따른 배팅 금액
typedef struct
{
	int	first;
	int	middle;
	int	high;
}BASEBETTINGMONEY;


// 유저 카드패에서의 카드 위치와 카드 값
typedef struct
{
	char Pos[8];
	char value[8];
}CARDPOS_VALUE;



// 룸의 정보
typedef struct
{
	char cType;
	char cTitleLen;
	char cTitle[ MAXROOMTITLE ];
	char cPWDLen;
	char cPWD[ MAXROOMPWD ];
}ROOMINFO;



// 유저의 정보
typedef struct
{
	char	nicLen;					// 닉네임의 크기;
	char	NicName[MAXIDLENGTH];	// 게임에서 사용하는 이름.
	char	sex;					// 성별

}USERINFO;


// 유저의 게임 상태 정보
typedef struct
{
	int			win;			//	총 이긴 횟수
	int			lose;			//  총 패한 횟수
	int 		disconnect;		//  게임 포기 횟수
	__int64		money;			//	소지금 ( 최대 소지금 크기에 따라 자료형이 변경 될 수 있음)
	char		level;			//	현재 

}GAMESTATEMENT;


// 바카라에서 Banker, Player, Tie 영역에 대한 정보 
typedef struct
{
	char nCurCardNum;
	char Cards[ BACCARAT_HOLDCARDNUM ];
	char Score;
}SELECTAREAINFO;


typedef struct
{
	OVERLAPPED		ovl;
	int				mode;
}EOVERLAPPED, *LPEOVERLAPPED;


//유저의 소켓 컨텍스트
typedef struct
{
	EOVERLAPPED			eovRecvTcp,
						eovSendTcp;

	char				cRecvTcpRingBuf[ RINGBUFSIZE + MAXRECVPACKSIZE ],	// 받기 링버퍼
						*cpRTBegin,											// 버퍼의 시작 위치
						*cpRTEnd,											// 버퍼의 끝 위치
						*cpRTMark,
						cSendTcpRingBuf[ RINGBUFSIZE + MAXSENDPACKSIZE ],	// 보내기 링버퍼
						*cpSTBegin,							// 버퍼의 시작 위치
						*cpSTEnd;							// 버퍼의 끝 위치
	int					iSTRestCnt;							// 보내야 되는 크기
	SOCKET				sockTcp;							// 클라이언트 소켓
	CRITICAL_SECTION	csSTcp;								// 임계영역
	sockaddr_in			remoteAddr;							// 클라이언트 주소
	
	time_t				RecvTime;							// 클라이언트가 패킷을 받은 시간
	
	int					index;								// 유저 고유 인덱스
	
	char				idLen, cID[ MAXIDLENGTH ];		// 유저 ID의 길이, 유저 ID
													 
	char				pwdLen, cPWD[ MAXPWDLENGTH ];
				
	int					iUserDBIndex;						// DB에서의 유저 인덱스
	USERINFO			UserInfo;							// 유저의 상세 정보
	GAMESTATEMENT		GameState;							// 유저의 게임 상태
	int					ibetMoney;							// 배팅한 금액

	short				AvatarInfo[ MAXAVATARLAYER ];		// 아바타 레이어의 정보를 저장한다.
	char				AvatarType;							// 아바타 타입.

	BOOL				bCanPrivateItem,					// 비공개 방을 만들 수 있는 아이템 소지 유무 
						bCanJumpItem;						// 레벨 점프 가능 아이템 소지 유무			
	
	BOOL				bALLChat;							// 모든 유저와 대화 거부
	BOOL				bAllInvite;                         // 모든 유저와 초대 거부
	char				cPosition;							// 유저가 위치한 곳(물리적)
	char				cLogoutState;						// 유저의 종료 요청 상태.
	char				iProcess;							// 유저가 위치하는 프로세스 번호
	char				iChannel;							// 유저가 위치하는 채널 번호
	char				iOrderInRoom;						// 방에서의 유저의 순번
	int 				iRoom;								// 유저가 위치하는 방 번호

	BOOL				bAgency;							// 비 정상 종료 유무.
	char				cState;								// 유저의 상태를 저장.
	BOOL				bRequsetClose;						// 클라이언트에서 종료를 요청.
	BOOL				bReservation;						// 나가기 예약 유무.
	char				nSelectArea;						// 유저가 선택한 영역.
										
}SOCKETCONTEXT, *LPSOCKETCONTEXT;


typedef struct
{
	LPSOCKETCONTEXT			lpSockContext;
	char					*cpPacket;
}DATABASEDATA, *LPDATABASEDATA;


typedef struct
{
	int		prev, next;
}OBJECTNODE, *LPOBJECTNODE;


typedef struct
{
	int( *proc )( LPSOCKETCONTEXT lpSockContext, char* cpPacket );
}ONTRANSFUNC;

extern ONTRANSFUNC	OnTransFunc[MAXTRANSFUNC];



#endif