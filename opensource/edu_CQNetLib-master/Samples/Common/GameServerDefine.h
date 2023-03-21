/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*		File			:	GameServerDefine.h														*/                     
/*		Date			:	2006년 5월 30일															*/
/*		Author			:	Kanggoon																*/
/*		Modifications	:																			*/
/*		Notes			:	서버 관련 정의 문서														*/
/*							_RQ	:	Client -> Server로 요청											*/
/*							_AQ	:	Server -> Client로 Rq에 대한 응답								*/
/*							_CN :   Client -> Server로 알림(응답이없음)								*/
/*							_SN :	Server -> Client로 알림(응답이없음)								*/
/*		Protocol		:	Clinet	<--->	LoginServer		:	1		~	20  					*/
/*							Client	<--->	GameServer		:	21		~	80						*/
/*							공용							:	81		~	99						*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
#pragma once
/*--------------------------------------------------------------------------------------*/
//define 관련
/*--------------------------------------------------------------------------------------*/
//문자열 길이  + 1은 널스트링을 위해서
#define MAX_USERID			12 + 1
#define MAX_USERPW			12 + 1
#define MAX_USERNAME		12 + 1
#define MAX_NICKNAME		16 + 1	//닉네임
#define MAX_DATE			24 + 1  //날짜
#define MAX_ITEMMESSAGE		20 + 1	//아이템 선물 받았을때 메시지
#define MAX_GAMEROOMNAME	24 + 1  //방이름
#define MAX_LOBBYROOMNAME	24 + 1  //방이름
#define MAX_ROOMPW			12 + 1  //방 비밀번호
#define MAX_CHATMSG			100 + 1 //채팅 메시지
#define MAX_INTRO			100 + 1 //인사말
#define MAX_EMAIL			100 + 1 //EMail
#define MAX_PRESENTMSG		30 + 1  //선물 메시지

//CHECK TIME
#define CHECKTIME_LOGIN		10
#define CHECKTIME_KEEPALIVE 60

//서버
#define SERVER_TICK			500

//최대 개수	
#define MAX_CHARACTER_PARTS		15	//캐릭터가 소유할 수 있는 최대 파츠수
#define MAX_CHARACTER_INSTRUMENTSLOT	6		//캐릭터 악기 슬롯
#define MAX_PROCESSFUNC		100		//패킷 처리를 위한 함수 포인터
#define MAX_CHARACTERITEM	500		//캐릭터가 소유할 수 있는 최대 캐릭터 아이템 수
#define MAX_ROOMUSER		8		//한방에 유저가 들어갈수 있는 최대수

//최소 개수
#define MIN_ROOMUSER		2

/*--------------------------------------------------------------------------------------*/
//enum 관련
/*--------------------------------------------------------------------------------------*/
enum eErrorCode
{
	//성공에 관한 코드
	ERR_NONE					= 0x00000000,			//일반적인 모든 성공
	ERR_NONE_LOGIN				= 0x00000001,			//로긴 인증 성공		
	ERR_NONE_CREATENICKNAME		= 0x00000002,			//닉네임 생성 성공
	ERR_NONE_CREATECHARACTER	= 0x00000003,			//캐릭터 생성 성공
	ERR_NONE_CONNECTGAMESERVER	= 0x00000004,			//게임서버 접속 허용
	ERR_NONE_CREATEACCOUNT		= 0x00000005,			//계정생성 성공

	ERR_EXIST					= 0x00001000,			//에러가 있는지 체크하는 기준
	//로긴 관련 에러 코드
	ERR_LOGIN_SAMEID			= 0x00001001,			//같은 아이디가 존재합니다.
	ERR_LOGIN_SERVER			= 0x00001002,			//로긴 서버 에러
	ERR_LOGIN_ID				= 0x00001003,			//ID가 존재하지 않습니다.
	ERR_LOGIN_PW				= 0x00001004,			//PW가 틀립니다.
	ERR_LOGIN_SAMENICKNAME     	= 0x00001005,			//같은 닉네임이 존재 합니다.
	ERR_LOGIN_NOEXISTCHACRACTER	= 0x00001006,			//캐릭터가 없다. 처음 들어온 사용자 임
	ERR_LOGIN_NOEXISTNICKNAME	= 0x00001007,			//닉네임이 없다
	

	

	//로비룸 관련 에러 코드
	ERR_LOBBYROOM_FULLROOM		= 0x00001100,			//로비룸을 더이상 생성할수 없다.
	ERR_LOBBYROOM_NOTEXIST		= 0x00001101,			//로비룸을 찾을수 없습니다.
	ERR_LOBBYROOM_FULLUSER		= 0x00001102,			//사용자가 가득차서 방에 들어갈수 없습니다.
	ERR_LOBBYROOM_NOTREADY		= 0x00001103,			//준비되지 않은 사용자가 있습니다
	ERR_LOBBYROOM_ALREADYSTART  = 0x00001104,			//이미 플레이중인 방이다.
	ERR_SQL_FAILED				= 0x00009001,			//디비 에러
	
	//아이템 관련 에러 코드
	ERR_ITEM_FULLRUBYBAG		= 0x00001200,			//루비 벡슬롯이 가득차서 더이상 루비를 소지할 수 없다.
	ERR_ITEM_FULLSKILLSLOT		= 0x00001201,			//스킬 슬롯이 가득차서 더이상 스킬 아이템을 소지할 수 없다.
};

enum eGameOver
{
	GAMEOVER_ALLDEAD	= 0 ,
	GAMEOVER_TIMEOUT	= 1 ,
		
};

//BATTLE,CONCERT
enum eGameRoomMode
{
	GAMEROOMMODE_BATTLE,
	GAMEROOMMODE_CONCERT,
	
};

/*--------------------------------------------------------------------------------------*/
//struct 관련
/*--------------------------------------------------------------------------------------*/
//패킷 헤더
struct KPACKET_BASE
{
	unsigned short usLength;
	unsigned short usType;
};

struct KUDPPACKET_BASE
{
	unsigned short usLength;
	unsigned short usType;
	DWORD		   dwSequenceNo;		//UDP패킷의 순서 보장
	DWORD		   dwPKey;				//패킷을 보낸 유저의 PKey
	
	KUDPPACKET_BASE()
	{
		dwSequenceNo = 0;
		dwPKey = 0;
	}
};

//유저가 가지고 있는 캐릭터 아이템
struct CHARACTER_ITEM
{
	__int64		n64ItemKey;
	int			nItemCode;
	int			nRentalEndDate;
	
	CHARACTER_ITEM()
	{
		ZeroMemory( this , sizeof( CHARACTER_ITEM ) );
	}
};

