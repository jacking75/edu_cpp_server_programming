/*--------------------------------------------------------------------------------------------------*/
//	File			:	Protocol.h
//  Date			:	08.01.25
//  Author			:	jacking
//	Description		:	게임 프로토콜 정의
//						
//						CTL : 클라이언트와 로긴서버
//						CTG : 클라이언트와 게임서버
//						LTG : 로그인서버와 게임서버
//						STD : 서버와 DB에이전트
//						MTG	: 게임서버와 메신저서버
//						CTSM : 클라이언트와 메인 관리 서버
//						BTSM : 브릿지와 메인 관리 서버
//
//						_RQ : Client -> Server 요청
//						_AQ : Server -> Client 요청에 대한 응답
//						_CN : Client -> Server 알림 
//						_SN : Server -> Client 알림
//						_MSG : 서버간에 알림
//					
//
// 에러 코드의 범위 :	로그인 서버		: 1 ~ 30
//						로그인 DB 서버	: 32 ~ 50
//						게임서버		: 61 ~ 80
//						메신저서버		: 81 ~ 90
//						게임 DB 서버	: 91 ~ 110
/*-------------------------------------------------------------------------------------------------*/


#pragma once

#define WIN32_LEAN_AND_MEAN  
#include <windows.h>

#include "CommonDefine.h"


#define ARRAY_DATA_START	
#define ARRAY_DATA_END

//패킷 기본 헤더
struct PACKET_BASE
{
	unsigned short usLength;
	unsigned short usType;
};

enum eProtocol
{
	GTL_GAMESERVER_BASIC_INFO_CN		= 1,	// 게임서버의 기본 정보를 통보 한다. 게임서버 -> 로그인 서버
	MTG_GAMESERVER_BASIC_INFO_MSG		= 2,	// 게임서버의 기본 정보를 통보 한다. 게임서버 -> 메신저 서버
	
	//로긴 서버 관련 패킷 21 ~ 40
	CTL_LOGIN_RQ						= 21,		// 로긴 서버에 로긴을 한다고 알림
	STD_LOGIN_RQ						= 22,		// 디비 서버에 로긴을 한다고 알림
	STD_LOGININFO_AQ					= 23,		// 디비 서버에서 캐릭터 정보를 클라이언트에게 알림
	CTL_LOGIN_AQ						= 24,		// 로긴 서버에서 로긴 결과를 알림
	CTL_CHANNEL_INFO_MSG				= 25,		// 채널의 정보를 알림

	CTL_LOGIN_QUIT_FORCE_RQ				= 28,		// 강제 접속 종료 요청
	LTG_LOGIN_QUIT_FORCE_MSG			= 29,		// 게임서버에 종료 요청
	CTL_LOGIN_QUIT_FORCE_AQ				= 30,		// 클라이언트에 결과 통보.

	GTL_LOBBY_CUR_USER_COUNT_MSG	= 31,		// 채널에 있는 유저 수 통보
	
	// DB의 게임 관련 데이터 로딩
	GTD_LOAD_ITEM_RQ					= 101,		// 아이템 데이터 로딩
	DTG_LOAD_ITEM_AQ					= 102,	

	GTD_LOAD_SUBJECTIVEQUIZ_RQ			= 103,		// 주관식 문제 데이터 로딩
	DTG_LOAD_SUBJECTIVEQUIZ_AQ			= 104,
	DTG_LOAD_SUBJECTIVEQUIZ_ANSWER_AQ	= 105,		// 주관식 문제의 답변

	GTD_LOAD_SKILL_RQ					= 106,		// 스킬 로딩
	DTG_LOAD_SKILL_AQ					= 107,

	MTD_LOAD_SUBJECTIVEQUIZ_RQ			= 108,		// 주관식 퀴즈의 번호만 로딩한다.
	DTM_LOAD_SUBJECTIVEQUIZ_AQ			= 109,

	MTD_UPDATE_SUBJECTIVEQUIZ_ANSWER_RQ	= 110,		// 주관식 문제의 답변의 갱신을 요청한다.
	MTG_UPDATE_SUBJECTIVEQUIZ_ANSWER_MSG	= 111,		
	
	CTG_CHANGE_CHAR_SKILL_SLOT_RQ		= 121,		// 캐릭터 스킬 슬롯 변경
	GTC_CHANGE_CHAR_SKILL_SLOT_AQ		= 122,

	GTD_SAVE_CHARACTER_DATA_RQ			= 181,		// 캐릭터의 게임 데이터 저장
	GTD_SAVE_CHAR_SKILL_SLOT_RQ			= 182,		// 캐릭터의 스킬 슬롯 정보 저장


	// 게임 서버 관련 패킷  
	CTG_CONNECT_GAMESERVER_RQ			= 201,		// 게임 서버에 연결 요청
	GTL_CLIENT_AUTH_CONFIRM_RQ			= 202,		// 로그인 서버에 인증 요청
	LTG_CLIENT_AUTH_CONFIRM_AQ			= 203,		// 게임 서버에 인증 결과 통보
	GTD_CHARACTER_INFO_RQ				= 204,		// 게임 DB에 캐릭터 정보 요청
	DTG_CHARACTER_INFO_AQ				= 205,		// 게임 서버에 캐릭터 정보 통보
	GTC_CONNECT_GAMESERVER_AQ			= 206,		// 클라이언트에 인증 결과 및 캐릭터 정보 통보
	GTL_CLIENT_GAME_AUTH_MSG			= 207,		// 게임서버에서 클라이언트의 게임서버 인증 완료를 알림.
	GTL_LOGOUT_CLIENT_MSG				= 208,		// 유저의 로그아웃을 로그인 서버에 통보

	CTG_GAMEMENU_SELECT_RQ				= 209,		// 메뉴 선택 요청. 자랑, 상점, 인벤토리
	CTG_GAMEMENU_SELECT_AQ				= 210,		

	CTG_CHANNEL_LIST_RQ					= 211,		// (클라이언트의)채널리스트 요청
	GTL_CHANNEL_LIST_RQ					= 212,		// (게임서버의)채널리스트 요청	
	CLTG_CHANNEL_LIST_AQ				= 213,

	CTG_CHANGE_CHANNEL_RQ				= 214,		// 채널 변경 요청
	GTC_CHANGE_CHANNEL_AQ				= 215,
	GTL_CHANGE_CHANNEL_MSG				= 216,		// 클라이언트가 다른 게임서버에 접속할 예정임을 통보
	
	CTG_ENTER_LOBBY_RQ					= 220,		// 로비 입장을 요청
	GTC_ENTER_LOBBY_AQ					= 221,		// 로비 입장 요청 결과 통보
	GTC_NOTICE_LOBBY_NEW_USER_MSG		= 222,		// 로비에 있는 유저들에게 새로 들어온 유저의 정보 통보.
	GTC_NOTICE_LOBBY_OUT_USER_MSG		= 223,		// 로비에 있는 유저들에게 누군가 로비에서 나감(방입실)을 통보

	CTG_LOBBY_USER_LIST_RQ				= 224,		// 로비에 있는 유저 리스트 요청
	GTC_LOBBY_USER_LIST_AQ				= 225,
	CTG_LOBBY_ROOM_LIST_RQ				= 226,		// 로비에 있는 방 리스트 요청
	GTC_LOBBY_ROOM_LIST_AQ				= 227,

	CTG_OWN_CHARITEM_RQ				= 228,		// 캐릭터 아이템 로딩 요청
	GTD_OWN_CHARITEM_RQ				= 229,
	DTGC_OWN_CHARITEM_AQ				= 230,	

	CTG_CREATE_CHARACTER_RQ				= 231,		// 캐릭터 생성 
	GTD_CREATE_CHARACTER_RQ				= 232,
	DTG_CREATE_CHARACTER_AQ				= 233,
	GTC_CREATE_CHARACTER_AQ				= 234,

	CTG_CHANGE_CHARITEM_WEAR_RQ			= 235,		// 캐릭터 아이템 변경 요청
	GTD_CHANGE_CHARITEM_WEAR_RQ			= 236,
	DTG_CHANGE_CHARITEM_WEAR_AQ			= 237,
	GTC_CHANGE_CHARITEM_WEAR_AQ			= 238,
	GTC_CHANGE_CHARITEM_WEAR_MSG		= 248,

	CTG_DELETE_CHARITEM_RQ				= 239,		// 캐릭터 아이템 삭제 요청
	GTD_DELETE_CHARITEM_RQ				= 240,
	DTG_DELETE_CHARITEM_AQ				= 241,
	GTC_DELETE_CHARITEM_AQ				= 242,

	CTG_BUY_ITEM_RQ						= 243,		// 아이템 구매 요청
	GTD_BUY_ITEM_RQ						= 244,
	DTG_BUY_ITEM_AQ						= 245,
	GTC_BUY_ITEM_AQ						= 246,
	
	CTG_LOBBY_CREATE_ROOM_RQ			= 251,		// 방 만들기
	GTC_LOBBY_CREATE_ROOM_AQ			= 252,
	GTC_LOBBY_CREATE_ROOM_MSG			= 253,

	CTG_LOBBY_JOIN_ROOM_RQ				= 254,		// 방 들어가기
	GTC_LOBBY_JOIN_ROOM_AQ				= 255,
	GTC_ROOM_NEW_USER_MSG				= 256,		// 새로운 유저가 방에 들어왔음을 방에 있는 사람들에게 통보

	CTG_ROOM_LEAVE_RQ					= 257,		// 방 나가기 요청 
	GTC_ROOM_LEAVE_AQ					= 258,		 
	GTC_ROOM_LEAVE_USER_MSG				= 259,		// 방의 사람이 나갔음을 방에 있는 사람에게 알림
	GTC_DESTORY_ROOM_MSG				= 260,		// 방이 없어짐을 로비의 사람에게 알림

	CTG_LOBBY_CHAT_RQ					= 261,		// 로비 - 채팅
	GTC_LOBBY_CHAT_MSG					= 262,

	CTG_ROOM_CHAT_RQ					= 263,		// 방 - 채팅
	GTC_ROOM_CHAT_MSG					= 264,

	CTG_ROOM_MOVE_RQ					= 265,		// 방 - 이동
	GTC_ROOM_MOVE_MSG					= 266,

	CTG_USER_INFO_RQ					= 267,		// 로비 - 유저 정보 보기
	GTC_USER_INFO_AQ					= 268,	

	CTG_SEND_NOTE_RQ					= 269,		// 로비 - 쪽지 보내기
	GTC_SEND_NOTE_ERR_AQ				= 270,
	GTC_SEND_NOTE_MSG					= 271,

	CTG_CHAT_1ON1_REQUEST_RQ			= 272,		// 로비 - 1:1 채팅 시작 요청
	GTC_CHAT_1ON1_ACCEPT_RQ				= 273,
	CTG_CHAT_1ON1_ACCEPT_AQ				= 274,
	GTC_CHAT_1ON1_REQUEST_AQ			= 275,
	CTG_CHAT_1ON1_RQ					= 276,		// 로비 - 1:1 채팅
	GTC_CHAT_1ON1_MSG					= 277,
	CTG_CHAT_1ON1_CLOSE_RQ				= 278,		// 로비 - 1:1 채팅 창 닫음
	GTC_CHAT_1ON1_CLOSE_MSG				= 279,

	GTD_OWN_CHAR_DETAIL_RQ				= 280,		// 캐릭터 상세 정보 로딩
	DTG_OWN_CHAR_DETAIL_AQ				= 281,
	GTC_OWN_CHAR_DETAIL_MSG				= 282,

	CTG_CREATE_CHAR_DETAIL_RQ			= 283,		// 캐릭터 상세 정보 생성
	GTD_CREATE_CHAR_DETAIL_RQ			= 284,
	DTG_CREATE_CHAR_DETAIL_AQ			= 285,
	GTC_CREATE_CHAR_DETAIL_AQ			= 286,

	CTG_CHANGE_CHAR_DETAIL_RQ			= 287,		// 캐릭터 상세 정보 변경
	GTD_CHANGE_CHAR_DETAIL_RQ			= 288,
	DTG_CHANGE_CHAR_DETAIL_AQ			= 289,
	GTC_CHANGE_CHAR_DETAIL_AQ			= 290,

	GTD_OWN_CHAR_SKILL_SLOT_RQ			= 291,		// 캐릭터 스킬 슬롯 정보 로딩
	DTG_OWN_CHAR_SKILL_SLOT_AQ			= 292,
	GTC_OWN_CHAR_SKILL_SLOT_MSG			= 293,

	CTG_ROOM_GAME_READY_RQ				= 301,		// 게임 준비 완료/해제 요청
	GTC_ROOM_GAME_READY_FAILED			= 302,		
	GTC_ROOM_GAME_READY_MSG				= 303,		

	CTG_ROOM_GAME_START_RQ				= 304,		// 게임
	GTC_ROOM_GAME_START_FAILED			= 305,		
	GTC_ROOM_GAME_START_MSG				= 306,		

	GTC_ROOM_CHANGE_STATE_MSG			= 307,		// 방 상태 변경 통보

	CTG_ROOM_GAME_TRY_ANSWER_RQ			= 308,		// 정답 도전 요청
	GTC_ROOM_GAME_TRY_ANSWER_MSG		= 309,
	
	CTG_ROOM_GAME_VOTE_RQ				= 310,		// 게임에서 투표를 한다.
	GTD_ROOM_GAME_VOTE_RQ				= 311,
	DTG_ROOM_GAME_VOTE_AQ				= 312,
	GTC_ROOM_GAME_VOTE_AQ				= 313,
	GTC_ROOM_GAME_VOTE_MSG				= 314,

	GTC_ROOM_GAME_END_RESULT_MSG		= 315,		// 게임 결과 통보

	GTC_ROOM_GAME_QUESTION_RESEARCH_LIST_MSG = 316,	// 설문 조사 선택 문항
	CTG_ROOM_GAME_QUESTION_RESEARCH_SELECT_RQ = 318, // 설문 조사 선택 문항 선택 
	GTD_ROOM_GAME_QUESTION_RESEARCH_SELECT_RQ = 319,
	DTG_ROOM_GAME_QUESTION_RESEARCH_SELECT_AQ = 320,
	GTC_ROOM_GAME_QUESTION_RESEARCH_SELECT_AQ = 321, 

	GTC_ROOM_GAME_LIFE_TIME_OVER_MSG	= 322,		// 유저의 라이프 타임이 초과 했음을 통보

	GTC_ROOM_GAME_INTERRUPTION_MSG		= 323,		// 게임 중단 통보

	CTG_ROOM_GAME_COPULE_CONSENT_RQ		= 324,	// (게임에서) 커플 승낙
	GTC_ROOM_GAME_COPULE_CONSENT_AQ		= 325,	
	GTC_ROOM_GAME_COPULE_CONSENT_MSG	= 326,

	CTG_VOTE_GOODFEEL_RQ				= 330,		// 호감도 추천
	GTD_VOTE_GOODFEEL_RQ				= 331,
	DTG_VOTE_GOODFEEL_AQ				= 332,
	GTC_VOTE_GOODFEEL_AQ				= 333,
	GTC_VOTE_GOODFEEL_MSG				= 334,

	CTG_USE_SKILL_RQ					= 336,		// 스킬 사용
	GTC_USE_SKILL_AQ					= 337,
	GTC_USE_SKILL_MSG					= 338,

	CTG_ALL_LISTEN_MUSIC_RQ				= 341,		// (방의)전체 음악 듣기
	GTC_ALL_LISTEN_MUSIC_AQ				= 342,
	GTC_ALL_LISTEN_MUSIC_MSG			= 343,
	
	CTG_ALL_LISTEN_MUSIC_SYNONYMY_RQ	= 344,	// (방의)전체 음악 듣기 - 승낙/취소
	GTC_ALL_LISTEN_MUSIC_SYNONYMY_AQ	= 345,

	CTG_ALL_LISTEN_MUSIC_END_RQ			= 346,	// (방의)전체 음악 듣기 - 종료
	GTC_ALL_LISTEN_MUSIC_END_AQ			= 347,
	GTC_ALL_LISTEN_MUSIC_END_MSG			= 348,

	CTG_CHARGE_CHARITEM_RQ				= 351,		// 캐릭터 아이템 충전(재 구매)
	GTD_CHARGE_CHARITEM_RQ				= 352,
	DTG_CHARGE_CHARITEM_AQ				= 353,
	GTC_CHARGE_CHARITEM_AQ				= 354,

	CTG_ROOM_BAN_CHAR_RQ					= 361,	// 방에서 강제 퇴장
	GTC_ROOM_BAN_CHAR_AQ					= 362,
	GTC_ROOM_BAN_CHAR_MSG				= 363,

	

	///// 801 번부터는 메신저를 위한 패킷이다.
	GTM_LOGIN_CLIENT_GAME_MSG			= 801,		// 유저의 게임서버 로그인
	GTM_CHANGE_CHANNEL_MSG				= 802,		// 유저의 채널 변경 통보
	GTM_LOGOUT_CLIENT_MSG				= 803,		// 유저의 게임서버 로그아웃

	CTG_FRIEND_LIST_RQ					= 804,		// 친구 리스트 요청
	GTM_FRIEND_LIST_RQ					= 805,
	MTD_FRIEND_LIST_RQ					= 806,
	DTM_FRIEND_LIST_AQ					= 807,
	MTG_FRIEND_LIST_AQ					= 808,
	GTC_FRIEND_LIST_AQ					= 809,

	MTG_FRIEND_LOGIN_STATE_MSG			= 810,		// 친구들의 로그인 상태를 얻는다.
	GTC_FRIEND_LOGIN_STATE_MSG			= 811,

	MTG_LOGIN_FRIEND_MSG				= 812,		// 친구의 로그인 통보
	GTC_LOGIN_FRIEND_MSG				= 813,

	MTG_LOGOUT_FRIEND_MSG				= 814,		// 친구의 로그아웃 통보
	GTC_LOGOUT_FRIEND_MSG				= 815,

	CTG_NEW_FRIEND_RQ					= 816,		// 친구 추가 요청
	GTM_NEW_FRIEND_RQ					= 817,
	MTD_NEW_FRIEND_RQ					= 818,
	DTM_NEW_FRIEND_AQ					= 819,
	MTG_NEW_FRIEND_AQ					= 820,
	GTC_NEW_FRIEND_AQ					= 821,

	MTG_NOTICE_NEW_FRIEND_MSG			= 822,		// 친구로 추가됨을 통보
	GTC_NOTICE_NEW_FRIEND_MSG			= 823,
	CTG_ACCEPT_NEW_FRIEND_RQ			= 824,		// 친구 수락 요청
	GTM_ACCEPT_NEW_FRIEND_RQ			= 825,
	MTD_ACCEPT_NEW_FRIEND_RQ			= 826,
	DTM_ACCEPT_NEW_FRIEND_AQ			= 827,
	MTG_ACCEPT_NEW_FRIEND_AQ			= 828,
	CTG_ACCEPT_NEW_FRIEND_AQ			= 829,

	CTG_DELETE_FRIEND_RQ				= 830,		// 친구 삭제
	GTM_DELETE_FRIEND_RQ				= 831,
	MTD_DELETE_FRIEND_RQ				= 832,
	DTM_DELETE_FRIEND_AQ				= 833,
	MTG_DELETE_FRIEND_AQ				= 834,
	GTC_DELETE_FRIEND_AQ				= 835,



	//// 1001 번부터는 서버 관리를 위한 패킷이다.
	SMTD_LOAD_BRIDGE_INFO_RQ			= 1001,
	SMTD_LOAD_BRIDGE_INFO_AQ			= 1002,
	SMTD_LOAD_APPSERVER_INFO_RQ			= 1003,
	SMTD_LOAD_APPSERVER_INFO_AQ			= 1004,
	
	BTSM_CONNECT_BRIDGE_RQ				= 1005,		// 브릿지의 메인관리 서버 접속
	BTSM_CONNECT_BRIDGE_AQ				= 1006,

	CTSM_CONNECT_CLIENT_RQ				= 1007,		// 클라이언트의 메인관리 서버 접속
	SMTD_CONNECT_CLIENT_RQ				= 1008,
	SMTD_CONNECT_CLIENT_AQ				= 1009,
	SMTC_CONNECT_CLIENT_AQ				= 1010,

	CTSM_SERVER_LIST_RQ					= 1011,		// 서버 리스트 요청
	SMTC_SERVER_LIST_AQ					= 1012,

	CTSM_START_SERVER_RQ				= 1013,		// 서버 시작 요청
	BTSM_START_SERVER_RQ				= 1014,		// 서버 시작 요청

	BTSM_NOTICE_SERVER_STATUS_MSG		= 1015,		// 서버 상태 통보
	CTSM_NOTICE_SERVER_STATUS_MSG		= 1016,		// 서버 상태 통보

	CTSM_TERMINATE_SERVER_RQ			= 1017,		// 서버 종료 요청
	SMTB_TERMINATE_SERVER_RQ			= 1018,

	SMTC_CLOSE_BRIDGE_MSG				= 1019,		// 브릿지와의 연결이 끊어짐을 통보

	CTSM_NOTICE_RQ							= 1021,		// 공지 요청
	SMTB_NOTICE_RQ							= 1022,
	
	CTSM_CHANGE_SERVER_STATE_RQ	= 1023,		// 서버 상태 변경	
	SMTB_CHANGE_SERVER_STATE_RQ		= 1024,

	//// 1451 번 부터는 운영 용 패킷이다.
	GTC_MANAGE_NOTICE_MSG				= 1451,		// 공지 메세지


	//// 1601 번부터는 개발자, 관리자, GM용 패킷이다
	CTG_DEV_SERVER_DATA_RELOAD_RQ	= 1601,	// 게임서버의 DB 데이터 재 로딩 요청
	GTC_DEV_SERVER_DATA_RELOAD_AQ	= 1602,

	CTL_DEV_LOGIN_CLOSE_RQ			= 1603,	// 로그인 서버와의 접속 종료 요청


	///// 1801 번부터는 치팅을 위한 패킷이다.
	CTG_CHEAT_CHARACTER_INFO_RQ		= 1801,		// 캐릭터 정보 변경
	GTC_CHEAT_CHARACTER_INFO_AQ		= 1802,

	CTG_CHEAT_ROOM_GAME_RQ				= 1803,		// 게임 중 게임 상태 변경
	GTC_CHEAT_ROOM_GAME_AQ				= 1804,

	CTG_CHEAT_REQ_SUBJECTIVE_QUIZ_ANSWER_RQ = 1805, // 주관식 퀴즈 답변 요구
	GTC_CHEAT_REQ_SUBJECTIVE_QUIZ_ANSWER_AQ = 1806, 

};


#pragma pack(1)

// [[게임서버의 기본 정보를 통보 한다. 게임서버 -> 로그인 서버]]
// [ GTL_GAMESERVER_BASIC_INFO_CN]	  
struct VBUFFER_GAMESERVER_BASIC_INFO_CN {       
	short sPort;						// 클라이언트 접속 포트 번호
	short sServerIndex;					// 게임서버의 인덱스
	char acServerName[ 1 ];				// 게임서버의 이름
	short sChannelCount;				// 채널 개수
	short asMaxChannelUserCount[ 1 ];	// 채널의 최대 유저 인원수
};

// [- MTG_GAMESERVER_BASIC_INFO_MSG]
//struct MTG_PACKET_GAMESERVER_BASIC_INFO_MSG : public PACKET_BASE
//{
//	short sPort;								// 클라이언트 접속 포트 번호
//	short sServerIndex;							// 게임서버의 인덱스
//	char acServerName[ MAX_SERVER_NAME_LEN ];	// 게임서버의 이름
//	short sChannelCount;						// 채널 개수
//	short sMaxUserCount;						// 최대 유저 인원
//
//	MTG_PACKET_GAMESERVER_BASIC_INFO_MSG()
//	{
//		usLength = sizeof( MTG_PACKET_GAMESERVER_BASIC_INFO_MSG );
//		usType = MTG_GAMESERVER_BASIC_INFO_MSG;
//	}
//};

// [[로긴 서버에 로긴을 한다고 알림]]
// [- CTL_LOGIN_RQ]								
struct CTL_PACKET_LOGIN_RQ : public PACKET_BASE
{
	char	acUserID[ MAX_USERID ];
	char	acUserPW[ MAX_USERPW ];

	CTL_PACKET_LOGIN_RQ()
	{
		usLength = sizeof( CTL_PACKET_LOGIN_RQ );
		usType = CTL_LOGIN_RQ;
	}
};
// [- STD_LOGIN_RQ]	
struct STD_PACKET_LOGIN_RQ : public PACKET_BASE
{
	char	acUserID[ MAX_USERID ];
	char	acUserPW[ MAX_USERPW ];

	//해당 클라이언트를 식별하기 위한 소켓 ID
	DWORD		dwConnKey;

	STD_PACKET_LOGIN_RQ()
	{
		usLength = sizeof( STD_PACKET_LOGIN_RQ );
		usType = STD_LOGIN_RQ;
	}
};
// [- STD_LOGININFO_AQ]
const char LOGININFO_AQ_RESULT_DUPLICATION = 25;	// 중복 접속.
struct STD_PACKET_LOGININFO_AQ : public PACKET_BASE
{
	DWORD	dwResult;							// 결과 코드
	UINT64 i64UKey;							// 계정 고유 키
	char	acUserID[ MAX_USERID ];				// ID
	char	cAdminLevel;						// 관리자 등급
	
	//해당 클라이언트를 식별하기 위한 커넥션 키
	DWORD	dwConnKey;

	STD_PACKET_LOGININFO_AQ()
	{
		usLength = sizeof( STD_PACKET_LOGININFO_AQ );
		usType = STD_LOGININFO_AQ;
	}
};

// [- CTL_LOGIN_AQ]
struct CTL_PACKET_LOGIN_AQ : public PACKET_BASE
{
	DWORD	dwResult;					// 결과
	UINT64	i64UKey;						// 계정 고유 키
	char acSecureString[ LEN_SECURE_STR ];// 보안문자
	char	cSex;						// 성별
	char	cAdminLevel;

	CTL_PACKET_LOGIN_AQ()
	{
		usLength = sizeof( CTL_PACKET_LOGIN_AQ );
		usType = CTL_LOGIN_AQ;
	}
};
//// [ CTL_CHANNEL_INFO_MSG]
//struct VBUFFER_CHANNEL_INFO_MSG {     
//	short sChannelCount;				// 총 채널 개수
//	ARRAY_DATA_START
//		short sServerIndex[ 1 ];			// 채널이 속한 서버의 인덱스
//		short sChannelMaxUserCount[ 1 ];	// 채널의 최대 접속 유저 수
//		short sChannelCurUserCount[ 1 ];	// 채널의 현재 접속 유저 수
//	ARRAY_DATA_END
//
//	short sServerAddressCount;
//	ARRAY_DATA_START
//		short	sServerIndex2[ 1 ];		// 채널이 속한 서버의 인덱스
//		char	acIP[ 1 ];
//	    short	sPort;
//	ARRAY_DATA_END
//};

// [[강제 접속 종료 요청]]
// [- CTL_LOGIN_QUIT_FORCE_RQ]
struct CTL_PACKET_LOGIN_QUIT_FORCE_RQ : public PACKET_BASE
{
	CTL_PACKET_LOGIN_QUIT_FORCE_RQ()
	{
		usLength = sizeof( CTL_PACKET_LOGIN_QUIT_FORCE_RQ );
		usType = CTL_LOGIN_QUIT_FORCE_RQ;
	}
};
//// [- LTG_LOGIN_QUIT_FORCE_MSG]
//struct LTG_PACKET_LOGIN_QUIT_FORCE_MSG : public PACKET_BASE
//{
//	UINT64	i64UKey;
//	UINT64	i64CharCd;
//
//	LTG_PACKET_LOGIN_QUIT_FORCE_MSG()
//	{
//		usLength = sizeof( LTG_PACKET_LOGIN_QUIT_FORCE_MSG );
//		usType = LTG_LOGIN_QUIT_FORCE_MSG;
//	}
//};
// [- CTL_LOGIN_QUIT_FORCE_AQ]
struct CTL_PACKET_LOGIN_QUIT_FORCE_AQ : public PACKET_BASE
{
	char cResult;

	CTL_PACKET_LOGIN_QUIT_FORCE_AQ()
	{
		usLength = sizeof( CTL_PACKET_LOGIN_QUIT_FORCE_AQ );
		usType = CTL_LOGIN_QUIT_FORCE_AQ;
	}
};


// [[ 채널에 있는 유저 수 통보 ]]
// [ GTL_LOBBY_CUR_USER_COUNT_MSG ]
struct GTL_PACKET_LOBBY_CUR_USER_COUNT_MSG : public PACKET_BASE
{
	short sGameServerIndex;		// 게임서버 인덱스
	short sChannelNum;			// 채널 번호
	short sUserCount;				// 채널에 있는 유저 수

	GTL_PACKET_LOBBY_CUR_USER_COUNT_MSG()
	{
		usLength = sizeof( GTL_PACKET_LOBBY_CUR_USER_COUNT_MSG );
		usType = GTL_LOBBY_CUR_USER_COUNT_MSG;
	}
};


//// [[ 게임서버 접속 ]]
//// [- CTG_CONNECT_GAMESERVER_RQ]
//struct CTG_PACKET_CONNECT_GAMESERVER_RQ : public PACKET_BASE
//{
//	UINT64	i64UKey;						// 계정 고유 키
//	char acSecureStr[ LEN_SECURE_STR ];	// 보안 문자열
//	short sChannelNum;					// 들어가기를 원하는 채널 번호
//
//	CTG_PACKET_CONNECT_GAMESERVER_RQ()
//	{
//		usLength = sizeof( CTG_PACKET_CONNECT_GAMESERVER_RQ );
//		usType = CTG_CONNECT_GAMESERVER_RQ;
//	}
//};

// [- GTL_CLIENT_AUTH_CONFIRM_RQ]
struct GTL_PACKET_CLIENT_AUTH_CONFIRM_RQ : public PACKET_BASE
{
	UINT64	i64UKey;						// 계정 고유 키
	DWORD   dwGameConnectKey;				
	char acSecureStr[ LEN_SECURE_STR ];	// 보안 문자열
	short sChannelNum;					// 들어가기를 원하는 채널 번호. 0부터 시작.

	GTL_PACKET_CLIENT_AUTH_CONFIRM_RQ()
	{
		usLength = sizeof( GTL_PACKET_CLIENT_AUTH_CONFIRM_RQ );
		usType = GTL_CLIENT_AUTH_CONFIRM_RQ;
	}
};

// [- LTG_CLIENT_AUTH_CONFIRM_AQ]
struct LTG_PACKET_CLIENT_AUTH_CONFIRM_AQ : public PACKET_BASE
{
	char	cResult;			// 결과
	UINT64	i64UKey;			// 계정 고유 키
	DWORD   dwGameConnectKey;				
	short	sChannelNum;		// 들어가기를 원하는 채널 번호
	char	acID[ MAX_USERID ];
	char	cAdminLevel;		// 관리자 레벨
	char	cSex;				
	short	sAge;

	LTG_PACKET_CLIENT_AUTH_CONFIRM_AQ()
	{
		usLength = sizeof( LTG_PACKET_CLIENT_AUTH_CONFIRM_AQ );
		usType = LTG_CLIENT_AUTH_CONFIRM_AQ;
	}
};

//// [- GTD_CHARACTER_INFO_RQ]
//struct GTD_PACKET_CHARACTER_INFO_RQ : public PACKET_BASE
//{
//	UINT64	i64UKey;				// 계정 고유 키
//	DWORD   dwGameConnectKey;				
//	short sChannelNum;			// 들어가기를 원하는 채널 번호
//	char acID[ MAX_USERID ];
//	
//	GTD_PACKET_CHARACTER_INFO_RQ()
//	{
//		usLength = sizeof( GTD_PACKET_CHARACTER_INFO_RQ );
//		usType = GTD_CHARACTER_INFO_RQ;
//	}
//};
//
//// [- DTG_CHARACTER_INFO_AQ]
//const char CHARACTER_INFO_RET_NONE_CHARACTER = 92;
//struct DTG_PACKET_CHARACTER_INFO_AQ : public PACKET_BASE
//{
//	char cResult;				// 결과
//	UINT64	i64UKey;				// 계정 고유 키
//	DWORD   dwGameConnectKey;				
//	short sChannelNum;			// 들어가기를 원하는 채널 번호
//
//	UINT64	i64CharCd;				// 캐릭터 코드
//	char acName[ MAX_NICKNAME ];	// 캐릭터 이름
//	char cSex;					// 성별. 남자(1), 여자(그 이외)
//	short sLv;					// 레벨
//	INT32 iExp;					// 경험치
//	INT32 iPopularity;			// 인기
//	INT64		i64Cash;					// 캐쉬
//	INT64		i64GameMoney;				// 게임머니
//	char cBloodType;			// 혈액형
//	
//	short sMaxWearItemCount;		// 현재 가질수 있는 최대 복장 아이템 개수
//	short sMaxGameItemCount;		// 현재 가질수 있는 최대 게임 아이템 개수
//	short sMaxStageItemCount;		// 현재 가질수 있는 최대 무대 아이템 개수
//	short sMaxArrangementItemCount;	// 현재 가질수 있는 최대 배치 아이템 개수	
//
//	UINT64	i64CapCharItemCd;		// 모자 캐릭터아이템 코드
//	UINT64	i64HeadCharItemCd;		// 머리 캐릭터아이템 코드
//	UINT64	i64FaceCharItemCd;		// 얼굴 캐릭터아이템 코드
//	UINT64	i64CoatCharItemCd;		// 상의 캐릭터아이템 코드
//	UINT64	i64ArmCharItemCd;		// 팔 캐릭터아이템 코드
//	UINT64	i64HandCharItemCd;		// 손 캐릭터아이템 코드
//	UINT64	i64PantCharItemCd;		// 하의 캐릭터아이템 코드
//	UINT64	i64LegCharItemCd;		// 다리 캐릭터아이템 코드
//	UINT64	i64ShoeCharItemCd;		// 구두 캐릭터아이템 코드
//	UINT64	i64SetsCharItemCd;		// 세트 캐릭터아이템 코드
//	UINT64	i64EyeCharItemCd;		// 눈 캐릭터아이템 코드
//	UINT64	i64BackCharItemCd;		// 등 캐릭터아이템 코드
//	UINT64	i64CarriageCharItemCd;	// 탈것 캐릭터아이템 코드
//	UINT64	i64PetCharItemCd;		// 펫 캐릭터아이템 코드
//
//	char	cGoodFeelCardRecvDay;	// 호감도 추천 표를 받은 날짜
//	short	sGoodFeelCardCount;		// 호감도 추천 표 개수.
//
//	UINT64	i64Eye2CharItemCd;		// 눈 부위 캐릭터아이템 코드
//	UINT64	i64EarCharItemCd;		// 귀 캐릭터아이템 코드
//	UINT64	i64RightHandCharItemCd;	// 오른 손 캐릭터아이템 코드
//	UINT64	i64LeftHandCharItemCd;	// 왼 손 캐릭터아이템 코드
//	UINT64	i64WaistCharItemCd;		// 허리 캐릭터아이템 코드
//
//	UINT32	TotalConnectedTime;	// 총 접속 시간(초).
//	
//	DTG_PACKET_CHARACTER_INFO_AQ()
//	{
//		usLength = sizeof( DTG_PACKET_CHARACTER_INFO_AQ );
//		usType = DTG_CHARACTER_INFO_AQ;
//	}
//};
//
//// 캐릭터가 이미 중복 되어 있음
//const char GTC_CONNECT_GAMESERVER_ERR_AQ_DUPP_USER = 75;
//// 캐릭터를 채널에 넣지 못했음.
//const char GTC_CONNECT_GAMESERVER_AQ_ERR_ADD_CHANNEL = 76;
//// [- GTC_CONNECT_GAMESERVER_AQ]
//struct GTC_PACKET_CONNECT_GAMESERVER_AQ : public PACKET_BASE
//{
//	char cResult;				// 결과 값이 성공이 아닌 경우 CHARACTER_INFO_RET_NONE_CHARACTER 라면 
//								// 캐릭터가 없는 것이므로 생성해야 된다.
//	short sChannelNum;			// 들어가기를 원하는 채널 번호
//	UINT64	i64CharCd;				// 캐릭터 코드. 
//	char acName[ MAX_NICKNAME ];	// 캐릭터 이름
//	char cSex;					// 성별. 남자(1), 여자(그 이외)
//	short sLv;					// 레벨
//	INT32 iExp;					// 경험치
//	INT32 iPopularity;			// 인기
//	INT64		i64Cash;			// 캐쉬
//	INT64		i64GameMoney;		// 게임머니
//	char cBloodType;			// 혈액형
//	
//	short sMaxWearItemCount;		// 현재 가질수 있는 최대 복장 아이템 개수
//	short sMaxGameItemCount;		// 현재 가질수 있는 최대 게임 아이템 개수
//	short sMaxStageItemCount;		// 현재 가질수 있는 최대 무대 아이템 개수
//	short sMaxArrangementItemCount;	// 현재 가질수 있는 최대 배치 아이템 개수
//
//	UINT64	i64CapCharItemCd;		// 모자 캐릭터아이템 코드
//	UINT64	i64HeadCharItemCd;		// 머리 캐릭터아이템 코드
//	UINT64	i64FaceCharItemCd;		// 얼굴 캐릭터아이템 코드
//	UINT64	i64CoatCharItemCd;		// 상의 캐릭터아이템 코드
//	UINT64	i64ArmCharItemCd;		// 팔 캐릭터아이템 코드
//	UINT64	i64HandCharItemCd;		// 손 캐릭터아이템 코드
//	UINT64	i64PantCharItemCd;		// 하의 캐릭터아이템 코드
//	UINT64	i64LegCharItemCd;		// 다리 캐릭터아이템 코드
//	UINT64	i64ShoeCharItemCd;		// 구두 캐릭터아이템 코드
//	UINT64	i64SetsCharItemCd;		// 세트 캐릭터아이템 코드
//	UINT64	i64EyeCharItemCd;		// 눈 캐릭터아이템 코드
//	UINT64	i64BackCharItemCd;		// 등 캐릭터아이템 코드
//	UINT64	i64CarriageCharItemCd;	// 탈것 캐릭터아이템 코드
//	UINT64	i64PetCharItemCd;		// 펫 캐릭터아이템 코드
//
//	short	sGoodFeelCardCount;		// 호감도 추천 표 개수.
//	UINT32	uiCurSecondTime;		// 현재 시간(초 단위)
//
//	UINT64	i64Eye2CharItemCd;		// 눈 부위 캐릭터아이템 코드
//	UINT64	i64EarCharItemCd;		// 귀 캐릭터아이템 코드
//	UINT64	i64RightHandCharItemCd;	// 오른 손 캐릭터아이템 코드
//	UINT64	i64LeftHandCharItemCd;	// 왼 손 캐릭터아이템 코드
//	UINT64	i64WaistCharItemCd;		// 허리 캐릭터아이템 코드
//	
//	GTC_PACKET_CONNECT_GAMESERVER_AQ()
//	{
//		usLength = sizeof( GTC_PACKET_CONNECT_GAMESERVER_AQ );
//		usType = GTC_CONNECT_GAMESERVER_AQ;
//	}
//};

// [- GTL_CLIENT_GAME_AUTH_MSG]
struct GTL_PACKET_CLIENT_GAME_AUTH_MSG : public PACKET_BASE
{
	short sGameServerIndex;		// 게임서버 인덱스
	short sChannelNum;			// 유저가 들어간 채널 번호
	UINT64	i64UKey;				// 계정 고유 키
	UINT64	i64CharCd;			// 캐릭터 코드

	GTL_PACKET_CLIENT_GAME_AUTH_MSG()
	{
		usLength = sizeof( GTL_PACKET_CLIENT_GAME_AUTH_MSG );
		usType = GTL_CLIENT_GAME_AUTH_MSG;
	}
};

// [- GTL_LOGOUT_CLIENT_MSG]
struct GTL_PACKET_LOGOUT_CLIENT_MSG : public PACKET_BASE
{
	short sGameServerIndex;		// 게임서버 인덱스
	short sChannelNum;			// 유저가 들어간 채널 번호
	UINT64	i64UKey;				// 계정 고유 키
	UINT64	i64CharCd;

	GTL_PACKET_LOGOUT_CLIENT_MSG()
	{
		usLength = sizeof( GTL_PACKET_LOGOUT_CLIENT_MSG );
		usType = GTL_LOGOUT_CLIENT_MSG;
	}
};

//// [- CTG_GAMEMENU_SELECT_RQ]
//struct CTG_PACKET_GAMEMENU_SELECT_RQ : public PACKET_BASE
//{
//	short	sSelectNum;
//
//	CTG_PACKET_GAMEMENU_SELECT_RQ()
//	{
//		usLength = sizeof( CTG_PACKET_GAMEMENU_SELECT_RQ );
//		usType = CTG_GAMEMENU_SELECT_RQ;
//	}
//};
//
//// [- CTG_GAMEMENU_SELECT_AQ]
//struct CTG_PACKET_GAMEMENU_SELECT_AQ : public PACKET_BASE
//{
//	char	cResult;
//	short	sSelectNum;
//
//	CTG_PACKET_GAMEMENU_SELECT_AQ()
//	{
//		usLength = sizeof( CTG_PACKET_GAMEMENU_SELECT_AQ );
//		usType = CTG_GAMEMENU_SELECT_AQ;
//	}
//};
//
//
//// [[ 채널 리스트 요청]]
//// [- CTG_CHANNEL_LIST_RQ]
//struct CTG_PACKET_CHANNEL_LIST_RQ : public PACKET_BASE
//{
//	CTG_PACKET_CHANNEL_LIST_RQ()
//	{
//		usLength = sizeof( CTG_PACKET_CHANNEL_LIST_RQ );
//		usType = CTG_CHANNEL_LIST_RQ;
//	}
//};
// [- GTL_CHANNEL_LIST_RQ]
struct GTL_PACKET_CHANNEL_LIST_RQ : public PACKET_BASE
{
	UINT64	i64CharCd;
	short	sCurChannelNum;
	short	sSeqCdInLobby;

	GTL_PACKET_CHANNEL_LIST_RQ()
	{
		usLength = sizeof( GTL_PACKET_CHANNEL_LIST_RQ );
		usType = GTL_CHANNEL_LIST_RQ;
	}
};
//// [- CLTG_CHANNEL_LIST_AQ]
//struct VBUFFER_CLTG_CHANNEL_LIST_AQ { 
//	UINT64	i64CharCd;
//	short	sCurChannelNum;
//	short	sSeqCdInLobby;
//
//	short sCount;	// 아래의 것들이 sCount 수만큼 인코딩 된다.
//	ARRAY_DATA_START
//	short sServerIndex[ 1 ];				// 채널이 속한 서버의 인덱스
//	short sChannelMaxUserCount[ 1 ];		// 채널의 최대 접속 유저 수
//	short sChannelCurUserCount[ 1 ];		// 채널의 현재 접속 유저 수
//	ARRAY_DATA_END
//};
//
//
//// [- CTG_ENTER_LOBBY_RQ]
//struct CTG_PACKET_ENTER_LOBBY_RQ : public PACKET_BASE
//{
//	CTG_PACKET_ENTER_LOBBY_RQ()
//	{
//		usLength = sizeof( CTG_PACKET_ENTER_LOBBY_RQ );
//		usType = CTG_ENTER_LOBBY_RQ;
//	}
//};
//
//// [- GTC_ENTER_LOBBY_AQ]		
//struct GTC_PACKET_ENTER_LOBBY_AQ : public PACKET_BASE
//{
//	char cResult;
//	short sSeqCdInLobby;		// 로비(채널)에서의 순서 번호
//	short sChannelNum;			// 채널 번호
//
//	GTC_PACKET_ENTER_LOBBY_AQ()
//	{
//		usLength = sizeof( GTC_PACKET_ENTER_LOBBY_AQ );
//		usType = GTC_ENTER_LOBBY_AQ;
//	}
//};
//
//// [- GTC_NOTICE_LOBBY_NEW_USER_MSG]	
//struct GTC_PACKET_NOTICE_LOBBY_NEW_USER_MSG : public PACKET_BASE
//{
//	UINT64	i64CharCd;			// 캐릭터 코드
//	short sSeqCdInLobby;		// 로비(채널)에서의 순서 번호
//	char cSex;					// 성별
//	short sLevel;				// 레벨
//	char acName[ MAX_NICKNAME ];	// 이름
//
//	GTC_PACKET_NOTICE_LOBBY_NEW_USER_MSG()
//	{
//		usLength = sizeof( GTC_PACKET_NOTICE_LOBBY_NEW_USER_MSG );
//		usType = GTC_NOTICE_LOBBY_NEW_USER_MSG;
//	}
//};
//
//// [- GTC_NOTICE_LOBBY_OUT_USER_MSG]
//struct GTC_PACKET_NOTICE_LOBBY_OUT_USER_MSG : public PACKET_BASE
//{
//	UINT64	i64CharCd;			// 캐릭터 코드
//	short sSeqCdInLobby;		// 로비(채널)에서의 순서 번호
//	char acName[ MAX_NICKNAME ];	// 이름
//	char	cIsLobbyOut;		// 1 이면 로비를 완전히 나가는 것이다.
//								// 방에 들어가는 가는 것은 로비를 완전히 나가는 것은 아니다.
//
//	GTC_PACKET_NOTICE_LOBBY_OUT_USER_MSG()
//	{
//		usLength = sizeof( GTC_PACKET_NOTICE_LOBBY_OUT_USER_MSG );
//		usType = GTC_NOTICE_LOBBY_OUT_USER_MSG;
//	}
//};
//
//// [[로비의 유저 리스트 요청]]
//// [- CTG_LOBBY_USER_LIST_RQ]		
//struct CTG_PACKET_LOBBY_USER_LIST_RQ : public PACKET_BASE
//{
//	short sListStartNum;
//
//	CTG_PACKET_LOBBY_USER_LIST_RQ()
//	{
//		usLength = sizeof( CTG_PACKET_LOBBY_USER_LIST_RQ );
//		usType = CTG_LOBBY_USER_LIST_RQ;
//	}
//};
//
//// [- GTC_LOBBY_USER_LIST_AQ]		
//struct VBUFFER_LOBBY_USER_LIST_AQ 
//{ 
//	char  cIsLast;	// 1 이면 더 이상 보낼 리스트가 없다는 뜻이다.
//	short sCount;	// 아래의 것들이 sCount 수만큼 인코딩 된다.
//	ARRAY_DATA_START
//		UINT64	i64CharCd;				// 캐릭터 코드
//		short sSeqCdInLobby;		// 로비(채널)에서의 순서 번호
//		char cSex;					// 성별
//		short sLevel;				// 레벨
//		char acName[ MAX_NICKNAME ];	// 이름
//	ARRAY_DATA_END
//};
//
//// [[룸 리스트 요청]]
//// [- CTG_LOBBY_ROOM_LIST_RQ]		
//struct CTG_PACKET_LOBBY_ROOM_LIST_RQ : public PACKET_BASE
//{
//	short sListStartNum;
//
//	CTG_PACKET_LOBBY_ROOM_LIST_RQ()
//	{
//		usLength = sizeof( CTG_PACKET_LOBBY_ROOM_LIST_RQ );
//		usType = CTG_LOBBY_ROOM_LIST_RQ;
//	}
//};
//
//// [- GTC_LOBBY_ROOM_LIST_AQ]		
//struct VBUFFER_LOBBY_ROOM_LIST_AQ 
//{ 
//	char  cIsLast;	// 1 이면 더 이상 보낼 리스트가 없다는 뜻이다.
//	short sCount;
//	ARRAY_DATA_START
//		char cRoomStatus;	// 방 상태
//		short sRoomNum;		// 방 번호
//		char acRoomName[ MAX_ROOM_NAME_LEN ]; // 방 이름
//		short sUserCount;	// 유저 수
//		short sManCount;	// 남자 수
//		short sWomanCount;	// 여자 수
//		char cGameMode;		// 게임 모드
//		short sMaxUserCount;	// 최대 입장 가능 유저 수
//		char cIsSecret;		// 비밀방 여부(1 이면 비밀방)
//	ARRAY_DATA_END
//};
//
//
//// [[아이템 로딩]]
//// [- GTD_LOAD_ITEM_RQ]
//struct GTD_PACKET_LOAD_ITEM_RQ : public PACKET_BASE
//{
//	GTD_PACKET_LOAD_ITEM_RQ()
//	{
//		usLength = sizeof( GTD_PACKET_LOAD_ITEM_RQ );
//		usType = GTD_LOAD_ITEM_RQ;
//	}
//};
//
//// DB의 아이템 정보
//struct DBItemInfo
//{
//	INT32 		iItemCd;						// 아이템 코드
//	char	cKind;							// 구분
//	char	cIsEnableSell;					// 판매 가능 여부
//	char	cPutPart;						// 착용 부위
//	char	cPutSex;						// 착용 성별
//	short sMaxUseCount;					// 최대 사용 회수
//	INT32 iMaxUseTime;					// 최대 사용 시간
//	INT32 		iMoneyPrice;				// 게임 머니 구입 가격
//	INT32 		iCashPrice;					// 캐쉬 머니 구입 가격
//	short	sLevelLimit;					// 레벨 제한
//	short	sPopularityLimit;				// 호감도(인기도) 제한
//	short	sSkillCd;						// 스킬 코드
//};
//const INT32 DTG_LOAD_ITEM_AQ_SEND_COUNT = 100;
//// [ DTG_LOAD_ITEM_AQ]
//struct DTG_PACKET_LOAD_ITEM_AQ : public PACKET_BASE
//{
//	int iCount;
//	DBItemInfo tDBItemInfo[ DTG_LOAD_ITEM_AQ_SEND_COUNT ];
//
//	DTG_PACKET_LOAD_ITEM_AQ()
//	{
//		usLength = sizeof( DTG_PACKET_LOAD_ITEM_AQ );
//		usType = DTG_LOAD_ITEM_AQ;
//	}
//};
//
//// [[주관식 문제 로딩]]
//// [- GTD_LOAD_SUBJECTIVEQUIZ_RQ]
//struct GTD_PACKET_LOAD_SUBJECTIVEQUIZ_RQ : public PACKET_BASE
//{
//	GTD_PACKET_LOAD_SUBJECTIVEQUIZ_RQ()
//	{
//		usLength = sizeof( GTD_PACKET_LOAD_SUBJECTIVEQUIZ_RQ );
//		usType = GTD_LOAD_SUBJECTIVEQUIZ_RQ;
//	}
//};
//// [ DTG_LOAD_SUBJECTIVEQUIZ_AQ]
//struct VBUFFER_LOAD_SUBJECTIVEQUIZ_AQ 
//{
//	INT32 	iCount;				// 개수
//	ARRAY_DATA_START
//		INT32 		iQuizCd;								
//		char	cKind;									
//		char	acQuestion[ MAX_SUBJECTIVE_QUESTION_LEN ];
//	ARRAY_DATA_END
//};
//// [ DTG_LOAD_SUBJECTIVEQUIZ_ANSWER_AQ]
//const INT32 MAX_LOAD_SUBJECTIVEQUIZ_ANSWE_COUNT = 100;
//struct DTG_PACKET_LOAD_SUBJECTIVEQUIZ_ANSWER_AQ : public PACKET_BASE
//{
//	INT32 		iKind;
//	INT32 		iCount;
//
//	UINT64 	ai64Code[ MAX_LOAD_SUBJECTIVEQUIZ_ANSWE_COUNT ];
//	INT32 		aiQuizCd[ MAX_LOAD_SUBJECTIVEQUIZ_ANSWE_COUNT ];
//	char		acAnswerValue[ MAX_LOAD_SUBJECTIVEQUIZ_ANSWE_COUNT ][ MAX_SUBJECTIVE_QUESTION_ANSWER_LEN ];
//	INT32 		aiValue[ MAX_LOAD_SUBJECTIVEQUIZ_ANSWE_COUNT ];
//
//	DTG_PACKET_LOAD_SUBJECTIVEQUIZ_ANSWER_AQ()
//	{
//		usLength = sizeof( DTG_PACKET_LOAD_SUBJECTIVEQUIZ_ANSWER_AQ );
//		usType = DTG_LOAD_SUBJECTIVEQUIZ_ANSWER_AQ;
//	}
//}; 
//
//
//// [[주관식 퀴즈의 번호만 로딩]]
//// [- MTD_LOAD_SUBJECTIVEQUIZ_RQ]
//struct MTD_PACKET_LOAD_SUBJECTIVEQUIZ_RQ : public PACKET_BASE
//{
//	MTD_PACKET_LOAD_SUBJECTIVEQUIZ_RQ()
//	{
//		usLength = sizeof( MTD_PACKET_LOAD_SUBJECTIVEQUIZ_RQ );
//		usType = MTD_LOAD_SUBJECTIVEQUIZ_RQ;
//	}
//};
//// [ DTM_LOAD_SUBJECTIVEQUIZ_AQ]
//struct VBUFFER_DTM_LOAD_SUBJECTIVEQUIZ_AQ 
//{
//	INT32 	iCount;				// 개수
//	
//	ARRAY_DATA_START
//	INT32 		iQuizCd;
//	char		cKind;
//	ARRAY_DATA_END
//};
//
//
//// [[주관식 문제의 답변의 갱신을 요청한다]]
//// [- MTD_UPDATE_SUBJECTIVEQUIZ_ANSWER_RQ]
//struct MTD_PACKET_UPDATE_SUBJECTIVEQUIZ_ANSWER_RQ : public PACKET_BASE
//{
//	INT32 		QuizCd;
//	char		Kind;
//
//	MTD_PACKET_UPDATE_SUBJECTIVEQUIZ_ANSWER_RQ()
//	{
//		usLength = sizeof( MTD_PACKET_UPDATE_SUBJECTIVEQUIZ_ANSWER_RQ );
//		usType = MTD_UPDATE_SUBJECTIVEQUIZ_ANSWER_RQ;
//	}
//};
//
//
//// [[스킬 로딩]]
//// [- GTD_LOAD_SKILL_RQ]
//struct GTD_PACKET_LOAD_SKILL_RQ : public PACKET_BASE
//{
//	GTD_PACKET_LOAD_SKILL_RQ()
//	{
//		usLength = sizeof( GTD_PACKET_LOAD_SKILL_RQ );
//		usType = GTD_LOAD_SKILL_RQ;
//	}
//};
//// [- DTG_LOAD_SKILL_AQ]
//const INT32 MAX_LOAD_SKILL_COUNT = 200;
//struct DTG_PACKET_LOAD_SKILL_AQ : public PACKET_BASE
//{
//	INT32	iCount;
//
//	short	asSkillCd[ MAX_LOAD_SKILL_COUNT ];
//	char	cArrKind[ MAX_LOAD_SKILL_COUNT ];
//	char	cArrUseSex[ MAX_LOAD_SKILL_COUNT ];
//	INT32	aiUsePoint[ MAX_LOAD_SKILL_COUNT ];
//	char	cArrIsOneTarget[ MAX_LOAD_SKILL_COUNT ];
//	char	cArrDirection[ MAX_LOAD_SKILL_COUNT ];
//	short	asDistance[ MAX_LOAD_SKILL_COUNT ];
//
//	DTG_PACKET_LOAD_SKILL_AQ()
//	{
//		usLength = sizeof( DTG_PACKET_LOAD_SKILL_AQ );
//		usType = DTG_LOAD_SKILL_AQ;
//	}
//};
//
//
//// [[캐릭터의 게임 데이터 저장]]
//// [- GTD_SAVE_CHARACTER_DATA_RQ]
//struct GTD_PACKET_SAVE_CHARACTER_DATA_RQ : public PACKET_BASE
//{
//	UINT64		i64CharCd;
//	short		sLv;
//	INT32 		iExp;
//	INT32 		iPopularity;
//	INT64		i64GameMoney;
//	UINT32		TotalConnectedTime; // 총 접속 시간(초)
//
//	GTD_PACKET_SAVE_CHARACTER_DATA_RQ()
//	{
//		usLength = sizeof( GTD_PACKET_SAVE_CHARACTER_DATA_RQ );
//		usType = GTD_SAVE_CHARACTER_DATA_RQ;
//	}
//};
//
//
//// [[캐릭터의 스킬 슬롯 정보 저장]]
//// [- GTD_SAVE_CHAR_SKILL_SLOT_RQ]
//const char SAVE_CHAR_SKILL_SLOT_DB_OPERATE_NONE = 0; // 무시
//const char SAVE_CHAR_SKILL_SLOT_DB_OPERATE_INSERT = 1; // 추가
//const char SAVE_CHAR_SKILL_SLOT_DB_OPERATE_UPDATE = 2; // 변경	
//const char SAVE_CHAR_SKILL_SLOT_DB_OPERATE_DELETE = 3; // 삭제
//struct GTD_PACKET_SAVE_CHAR_SKILL_SLOT_RQ : public PACKET_BASE
//{
//	UINT64		i64CharCd;
//
//	int			aiCharSkillSlotCd[ MAX_CHAR_SKILL_SLOT_COUNT ];
//	short		asSkillCd[ MAX_CHAR_SKILL_SLOT_COUNT ];
//	char		cArrDBOperateKind[ MAX_CHAR_SKILL_SLOT_COUNT ]; // DB 조작 종류
//
//	GTD_PACKET_SAVE_CHAR_SKILL_SLOT_RQ()
//	{
//		usLength = sizeof( GTD_PACKET_SAVE_CHAR_SKILL_SLOT_RQ );
//		usType = GTD_SAVE_CHAR_SKILL_SLOT_RQ;
//	}
//};
//
//
//// [[자신의 아이템 로딩 요청]]
//// [- CTG_OWN_CHARITEM_RQ]
//struct CTG_PACKET_OWN_CHARITEM_RQ : public PACKET_BASE
//{
//	CTG_PACKET_OWN_CHARITEM_RQ()
//	{
//		usLength = sizeof( CTG_PACKET_OWN_CHARITEM_RQ );
//		usType = CTG_OWN_CHARITEM_RQ;
//	}
//};
//// [- GTD_OWN_CHARITEM_RQ]	
//struct GTD_PACKET_OWN_CHARITEM_RQ : public PACKET_BASE
//{
//	UINT64	i64CharCd;			// 캐릭터 코드
//	INT32 		iGameConnectKey;
//
//	GTD_PACKET_OWN_CHARITEM_RQ()
//	{
//		usLength = sizeof( GTD_PACKET_OWN_CHARITEM_RQ );
//		usType = GTD_OWN_CHARITEM_RQ;
//	}
//};
//// [- DTGC_OWN_CHAR_ITEM_AQ]	
//struct VBUFFER_OWN_CHARITEM_AVATAR_AQ 
//{
//	INT32 		iGameConnectKey;
//	UINT64	i64CharCd;			// 캐릭터 코드
//	char	cKind;				// 아이템 종류
//	short	sCount;				// 개수
//	ARRAY_DATA_START
//		UINT64		i64CharItemCd;	// 캐릭터아이템코드
//		INT32 		iItemCd;		// 아이템코드		
//		INT32 		iGetTime;		// 입수시간(초)
//		INT32 		iUseStartTime;	// 사용 시작 시간(초)
//		INT32		iUsedTime;		// 사용한 시간.(사용하지 않을 때 계산)		
//	ARRAY_DATA_END
//};
//// [- DTGC_OWN_CHAR_ITEM_AQ]
//struct VBUFFER_OWN_CHARITEM_GAME_AQ 
//{
//	INT32 		iGameConnectKey;
//	UINT64	i64CharCd;			// 캐릭터 코드
//	char	cKind;				// 아이템 종류
//	short	sCount;				// 개수
//	ARRAY_DATA_START
//		UINT64		i64CharItemCd;	// 캐릭터 아이템 코드
//		INT32 		iItemCd;		// 아이템 코드		
//		INT32 		iGetTime;		// 입수 시간(초)
//		INT32 		iUseStartTime;	// 사용 시작 시간(초)
//		short		sCurUseCount;	// 현재 사용 횟수
//		INT32		iUsedTime;		// 사용한 시간.(사용하지 않을 때 계산)
//	ARRAY_DATA_END
//};
//// [- DTGC_OWN_CHAR_ITEM_AQ]
//struct VBUFFER_OWN_CHARITEM_STAGE_AQ 
//{
//	INT32 		iGameConnectKey;
//	UINT64	i64CharCd;			// 캐릭터 코드
//	char	cKind;				// 아이템 종류
//	short	sCount;				// 개수
//	ARRAY_DATA_START
//		UINT64		i64CharItemCd;	// 캐릭터 아이템 코드
//		INT32 		iItemCd;		// 아이템 코드		
//		INT32 		iGetTime;		// 입수 시간(초)
//		INT32 		iUseStartTime;	// 사용 시작 시간(초)
//		char		cIsCurUse;		// 현재 사용 여부
//		INT32		iUsedTime;		// 사용한 시간.(사용하지 않을 때 계산)
//	ARRAY_DATA_END
//};
//// [- DTGC_OWN_CHAR_ITEM_AQ]
//struct VBUFFER_OWN_CHARITEM_ARRANGE_AQ					
//{
//	INT32 		iGameConnectKey;
//	UINT64	i64CharCd;			// 캐릭터 코드
//	char	cKind;				// 아이템 종류
//	short	sCount;				// 개수
//	ARRAY_DATA_START
//		UINT64		i64CharItemCd;	// 캐릭터아이템코드
//		INT32 		iItemCd;		// 아이템코드		
//		INT32 		iGetTime;		// 입수시간(초)
//		INT32 		iUseStartTime;	// 사용 시작 시간(초)
//		char		cIsCurUse;		// 현재사용여부	
//		float		fPosX;			// 위치X
//		float		fPosY;			// 위치Y
//		float		fPosZ;			// 위치Z
//		INT32		iUsedTime;		// 사용한 시간.(사용하지 않을 때 계산)
//	ARRAY_DATA_END
//};
//
//// [[캐릭터 생성 요청]]
//// [- CTG_CREATE_CHARACTER_RQ]			
//struct CTG_PACKET_CREATE_CHARACTER_RQ : public PACKET_BASE
//{
//	char	acName[ MAX_NICKNAME ];	 
//	char	cBloodType;				// 혈액형	
//	
//	CTG_PACKET_CREATE_CHARACTER_RQ()
//	{
//		usLength = sizeof( CTG_PACKET_CREATE_CHARACTER_RQ );
//		usType = CTG_CREATE_CHARACTER_RQ;
//	}
//};
//// [- GTD_CREATE_CHARACTER_RQ]				
//struct GTD_PACKET_CREATE_CHARACTER_RQ : public PACKET_BASE
//{
//	UINT64	i64UKey;				// 계정 고유 키
//	DWORD   dwGameConnectKey;
//
//	char	acID[ MAX_USERID ];
//	char	cSex;
//	char	acName[ MAX_NICKNAME ];	 
//	char	cBloodType;				// 혈액형	
//	
//	GTD_PACKET_CREATE_CHARACTER_RQ()
//	{
//		usLength = sizeof( GTD_PACKET_CREATE_CHARACTER_RQ );
//		usType = GTD_CREATE_CHARACTER_RQ;
//	}
//};
//// [- DTG_CREATE_CHARACTER_AQ]				
//struct DTG_PACKET_CREATE_CHARACTER_AQ : public PACKET_BASE
//{
//	char cResult;
//
//	UINT64	i64UKey;				// 계정 고유 키
//	DWORD   dwGameConnectKey;
//
//	UINT64	i64CharCd;				// 캐릭터 코드
//	char	acName[ MAX_NICKNAME ];	 
//	char	cSex;					// 성별. 남자(1), 여자(그 이외)
//	short	sLv;					// 레벨
//	INT32 		iExp;					// 경험치
//	INT32 		iPopularity;			// 인기
//	INT64		i64Cash;					// 캐쉬
//	INT64		i64GameMoney;				// 게임머니
//	char	cBloodType;			// 혈액형
//	
//	DTG_PACKET_CREATE_CHARACTER_AQ()
//	{
//		usLength = sizeof( DTG_PACKET_CREATE_CHARACTER_AQ );
//		usType = DTG_CREATE_CHARACTER_AQ;
//	}
//};
//
//const char GTC_CREATE_CHARACTER_AQ_RESULT_DUPLICATION = 99;
//const char GTC_CREATE_CHARACTER_AQ_ERR_ALREADY_USER = 75;
//// [- GTC_CREATE_CHARACTER_AQ]
//struct GTC_PACKET_CREATE_CHARACTER_AQ : public PACKET_BASE
//{
//	char cResult;
//
//	UINT64	i64CharCd;
//	char	acName[ MAX_NICKNAME ];	 
//	char	cSex;					// 성별. 남자(1), 여자(그 이외)
//	short	sLv;					// 레벨
//	INT32 		iExp;					// 경험치
//	INT32 		iPopularity;			// 인기
//	INT64 	i64Cash;				// 캐쉬
//	INT64	    i64GameMoney;			// 게임머니
//	char	cBloodType;				// 혈액형
//	short	sGoodFeelCardCount;		// 호감도 추천표 개수.
//	
//	GTC_PACKET_CREATE_CHARACTER_AQ()
//	{
//		usLength = sizeof( GTC_PACKET_CREATE_CHARACTER_AQ );
//		usType = GTC_CREATE_CHARACTER_AQ;
//	}
//};
//
//
//// [[방 생성 요청]]
//// [- CTG_LOBBY_CREATE_ROOM_RQ]
//struct CTG_PACKET_LOBBY_CREATE_ROOM_RQ : public PACKET_BASE
//{
//	char	acRoomName[ MAX_ROOM_NAME_LEN ];		// 방 이름
//	char	cIsusePassWord;						// 패스워드 사용 여부.
//	char	acPassWord[ MAX_ROOM_PASSWORD_LEN ];	// 방 암호
//	char	cMode;								// 방 모드	
//	char	cSelIndexMemberCount;				// 인원 수에 관한 인덱스 번호.
//	char	cGameRoundcount;					// 게임 라운드 수
//	char	cBackGroundNum;						// 배경 번호
//	
//	CTG_PACKET_LOBBY_CREATE_ROOM_RQ()
//	{
//		usLength = sizeof( CTG_PACKET_LOBBY_CREATE_ROOM_RQ );
//		usType = CTG_LOBBY_CREATE_ROOM_RQ;
//	}
//};
//// [- GTC_LOBBY_CREATE_ROOM_AQ]
//struct GTC_PACKET_LOBBY_CREATE_ROOM_AQ : public PACKET_BASE
//{
//	char			cResult;		// 결과
//	unsigned short	usRoomNumber;	// 방 번호
//	float	fCurX;
//	float	fCurY;
//	float	fCurZ;
//	float	fCurAngle;
//	char	cBackGroundNum;
//
//	GTC_PACKET_LOBBY_CREATE_ROOM_AQ()
//	{
//		usLength = sizeof( GTC_PACKET_LOBBY_CREATE_ROOM_AQ );
//		usType = GTC_LOBBY_CREATE_ROOM_AQ;
//	}
//};
//// [- GTC_LOBBY_CREATE_ROOM_MSG]
//struct GTC_PACKET_LOBBY_CREATE_ROOM_MSG : public PACKET_BASE 
//{ 
//	char cRoomStatus;					// 방 상태
//	short sRoomNum;						// 방 번호
//	char acRoomName[ MAX_ROOM_NAME_LEN ]; // 방 이름
//	short sMaxUserCount;				// 최대 입장 가능 유저 수
//	short sUserCount;					// 유저 수
//	short sManCount;					// 남자 수
//	short sWomanCount;					// 여자 수
//	char cGameMode;						// 게임 모드
//	char cIsSecret;		// 비밀방 여부(1 이면 비밀방)
//		
//	GTC_PACKET_LOBBY_CREATE_ROOM_MSG()
//	{
//		usLength = sizeof( GTC_PACKET_LOBBY_CREATE_ROOM_MSG );
//		usType = GTC_LOBBY_CREATE_ROOM_MSG;
//	}
//};
//
//
//// [[방 입장 요청]]
//// [- CTG_LOBBY_JOIN_ROOM_RQ]
//const char LOBBY_JOIN_ROOM_FAILED_GAMEING = 68; // 게임 방 들어가기 실패. 게임 중
//struct CTG_PACKET_LOBBY_JOIN_ROOM_RQ : public PACKET_BASE
//{
//	short sRoomNumber;						// 들어갈 방의 번호
//	char  acPassword[ MAX_ROOM_PASSWORD_LEN ]; // 방의 비밀번호 
//	
//	CTG_PACKET_LOBBY_JOIN_ROOM_RQ()
//	{
//		usLength = sizeof( CTG_PACKET_LOBBY_JOIN_ROOM_RQ );
//		usType = CTG_LOBBY_JOIN_ROOM_RQ;
//	}
//};
//// [- GTC_LOBBY_JOIN_ROOM_AQ]
//struct VBUFFER_LOBBY_JOIN_ROOM_AQ 
//{
//	char	cResult;
//	char	cIsGameUser;				// '1' 이면 게임 참가자, 그 이외는 관전자
//	short	sRoomNumber;				// 방 번호
//	char	cBackGroundNum;				// 배경 번호
//	short	sMusicNumber;				// 음악 번호(0 이면 전체 음악 듣기가 아니다)
//	UINT64 i64MasterCharCd;			// 방장의 캐릭터 코드
//	short	sCount;						// 방에 있는 유저수
//	ARRAY_DATA_START 
//		UINT64	i64CharCd;					// 캐릭터 코드
//		char	acNickName[ 1 ];				// 닉네임
//		unsigned short ausWearItemCd[ WEAR_POS_END ]; // 착용 아이템 정보
//		char	cSex;						// 성별
//		short	sAge;						// 나이
//		char	cBloodType;				// 혈액형
//		
//		short	sDistrict2;			// 지역 정보 2
//		char	cPersonality;		// 성격	
//		char	cGoodPoint;			// 장점
//		char	cWeekPoint;			// 단점
//		char	cCharms;			// 애교
//		char	cLikeColor;			// 좋아하는 사람
//		char	cAlcoholic;			// 음주량
//		char	cSmoking;			// 흡연량
//		char	cWeekendWork;		// 주말에 하는 것
//		char	cAssuranceBodyPart;	// 자신있는 신체 부위
//		char	cFirstLove;			// 첫 사랑
//		char	cRespectPerson;		// 존경하는 사람
//		char	cMerriageAge;		// 결혼하고 싶은 나이
//		char	cWantedAge;			// 이상향 나이
//		char	cWantedSex;			// 이상향 성별 
//		char	cWantedBlood;		// 이상향 혈액형
//		char	cWantedInterest;	// 이상향 취미
//		short	sWantedDistrict2;	// 이상향 지역 2
//		char	cWantedPersonality;	// 이상향 성격
//		char	cWantedCharms;		// 이상향 애교
//		char	cWantedAlcoholic;	// 이상향 음주량
//		char	cWantedSmoking;		// 이상향 흡연량
//		char	cWantedWeekPoint;	// 이상향 단점 
//		char	cInterest;			// 취미
//
//		// 현재 위치, 각도
//		float	fCurX;
//		float	fCurY;
//		float	fCurZ;
//		float	fCurAngle;
//
//		char	cIsReady;			// 퀴즈 게임 준비 여부.
//	ARRAY_DATA_END
//};
//
//// [- GTC_ROOM_NEW_USER_MSG]
//struct GTC_PACKET_ROOM_NEW_USER_MSG : public PACKET_BASE
//{
//	UINT64	i64CharCd;					// 캐릭터 코드
//	char	acNickName[ MAX_NICKNAME ];	// 닉네임
//	char	cIsGameUser;				// '1' 이면 게임 참가자, 그 이외는 관전자
//	unsigned short ausWearItemCd[ WEAR_POS_END ]; // 착용 아이템 정보. * 참가자의 경우만 정보를 보냄
//	char	cSex;						// 성별
//	short	sAge;						// 나이
//	char	cBloodType;					// 혈액형
//
//	short	sDistrict2;			// 지역 정보 2
//	char	cPersonality;		// 성격	
//	char	cGoodPoint;			// 장점
//	char	cWeekPoint;			// 단점
//	char	cCharms;			// 애교
//	char	cLikeColor;			// 좋아하는 사람
//	char	cAlcoholic;			// 음주량
//	char	cSmoking;			// 흡연량
//	char	cWeekendWork;		// 주말에 하는 것
//	char	cAssuranceBodyPart;	// 자신있는 신체 부위
//	char	cFirstLove;			// 첫 사랑
//	char	cRespectPerson;		// 존경하는 사람
//	char	cMerriageAge;		// 결혼하고 싶은 나이
//	char	cWantedAge;			// 이상향 나이
//	char	cWantedSex;			// 이상향 성별 
//	char	cWantedBlood;		// 이상향 혈액형
//	char	cWantedInterest;	// 이상향 취미
//	short	sWantedDistrict2;	// 이상향 지역 2
//	char	cWantedPersonality;	// 이상향 성격
//	char	cWantedCharms;		// 이상향 애교
//	char	cWantedAlcoholic;	// 이상향 음주량
//	char	cWantedSmoking;		// 이상향 흡연량
//	char	cWantedWeekPoint;	// 이상향 단점 
//	char	cInterest;			// 취미
//
//	// 현재 위치 및 각도
//	float	fCurX;
//	float	fCurY;
//	float	fCurZ;
//	float	fCurAngle;
//	
//	GTC_PACKET_ROOM_NEW_USER_MSG()
//	{
//		usLength = sizeof( GTC_PACKET_ROOM_NEW_USER_MSG );
//		usType = GTC_ROOM_NEW_USER_MSG;
//	}
//};
//
//
////[[방 나가기 요청]]
//// [- CTG_ROOM_LEAVE_RQ]					
//struct CTG_PACKET_ROOM_LEAVE_RQ : public PACKET_BASE
//{
//	CTG_PACKET_ROOM_LEAVE_RQ()
//	{
//		usLength = sizeof( CTG_PACKET_ROOM_LEAVE_RQ );
//		usType = CTG_ROOM_LEAVE_RQ;
//	}
//};
//// [- GTC_ROOM_LEAVE_AQ]	
//struct GTC_PACKET_ROOM_LEAVE_AQ : public PACKET_BASE
//{
//	char cResult;
//
//	GTC_PACKET_ROOM_LEAVE_AQ()
//	{
//		usLength = sizeof( GTC_PACKET_ROOM_LEAVE_AQ );
//		usType = GTC_ROOM_LEAVE_AQ;
//	}
//};
//// [- GTC_ROOM_LEAVE_USER_MSG]				
//struct GTC_PACKET_ROOM_LEAVE_USER_MSG : public PACKET_BASE
//{
//	UINT64	i64CharCd;					// 나가는 캐릭터의 코드
//	UINT64	i64MasterCharCd;		// 방장의 캐릭터 코드
//
//	// 방장의 위치 좌표
//	float fX;
//	float fY;
//	float fZ;
//	float fAngle;
//
//	GTC_PACKET_ROOM_LEAVE_USER_MSG()
//	{
//		usLength = sizeof( GTC_PACKET_ROOM_LEAVE_USER_MSG );
//		usType = GTC_ROOM_LEAVE_USER_MSG;
//	}
//};
//// [- GTC_DESTORY_ROOM_MSG]		
//struct GTC_PACKET_DESTORY_ROOM_MSG : public PACKET_BASE
//{
//	short sRoomNumber; // 없어지는 방 번호
//
//	GTC_PACKET_DESTORY_ROOM_MSG()
//	{
//		usLength = sizeof( GTC_PACKET_DESTORY_ROOM_MSG );
//		usType = GTC_DESTORY_ROOM_MSG;
//	}
//};
//
//
////[[로비에서의 채팅]]
//// [- CTG_LOBBY_CHAT_RQ]
//struct CTG_PACKET_LOBBY_CHAT_RQ  : public PACKET_BASE
//{
//	short	sTextLen;	// 채팅 문자 길이
//	char	acText[ MAX_LOBBY_CHAT_LEN ];	// 채팅 문자
//
//	CTG_PACKET_LOBBY_CHAT_RQ()
//	{
//		usLength = sizeof( CTG_PACKET_LOBBY_CHAT_RQ );
//		usType = CTG_LOBBY_CHAT_RQ;
//	}
//};
//// [- GTC_LOBBY_CHAT_MSG]
//struct GTC_PACKET_LOBBY_CHAT_MSG  : public PACKET_BASE
//{
//	UINT64	i64CharCd;
//	short	sTextLen;	// 채팅 문자 길이
//	char	acText[ MAX_LOBBY_CHAT_LEN ];	// 채팅 문자
//
//	GTC_PACKET_LOBBY_CHAT_MSG()
//	{
//		usLength = sizeof( GTC_PACKET_LOBBY_CHAT_MSG );
//		usType = GTC_LOBBY_CHAT_MSG;
//	}
//};
//
//// [[방에서의 채팅]]
//// [- CTG_ROOM_CHAT_RQ]	
//struct CTG_PACKET_ROOM_CHAT_RQ  : public PACKET_BASE
//{
//	short	sTextLen;	// 채팅 문자 길이
//	char	acText[ MAX_ROOM_CHAT_LEN ];	// 채팅 문자
//
//	CTG_PACKET_ROOM_CHAT_RQ()
//	{
//		usLength = sizeof( CTG_PACKET_ROOM_CHAT_RQ );
//		usType = CTG_ROOM_CHAT_RQ;
//	}
//};
//// [- GTC_ROOM_CHAT_MSG]
//struct GTC_PACKET_ROOM_CHAT_MSG  : public PACKET_BASE
//{
//	UINT64	i64CharCd;
//	short	sTextLen;					// 채팅 문자 길이
//	char	acText[ MAX_ROOM_CHAT_LEN ];	// 채팅 문자
//
//	GTC_PACKET_ROOM_CHAT_MSG()
//	{
//		usLength = sizeof( GTC_PACKET_ROOM_CHAT_MSG );
//		usType = GTC_ROOM_CHAT_MSG;
//	}
//};
//
//
//// [[룸에서의 이동]]
//// [- CTG_ROOM_MOVE_RQ]
//struct CTG_PACKET_ROOM_MOVE_RQ  : public PACKET_BASE
//{
//	// 현재 위치, 각도
//	float	fCurPosX;	
//	float	fCurPosY;
//	float	fCurPosZ;
//	float	fCurAngle;
//
//	// 목적 위치, 각도
//	float	fNextPosX;
//	float	fNextPosY;
//	float	fNextPosZ;
//	float	fNextAngle;
//
//	char	cDir;		// 방향
//
//	CTG_PACKET_ROOM_MOVE_RQ()
//	{
//		usLength = sizeof( CTG_PACKET_ROOM_MOVE_RQ );
//		usType = CTG_ROOM_MOVE_RQ;
//	}
//};
//// [- GTC_ROOM_MOVE_MSG]
//struct GTC_PACKET_ROOM_MOVE_MSG  : public PACKET_BASE
//{
//	UINT64	i64CharCd;	// 움직일 캐릭터의 코드
//
//	// 현재 위치, 각도
//	float	fCurPosX;	
//	float	fCurPosY;
//	float	fCurPosZ;
//	float	fCurAngle;
//
//	// 목적 위치, 각도
//	float	fNextPosX;
//	float	fNextPosY;
//	float	fNextPosZ;
//	float	fNextAngle;
//
//	char	cDir;		// 방향
//
//	GTC_PACKET_ROOM_MOVE_MSG()
//	{
//		usLength = sizeof( GTC_PACKET_ROOM_MOVE_MSG );
//		usType = GTC_ROOM_MOVE_MSG;
//	}
//};
//
//
//// [[유저 정보 보기]]
//// [- CTG_USER_INFO_RQ]
//struct CTG_PACKET_USER_INFO_RQ  : public PACKET_BASE
//{
//	UINT64	i64CharCd;		// 캐릭터 코드
//	short	sSeqCdInLobby;	// 채널에서의 순번
//
//	CTG_PACKET_USER_INFO_RQ()
//	{
//		usLength = sizeof( CTG_PACKET_USER_INFO_RQ );
//		usType = CTG_USER_INFO_RQ;
//	}
//};
//// [- GTC_USER_INFO_AQ]
//struct GTC_PACKET_USER_INFO_AQ  : public PACKET_BASE
//{
//	char	cResult;					// 결과
//	char	cSex;						// 설명
//	unsigned short ausWearItemCd[ WEAR_POS_END ]; // 착용 아이템 정보
//	short	sLevel;						// 레벨	
//	INT32 	iPopularity;				// 인기도
//	short	sAge;				// 나이
//	char	cBlood;				// 혈액형
//	char	cInterest;			// 취미 
//	short	sDistrict2;			// 지역 정보 2
//	char	cPersonality;		// 성격	
//	char	cGoodPoint;			// 장점
//	char	cWeekPoint;			// 단점
//	char	cCharms;			// 애교
//	char	cLikeColor;			// 좋아하는 색
//	char	cAlcoholic;			// 음주량
//	char	cSmoking;			// 흡연량
//	char	cWeekendWork;		// 주말에 하는 것
//	char	cAssuranceBodyPart;	// 자신있는 신체 부위
//	char	cFirstLove;			// 첫 사랑
//	char	cRespectPerson;		// 존경하는 사람
//	char	cMerriageAge;		// 결혼하고 싶은 나이
//	char	cWantedAge;			// 이상향 나이
//	char	cWantedSex;			// 이상향 성별 
//	char	cWantedBlood;		// 이상향 혈액형
//	char	cWantedInterest;	// 이상향 취미
//	short	sWantedDistrict2;	// 이상향 지역 2
//	char	cWantedPersonality;	// 이상향 성격
//	char	cWantedCharms;		// 이상향 애교
//	char	cWantedAlcoholic;	// 이상향 음주량
//	char	cWantedSmoking;		// 이상향 흡연량
//	char	cWantedWeekPoint;	// 이상향 단점
//	
//	GTC_PACKET_USER_INFO_AQ()
//	{
//		usLength = sizeof( GTC_PACKET_USER_INFO_AQ );
//		usType = GTC_USER_INFO_AQ;
//	}
//};
//
//
//// [[로비 - 쪽지 보내기]]
//// [- CTG_SEND_NOTE_RQ]
//struct CTG_PACKET_SEND_NOTE_RQ  : public PACKET_BASE
//{
//	UINT64	i64CharCd;		// 캐릭터 코드
//	short	sSeqCdInLobby;	// 채널에서의 순번
//
//	short	sTextLen;		// 문자 길이
//	char	acText[ MAX_LOBBY_NOTE_LEN ];	// 문자
//
//	CTG_PACKET_SEND_NOTE_RQ()
//	{
//		usLength = sizeof( CTG_PACKET_SEND_NOTE_RQ );
//		usType = CTG_SEND_NOTE_RQ;
//	}
//};
//// [- GTC_SEND_NOTE_ERR_AQ]
//struct GTC_PACKET_SEND_NOTE_ERR_AQ  : public PACKET_BASE
//{
//	char	cResult;
//	UINT64	i64CharCd;		// 캐릭터 코드
//	
//	GTC_PACKET_SEND_NOTE_ERR_AQ()
//	{
//		usLength = sizeof( GTC_PACKET_SEND_NOTE_ERR_AQ );
//		usType = GTC_SEND_NOTE_ERR_AQ;
//	}
//};
//// [- GTC_SEND_NOTE_MSG]
//struct GTC_PACKET_SEND_NOTE_MSG  : public PACKET_BASE
//{
//	UINT64	i64CharCd;					// 쪽지를 보낸 캐릭터 코드
//	short	sTextLen;					// 문자 길이
//	char	acText[ MAX_LOBBY_NOTE_LEN ];	// 문자
//	
//	GTC_PACKET_SEND_NOTE_MSG()
//	{
//		usLength = sizeof( GTC_PACKET_SEND_NOTE_MSG );
//		usType = GTC_SEND_NOTE_MSG;
//	}
//};
//
//
//// [[로비 - 1:1 채팅 시작 요청]]
//// [- CTG_CHAT_1ON1_REQUEST_RQ]
//struct CTG_PACKET_CHAT_1ON1_REQUEST_RQ  : public PACKET_BASE
//{
//	// 상대방
//	UINT64	i64CharCd;		// 캐릭터 코드
//	short	sSeqCdInLobby;	// 채널에서의 순번
//
//	CTG_PACKET_CHAT_1ON1_REQUEST_RQ()
//	{
//		usLength = sizeof( CTG_PACKET_CHAT_1ON1_REQUEST_RQ );
//		usType = CTG_CHAT_1ON1_REQUEST_RQ;
//	}
//};
//// [- GTC_CHAT_1ON1_ACCEPT_RQ]
//struct GTC_PACKET_CHAT_1ON1_ACCEPT_RQ  : public PACKET_BASE
//{
//	// 상대방
//	UINT64	i64CharCd;		// 캐릭터 코드
//	
//	GTC_PACKET_CHAT_1ON1_ACCEPT_RQ()
//	{
//		usLength = sizeof( GTC_PACKET_CHAT_1ON1_ACCEPT_RQ );
//		usType = GTC_CHAT_1ON1_ACCEPT_RQ;
//	}
//};
//// [- CTG_CHAT_1ON1_ACCEPT_AQ]
//struct CTG_PACKET_CHAT_1ON1_ACCEPT_AQ  : public PACKET_BASE
//{
//	char	cIsOK;			// 1 이면 승낙
//
//	// 상대방
//	UINT64	i64CharCd;		// 캐릭터 코드
//	short	sSeqCdInLobby;	// 채널에서의 순번
//
//	CTG_PACKET_CHAT_1ON1_ACCEPT_AQ()
//	{
//		usLength = sizeof( CTG_PACKET_CHAT_1ON1_ACCEPT_AQ );
//		usType = CTG_CHAT_1ON1_ACCEPT_AQ;
//	}
//};
//// [- GTC_CHAT_1ON1_REQUEST_AQ]
//struct GTC_PACKET_CHAT_1ON1_REQUEST_AQ  : public PACKET_BASE
//{
//	char	cResult;
//	char	cIsOK;
//	// 상대방
//	UINT64	i64CharCd;		// 캐릭터 코드
//	
//	GTC_PACKET_CHAT_1ON1_REQUEST_AQ()
//	{
//		usLength = sizeof( GTC_PACKET_CHAT_1ON1_REQUEST_AQ );
//		usType = GTC_CHAT_1ON1_REQUEST_AQ;
//	}
//};
//
//// [[로비 - 1:1 채팅]]
//// [- CTG_CHAT_1ON1_RQ]
//struct CTG_PACKET_CHAT_1ON1_RQ  : public PACKET_BASE
//{
//	UINT64	i64CharCd;		// 받을 캐릭터 코드
//	short	sSeqCdInLobby;	// 채널에서의 순번
//
//	short	sTextLen;					// 문자 길이
//	char	acText[ MAX_LOBBY_NOTE_LEN ];	// 문자
//
//	CTG_PACKET_CHAT_1ON1_RQ()
//	{
//		usLength = sizeof( CTG_PACKET_CHAT_1ON1_RQ );
//		usType = CTG_CHAT_1ON1_RQ;
//	}
//};
//// [- GTC_CHAT_1ON1_MSG]
//struct GTC_PACKET_CHAT_1ON1_MSG  : public PACKET_BASE
//{
//	UINT64	i64CharCd;					// 쪽지를 보낸 캐릭터 코드
//	short	sTextLen;					// 문자 길이
//	char	acText[ MAX_LOBBY_NOTE_LEN ];	// 문자
//
//	GTC_PACKET_CHAT_1ON1_MSG()
//	{
//		usLength = sizeof( GTC_PACKET_CHAT_1ON1_MSG );
//		usType = GTC_CHAT_1ON1_MSG;
//	}
//};
//
//// [[로비 - 1:1 채팅 창 닫음]]
//// [- CTG_CHAT_1ON1_CLOSE_RQ]
//struct CTG_PACKET_CHAT_1ON1_CLOSE_RQ  : public PACKET_BASE
//{
//	UINT64 i64CloseCharCd; // 닫을 상대방의 캐릭터 코드
//	short	sSeqCdInLobby;  // 위 캐릭터의 로비의 시퀸스
//
//	CTG_PACKET_CHAT_1ON1_CLOSE_RQ()
//	{
//		usLength = sizeof( CTG_PACKET_CHAT_1ON1_CLOSE_RQ );
//		usType = CTG_CHAT_1ON1_CLOSE_RQ;
//	}
//};
//// [- GTC_CHAT_1ON1_CLOSE_MSG]
//struct GTC_PACKET_CHAT_1ON1_CLOSE_MSG  : public PACKET_BASE
//{
//	UINT64 i64CloseCharCdFrom;	// 닫은 캐릭터의 코드
//	UINT64 i64CloseCharCdTo;	// 닫힌 캐릭터의 코드
//
//	GTC_PACKET_CHAT_1ON1_CLOSE_MSG()
//	{
//		usLength = sizeof( GTC_PACKET_CHAT_1ON1_CLOSE_MSG );
//		usType = GTC_CHAT_1ON1_CLOSE_MSG;
//	}
//};
//
//
//// [[캐릭터 상세 정보 로딩]]
//// [- GTD_OWN_CHAR_DETAIL_RQ]
//struct GTD_PACKET_OWN_CHAR_DETAIL_RQ  : public PACKET_BASE
//{
//	UINT64	i64CharCd;
//	int		iGameConnectKey;
//	
//	GTD_PACKET_OWN_CHAR_DETAIL_RQ()
//	{
//		usLength = sizeof( GTD_PACKET_OWN_CHAR_DETAIL_RQ );
//		usType = GTD_OWN_CHAR_DETAIL_RQ;
//	}
//};
//// [- DTG_OWN_CHAR_DETAIL_AQ]
//const char OWN_CHAR_DETAIL_NONE = 92; // 상세 정보가 db에 없음
//struct DTG_PACKET_OWN_CHAR_DETAIL_AQ  : public PACKET_BASE
//{
//	UINT64	i64CharCd;
//	int		iGameConnectKey;
//
//	char	cIsCreated;			// 1 이면 상세 정보가 만들어져 있다.
//	short	sDistrict2;			// 지역 정보 2
//	char	cPersonality;		// 성격	
//	char	cGoodPoint;			// 장점
//	char	cWeekPoint;			// 단점
//	char	cCharms;			// 애교
//	char	cLikeColor;			// 좋아하는 사람
//	char	cAlcoholic;			// 음주량
//	char	cSmoking;			// 흡연량
//	char	cWeekendWork;		// 주말에 하는 것
//	char	cAssuranceBodyPart;	// 자신있는 신체 부위
//	char	cFirstLove;			// 첫 사랑
//	char	cRespectPerson;		// 존경하는 사람
//	char	cMerriageAge;		// 결혼하고 싶은 나이
//	char	cWantedAge;			// 이상향 나이
//	char	cWantedSex;			// 이상향 성별 
//	char	cWantedBlood;		// 이상향 혈액형
//	char	cWantedInterest;	// 이상향 취미
//	short	sWantedDistrict2;	// 이상향 지역 2
//	char	cWantedPersonality;	// 이상향 성격
//	char	cWantedCharms;		// 이상향 애교
//	char	cWantedAlcoholic;	// 이상향 음주량
//	char	cWantedSmoking;		// 이상향 흡연량
//	char	cWantedWeekPoint;	// 이상향 단점 
//	char	cInterest;			// 취미
//
//	DTG_PACKET_OWN_CHAR_DETAIL_AQ()
//	{
//		usLength = sizeof( DTG_PACKET_OWN_CHAR_DETAIL_AQ );
//		usType = DTG_OWN_CHAR_DETAIL_AQ;
//	}
//};
//// [- GTC_OWN_CHAR_DETAIL_MSG]
//struct GTC_PACKET_OWN_CHAR_DETAIL_MSG : public PACKET_BASE
//{
//	char	cIsCreated;
//	short	sDistrict2;			// 지역 정보 2
//	char	cPersonality;		// 성격	
//	char	cGoodPoint;			// 장점
//	char	cWeekPoint;			// 단점
//	char	cCharms;			// 애교
//	char	cLikeColor;			// 좋아하는 사람
//	char	cAlcoholic;			// 음주량
//	char	cSmoking;			// 흡연량
//	char	cWeekendWork;		// 주말에 하는 것
//	char	cAssuranceBodyPart;	// 자신있는 신체 부위
//	char	cFirstLove;			// 첫 사랑
//	char	cRespectPerson;		// 존경하는 사람
//	char	cMerriageAge;		// 결혼하고 싶은 나이
//	char	cWantedAge;			// 이상향 나이
//	char	cWantedSex;			// 이상향 성별 
//	char	cWantedBlood;		// 이상향 혈액형
//	char	cWantedInterest;	// 이상향 취미
//	short	sWantedDistrict2;	// 이상향 지역 2
//	char	cWantedPersonality;	// 이상향 성격
//	char	cWantedCharms;		// 이상향 애교
//	char	cWantedAlcoholic;	// 이상향 음주량
//	char	cWantedSmoking;		// 이상향 흡연량
//	char	cWantedWeekPoint;	// 이상향 단점 
//	char	cInterest;			// 취미
//
//	GTC_PACKET_OWN_CHAR_DETAIL_MSG()
//	{
//		usLength = sizeof( GTC_PACKET_OWN_CHAR_DETAIL_MSG );
//		usType = GTC_OWN_CHAR_DETAIL_MSG;
//	}
//};
//
//
//// [[캐릭터 상세 정보 생성]]
//// [- CTG_CREATE_CHAR_DETAIL_RQ]
//struct CTG_PACKET_CREATE_CHAR_DETAIL_RQ : public PACKET_BASE
//{
//	short	sDistrict2;			// 지역 정보 2
//	char	cPersonality;		// 성격	
//	char	cGoodPoint;			// 장점
//	char	cWeekPoint;			// 단점
//	char	cCharms;			// 애교
//	char	cLikeColor;			// 좋아하는 사람
//	char	cAlcoholic;			// 음주량
//	char	cSmoking;			// 흡연량
//	char	cWeekendWork;		// 주말에 하는 것
//	char	cAssuranceBodyPart;	// 자신있는 신체 부위
//	char	cFirstLove;			// 첫 사랑
//	char	cRespectPerson;		// 존경하는 사람
//	char	cMerriageAge;		// 결혼하고 싶은 나이
//	char	cWantedAge;			// 이상향 나이
//	char	cWantedSex;			// 이상향 성별 
//	char	cWantedBlood;		// 이상향 혈액형
//	char	cWantedInterest;	// 이상향 취미
//	short	sWantedDistrict2;	// 이상향 지역 2
//	char	cWantedPersonality;	// 이상향 성격
//	char	cWantedCharms;		// 이상향 애교
//	char	cWantedAlcoholic;	// 이상향 음주량
//	char	cWantedSmoking;		// 이상향 흡연량
//	char	cWantedWeekPoint;	// 이상향 단점 
//	char	cInterest;			// 취미
//
//	CTG_PACKET_CREATE_CHAR_DETAIL_RQ()
//	{
//		usLength = sizeof( CTG_PACKET_CREATE_CHAR_DETAIL_RQ );
//		usType = CTG_CREATE_CHAR_DETAIL_RQ;
//	}
//};
//// [- GTD_CREATE_CHAR_DETAIL_RQ]
//struct GTD_PACKET_CREATE_CHAR_DETAIL_RQ : public PACKET_BASE
//{
//	UINT64	i64CharCd;
//	int		iGameConnectKey;
//
//	short	sDistrict2;			// 지역 정보 2
//	char	cPersonality;		// 성격	
//	char	cGoodPoint;			// 장점
//	char	cWeekPoint;			// 단점
//	char	cCharms;			// 애교
//	char	cLikeColor;			// 좋아하는 사람
//	char	cAlcoholic;			// 음주량
//	char	cSmoking;			// 흡연량
//	char	cWeekendWork;		// 주말에 하는 것
//	char	cAssuranceBodyPart;	// 자신있는 신체 부위
//	char	cFirstLove;			// 첫 사랑
//	char	cRespectPerson;		// 존경하는 사람
//	char	cMerriageAge;		// 결혼하고 싶은 나이
//	char	cWantedAge;			// 이상향 나이
//	char	cWantedSex;			// 이상향 성별 
//	char	cWantedBlood;		// 이상향 혈액형
//	char	cWantedInterest;	// 이상향 취미
//	short	sWantedDistrict2;	// 이상향 지역 2
//	char	cWantedPersonality;	// 이상향 성격
//	char	cWantedCharms;		// 이상향 애교
//	char	cWantedAlcoholic;	// 이상향 음주량
//	char	cWantedSmoking;		// 이상향 흡연량
//	char	cWantedWeekPoint;	// 이상향 단점 
//	char	cInterest;			// 취미
//
//	GTD_PACKET_CREATE_CHAR_DETAIL_RQ()
//	{
//		usLength = sizeof( GTD_PACKET_CREATE_CHAR_DETAIL_RQ );
//		usType = GTD_CREATE_CHAR_DETAIL_RQ;
//	}
//};
//// [- DTG_CREATE_CHAR_DETAIL_AQ]
//struct DTG_PACKET_CREATE_CHAR_DETAIL_AQ : public PACKET_BASE
//{
//	UINT64	i64CharCd;
//	int		iGameConnectKey;
//
//	char	cResult;
//	short	sDistrict2;			// 지역 정보 2
//	char	cPersonality;		// 성격	
//	char	cGoodPoint;			// 장점
//	char	cWeekPoint;			// 단점
//	char	cCharms;			// 애교
//	char	cLikeColor;			// 좋아하는 사람
//	char	cAlcoholic;			// 음주량
//	char	cSmoking;			// 흡연량
//	char	cWeekendWork;		// 주말에 하는 것
//	char	cAssuranceBodyPart;	// 자신있는 신체 부위
//	char	cFirstLove;			// 첫 사랑
//	char	cRespectPerson;		// 존경하는 사람
//	char	cMerriageAge;		// 결혼하고 싶은 나이
//	char	cWantedAge;			// 이상향 나이
//	char	cWantedSex;			// 이상향 성별 
//	char	cWantedBlood;		// 이상향 혈액형
//	char	cWantedInterest;	// 이상향 취미
//	short	sWantedDistrict2;	// 이상향 지역 2
//	char	cWantedPersonality;	// 이상향 성격
//	char	cWantedCharms;		// 이상향 애교
//	char	cWantedAlcoholic;	// 이상향 음주량
//	char	cWantedSmoking;		// 이상향 흡연량
//	char	cWantedWeekPoint;	// 이상향 단점 
//	char	cInterest;			// 취미
//
//	DTG_PACKET_CREATE_CHAR_DETAIL_AQ()
//	{
//		usLength = sizeof(DTG_PACKET_CREATE_CHAR_DETAIL_AQ );
//		usType = DTG_CREATE_CHAR_DETAIL_AQ;
//	}
//};
//// [- GTC_CREATE_CHAR_DETAIL_AQ]
//const char CREATE_CHAR_DETAIL_ALREADY = 99; // 이미 만들어져 있다.
//struct GTC_PACKET_CREATE_CHAR_DETAIL_AQ : public PACKET_BASE
//{
//	char	cResult;
//	short	sDistrict2;			// 지역 정보 2
//	char	cPersonality;		// 성격	
//	char	cGoodPoint;			// 장점
//	char	cWeekPoint;			// 단점
//	char	cCharms;			// 애교
//	char	cLikeColor;			// 좋아하는 사람
//	char	cAlcoholic;			// 음주량
//	char	cSmoking;			// 흡연량
//	char	cWeekendWork;		// 주말에 하는 것
//	char	cAssuranceBodyPart;	// 자신있는 신체 부위
//	char	cFirstLove;			// 첫 사랑
//	char	cRespectPerson;		// 존경하는 사람
//	char	cMerriageAge;		// 결혼하고 싶은 나이
//	char	cWantedAge;			// 이상향 나이
//	char	cWantedSex;			// 이상향 성별 
//	char	cWantedBlood;		// 이상향 혈액형
//	char	cWantedInterest;	// 이상향 취미
//	short	sWantedDistrict2;	// 이상향 지역 2
//	char	cWantedPersonality;	// 이상향 성격
//	char	cWantedCharms;		// 이상향 애교
//	char	cWantedAlcoholic;	// 이상향 음주량
//	char	cWantedSmoking;		// 이상향 흡연량
//	char	cWantedWeekPoint;	// 이상향 단점 
//	char	cInterest;			// 취미
//
//	GTC_PACKET_CREATE_CHAR_DETAIL_AQ()
//	{
//		usLength = sizeof( GTC_PACKET_CREATE_CHAR_DETAIL_AQ );
//		usType = GTC_CREATE_CHAR_DETAIL_AQ;
//	}
//};
//
//
//// [[캐릭터 상세 정보 변경]]
//// [- CTG_CHANGE_CHAR_DETAIL_RQ]
//struct CTG_PACKET_CHANGE_CHAR_DETAIL_RQ : public PACKET_BASE
//{
//	short	sDistrict2;			// 지역 정보 
//	char	cPersonality;		// 성격	
//	char	cGoodPoint;			// 장점
//	char	cWeekPoint;			// 단점
//	char	cCharms;			// 애교
//	char	cLikeColor;			// 좋아하는 사람
//	char	cAlcoholic;			// 음주량
//	char	cSmoking;			// 흡연량
//	char	cWeekendWork;		// 주말에 하는 것
//	char	cAssuranceBodyPart;	// 자신있는 신체 부위
//	char	cFirstLove;			// 첫 사랑
//	char	cRespectPerson;		// 존경하는 사람
//	char	cMerriageAge;		// 결혼하고 싶은 나이
//	char	cWantedAge;			// 이상향 나이
//	char	cWantedSex;			// 이상향 성별 
//	char	cWantedBlood;		// 이상향 혈액형
//	char	cWantedInterest;	// 이상향 취미
//	short	sWantedDistrict2;	// 이상향 지역 2
//	char	cWantedPersonality;	// 이상향 성격
//	char	cWantedCharms;		// 이상향 애교
//	char	cWantedAlcoholic;	// 이상향 음주량
//	char	cWantedSmoking;		// 이상향 흡연량
//	char	cWantedWeekPoint;	// 이상향 단점 
//	char	cInterest;			// 취미
//
//	CTG_PACKET_CHANGE_CHAR_DETAIL_RQ()
//	{
//		usLength = sizeof( CTG_PACKET_CHANGE_CHAR_DETAIL_RQ );
//		usType = CTG_CHANGE_CHAR_DETAIL_RQ;
//	}
//};
//// [- GTD_CHANGE_CHAR_DETAIL_RQ]
//struct GTD_PACKET_CHANGE_CHAR_DETAIL_RQ : public PACKET_BASE
//{
//	UINT64	i64CharCd;
//	int		iGameConnectKey;
//
//	short	sDistrict2;			// 지역 정보 
//	char	cPersonality;		// 성격	
//	char	cGoodPoint;			// 장점
//	char	cWeekPoint;			// 단점
//	char	cCharms;			// 애교
//	char	cLikeColor;			// 좋아하는 사람
//	char	cAlcoholic;			// 음주량
//	char	cSmoking;			// 흡연량
//	char	cWeekendWork;		// 주말에 하는 것
//	char	cAssuranceBodyPart;	// 자신있는 신체 부위
//	char	cFirstLove;			// 첫 사랑
//	char	cRespectPerson;		// 존경하는 사람
//	char	cMerriageAge;		// 결혼하고 싶은 나이
//	char	cWantedAge;			// 이상향 나이
//	char	cWantedSex;			// 이상향 성별 
//	char	cWantedBlood;		// 이상향 혈액형
//	char	cWantedInterest;	// 이상향 취미
//	short	sWantedDistrict2;	// 이상향 지역 2
//	char	cWantedPersonality;	// 이상향 성격
//	char	cWantedCharms;		// 이상향 애교
//	char	cWantedAlcoholic;	// 이상향 음주량
//	char	cWantedSmoking;		// 이상향 흡연량
//	char	cWantedWeekPoint;	// 이상향 단점 
//	char	cInterest;			// 취미
//
//	GTD_PACKET_CHANGE_CHAR_DETAIL_RQ()
//	{
//		usLength = sizeof( GTD_PACKET_CHANGE_CHAR_DETAIL_RQ );
//		usType = GTD_CHANGE_CHAR_DETAIL_RQ;
//	}
//};
//// [DTG_CHANGE_CHAR_DETAIL_AQ]   
//const char CHANGE_CHAR_DETAIL_NONE = 91;
//// DTG_PACKET_OWN_CHAR_DETAIL_AQ
//// [- GTC_CHANGE_CHAR_DETAIL_AQ]
//struct GTC_PACKET_CHANGE_CHAR_DETAIL_AQ : public PACKET_BASE
//{
//	char	cResult;
//	
//	short	sDistrict2;			// 지역 정보 
//	char	cPersonality;		// 성격	
//	char	cGoodPoint;			// 장점
//	char	cWeekPoint;			// 단점
//	char	cCharms;			// 애교
//	char	cLikeColor;			// 좋아하는 사람
//	char	cAlcoholic;			// 음주량
//	char	cSmoking;			// 흡연량
//	char	cWeekendWork;		// 주말에 하는 것
//	char	cAssuranceBodyPart;	// 자신있는 신체 부위
//	char	cFirstLove;			// 첫 사랑
//	char	cRespectPerson;		// 존경하는 사람
//	char	cMerriageAge;		// 결혼하고 싶은 나이
//	char	cWantedAge;			// 이상향 나이
//	char	cWantedSex;			// 이상향 성별 
//	char	cWantedBlood;		// 이상향 혈액형
//	char	cWantedInterest;	// 이상향 취미
//	short	sWantedDistrict2;	// 이상향 지역 2
//	char	cWantedPersonality;	// 이상향 성격
//	char	cWantedCharms;		// 이상향 애교
//	char	cWantedAlcoholic;	// 이상향 음주량
//	char	cWantedSmoking;		// 이상향 흡연량
//	char	cWantedWeekPoint;	// 이상향 단점 
//	char	cInterest;			// 취미
//
//	GTC_PACKET_CHANGE_CHAR_DETAIL_AQ()
//	{
//		usLength = sizeof( GTC_PACKET_CHANGE_CHAR_DETAIL_AQ );
//		usType = GTC_CHANGE_CHAR_DETAIL_AQ;
//	}
//};
//
//
//// [[캐릭터 스킬 슬롯 정보 로딩]]
//// [- GTD_OWN_CHAR_SKILL_SLOT_RQ]
//struct GTD_PACKET_OWN_CHAR_SKILL_SLOT_RQ : public PACKET_BASE
//{
//	UINT64	i64CharCd;			
//	INT32 	iGameConnectKey;
//
//	GTD_PACKET_OWN_CHAR_SKILL_SLOT_RQ()
//	{
//		usLength = sizeof( GTD_PACKET_OWN_CHAR_SKILL_SLOT_RQ );
//		usType = GTD_OWN_CHAR_SKILL_SLOT_RQ;
//	}
//};
//// [- DTG_OWN_CHAR_SKILL_SLOT_AQ]
//struct DTG_PACKET_OWN_CHAR_SKILL_SLOT_AQ : public PACKET_BASE
//{
//	UINT64	i64CharCd;			
//	INT32 		iGameConnectKey;
//
//	short sCount;
//	INT32 aiCharSkillSlotCd[ MAX_CHAR_SKILL_SLOT_COUNT ];
//	short asSkillCd[ MAX_CHAR_SKILL_SLOT_COUNT ];
//	short asSlotNum[ MAX_CHAR_SKILL_SLOT_COUNT ];
//	
//	DTG_PACKET_OWN_CHAR_SKILL_SLOT_AQ()
//	{
//		usLength = sizeof( DTG_PACKET_OWN_CHAR_SKILL_SLOT_AQ );
//		usType = DTG_OWN_CHAR_SKILL_SLOT_AQ;
//	}
//};
//// [- GTC_OWN_CHAR_SKILL_SLOT_MSG]
//struct GTC_PACKET_OWN_CHAR_SKILL_SLOT_MSG : public PACKET_BASE
//{
//	short asSkillCd[ MAX_CHAR_SKILL_SLOT_COUNT ];
//	
//	GTC_PACKET_OWN_CHAR_SKILL_SLOT_MSG()
//	{
//		usLength = sizeof( GTC_PACKET_OWN_CHAR_SKILL_SLOT_MSG );
//		usType = GTC_OWN_CHAR_SKILL_SLOT_MSG;
//	}
//};
//
//
//// [[채널 변경 요청]]
//// [- CTG_CHANGE_CHANNEL_RQ]			
//struct CTG_PACKET_CHANGE_CHANNEL_RQ  : public PACKET_BASE
//{
//	char cIsCurGameServer;	// '1'의 지금 게임서버의 다른 채널로 변경 원함. 그 이외는 다른 게임서버
//	short sEnterChannelNum;	// 변경할 채널 번호
//
//	CTG_PACKET_CHANGE_CHANNEL_RQ()
//	{
//		usLength = sizeof( CTG_PACKET_CHANGE_CHANNEL_RQ );
//		usType = CTG_CHANGE_CHANNEL_RQ;
//	}
//};
//// [- GTC_CHANGE_CHANNEL_AQ]				
//struct GTC_PACKET_CHANGE_CHANNEL_AQ  : public PACKET_BASE
//{
//	char cResult;			
//	char cIsCurGameServer;	
//	short sEnterChannelNum;	
//
//	GTC_PACKET_CHANGE_CHANNEL_AQ()
//	{
//		usLength = sizeof( GTC_PACKET_CHANGE_CHANNEL_AQ );
//		usType = GTC_CHANGE_CHANNEL_AQ;
//	}
//};
// [- GTL_CHANGE_CHANNEL_MSG]			
struct GTL_PACKET_CHANGE_CHANNEL_MSG  : public PACKET_BASE
{
	UINT64	i64UKey;				// 계정 고유 키
	
	GTL_PACKET_CHANGE_CHANNEL_MSG()
	{
		usLength = sizeof( GTL_PACKET_CHANGE_CHANNEL_MSG );
		usType = GTL_CHANGE_CHANNEL_MSG;
	}
};


//// [[복장 아이템 부착 변경 요청]]
//// [- CTG_CHANGE_CHARITEM_WEAR_RQ]  
//struct CTG_PACKET_CHANGE_CHARITEM_WEAR_RQ  : public PACKET_BASE
//{
//	short asItemInvenArrayNum[ WEAR_POS_END ]; // 각 부위별 착용 아이템이 아바타 인벤에 있는 위치
//	                                         // 착용 하지 않는 것은 -1 로 한다.
//
//	CTG_PACKET_CHANGE_CHARITEM_WEAR_RQ()
//	{
//		usLength = sizeof( CTG_PACKET_CHANGE_CHARITEM_WEAR_RQ );
//		usType = CTG_CHANGE_CHARITEM_WEAR_RQ;
//	}
//};
//// [- GTD_CHANGE_CHARITEM_WEAR_RQ]
//const char CHANGE_CHARITEM_WEAR_NONE = 0; // 변경 없음
//const char CHANGE_CHARITEM_WEAR_PUT = 1; // 다른 것을 입는다.
//const char CHANGE_CHARITEM_WEAR_TAKEOFF = 2; // 벗는다.
//struct GTD_PACKET_CHANGE_CHARITEM_WEAR_RQ : public PACKET_BASE
//{
//	UINT64	i64CharCd;				
//	DWORD   dwGameConnectKey;		
//	
//	short asItemInvenArrayNum[ WEAR_POS_END ];
//	UINT64 ai64CharItemCdWear[ WEAR_POS_END ];	
//	char	cArrChangeKind[ WEAR_POS_END ];	
//	
//	short asItemInvenArrayNumOfTakeOff[ WEAR_POS_END ]; // 벗는 옷의 인벤토리에서의 인덱스
//	UINT64 ai64CharItemCdOfTakeOff[ WEAR_POS_END ]; // 벗는 옷의 캐릭터 아이템 코드
//	unsigned int auiUsedTimeOfTakeOff[ WEAR_POS_END ]; // 벗는 옷 캐릭터 아이템의 사용한 시간.
//
//	GTD_PACKET_CHANGE_CHARITEM_WEAR_RQ()
//	{
//		usLength = sizeof( GTD_PACKET_CHANGE_CHARITEM_WEAR_RQ );
//		usType = GTD_CHANGE_CHARITEM_WEAR_RQ;
//	}
//};
//// [- DTG_CHANGE_CHARITEM_WEAR_AQ]
//struct DTG_PACKET_CHANGE_CHARITEM_WEAR_AQ  : public PACKET_BASE
//{
//	char	cResult;
//	UINT64	i64CharCd;				
//	DWORD   dwGameConnectKey;		
//	
//	unsigned int uiCurSecondTime;
//	short asItemInvenArrayNum[ WEAR_POS_END ];
//	UINT64 ai64CharItemCdWear[ WEAR_POS_END ];	
//	char	cArrChangeKind[ WEAR_POS_END ];	
//	
//	short asItemInvenArrayNumOfTakeOff[ WEAR_POS_END ];
//	UINT64 ai64CharItemCdOfTakeOff[ WEAR_POS_END ]; 
//	unsigned int auiUsedTimeOfTakeOff[ WEAR_POS_END ];
//
//	DTG_PACKET_CHANGE_CHARITEM_WEAR_AQ()
//	{
//		usLength = sizeof( DTG_PACKET_CHANGE_CHARITEM_WEAR_AQ );
//		usType = DTG_CHANGE_CHARITEM_WEAR_AQ;
//	}
//};
//// [- GTC_CHANGE_CHARITEM_WEAR_AQ]
//const char CHANGE_CHARITEM_WEAR_ERR_TIME = 71;// 시간
//const char CHANGE_CHARITEM_WEAR_ERR_LEVEL = 72;// 레벨 제한
//const char CHANGE_CHARITEM_WEAR_ERR_POPULARITY = 73;// 인기도
//const char CHANGE_CHARITEM_WEAR_ERR_SEX = 74;// 성별
//struct GTC_PACKET_CHANGE_CHARITEM_WEAR_AQ  : public PACKET_BASE
//{
//	char	cResult;
//	
//	unsigned int uiCurSecondTime;
//	short asItemInvenArrayNum[ WEAR_POS_END ];
//	UINT64 ai64CharItemCdWear[ WEAR_POS_END ];	
//	
//	UINT64 ai64CharItemCdOfTakeOff[ WEAR_POS_END ]; 
//	unsigned int auiUsedTimeOfTakeOff[ WEAR_POS_END ];
//
//	GTC_PACKET_CHANGE_CHARITEM_WEAR_AQ()
//	{
//		usLength = sizeof( GTC_PACKET_CHANGE_CHARITEM_WEAR_AQ );
//		usType = GTC_CHANGE_CHARITEM_WEAR_AQ;
//	}
//};
//// [- GTC_CHANGE_CHARITEM_WEAR_MSG]
//struct GTC_PACKET_CHANGE_CHARITEM_WEAR_MSG  : public PACKET_BASE
//{
//	UINT64		i64CharCd;
//	unsigned short	ausWearItemCd[ WEAR_POS_END ]; // 각 부위별 착용된 아이템 코드		
//	
//	GTC_PACKET_CHANGE_CHARITEM_WEAR_MSG()
//	{
//		usLength = sizeof( GTC_PACKET_CHANGE_CHARITEM_WEAR_MSG );
//		usType = GTC_CHANGE_CHARITEM_WEAR_MSG;
//	}
//};
//
//
//// [[아이템 삭제]]
//// [- CTG_DELETE_CHARITEM_RQ]
//struct CTG_PACKET_DELETE_CHARITEM_RQ  : public PACKET_BASE
//{
//	char	cItemKind;			// 아이템의 종류
//	short	sItemInvenArrayNum; // 인벤에서의 위치
//                                
//	CTG_PACKET_DELETE_CHARITEM_RQ()
//	{
//		usLength = sizeof( CTG_PACKET_DELETE_CHARITEM_RQ );
//		usType = CTG_DELETE_CHARITEM_RQ;
//	}
//};
//// [- GTD_DELETE_CHARITEM_RQ]
//struct GTD_PACKET_DELETE_CHARITEM_RQ : public PACKET_BASE
//{
//	UINT64	i64CharCd;				
//	DWORD   dwGameConnectKey;		
//	
//	char	cItemKind;	// 아이템의 종류
//	short	sItemInvenArrayNum;
//	UINT64 i64CharItemCd;	
//	
//	GTD_PACKET_DELETE_CHARITEM_RQ()
//	{
//		usLength = sizeof( GTD_PACKET_DELETE_CHARITEM_RQ );
//		usType = GTD_DELETE_CHARITEM_RQ;
//	}
//};
//// [- DTG_DELETE_CHARITEM_AQ]
//struct DTG_PACKET_DELETE_CHARITEM_AQ : public PACKET_BASE
//{
//	char	cResult;
//	UINT64	i64CharCd;				
//	DWORD   dwGameConnectKey;		
//	
//	char	cItemKind;	// 아이템의 종류
//	short	sItemInvenArrayNum;
//	UINT64 i64CharItemCd;	
//	
//	DTG_PACKET_DELETE_CHARITEM_AQ()
//	{
//		usLength = sizeof( DTG_PACKET_DELETE_CHARITEM_AQ );
//		usType = DTG_DELETE_CHARITEM_AQ;
//	}
//};
//// [- GTC_DELETE_CHARITEM_AQ]
//struct GTC_PACKET_DELETE_CHARITEM_AQ  : public PACKET_BASE
//{
//	char	cResult;
//	char	cItemKind;	// 아이템의 종류
//	short	sItemInvenArrayNum; 
//	                                
//	GTC_PACKET_DELETE_CHARITEM_AQ()
//	{
//		usLength = sizeof( GTC_PACKET_DELETE_CHARITEM_AQ );
//		usType = GTC_DELETE_CHARITEM_AQ;
//	}
//};
//
//
//// [[아이템 구매 요청]]
//// [- CTG_BUY_ITEM_RQ]
//struct CTG_PACKET_BUY_ITEM_RQ  : public PACKET_BASE
//{
//	INT32 		iBuyItemCd;			// 구입을 바라는 아이템
//	short	sItemInvenArrayNum;	// 구입한 아이템을 넣을 인벤에서의 번호
//                                
//	CTG_PACKET_BUY_ITEM_RQ()
//	{
//		usLength = sizeof( CTG_PACKET_BUY_ITEM_RQ );
//		usType = CTG_BUY_ITEM_RQ;
//	}
//};
//// [- GTD_BUY_ITEM_RQ]
//struct GTD_PACKET_BUY_ITEM_RQ : public PACKET_BASE
//{
//	UINT64	i64CharCd;				
//	DWORD   dwGameConnectKey;		
//	
//	char	cItemKind;			// 아이템의 종류
//	INT32 		iBuyItemCd;	
//	INT32 		iBuyGameMoney;		// 구매금액
//	short	sItemInvenArrayNum;	
//	
//	GTD_PACKET_BUY_ITEM_RQ()
//	{
//		usLength = sizeof( GTD_PACKET_BUY_ITEM_RQ );
//		usType = GTD_BUY_ITEM_RQ;
//	}
//};
//// [- DTG_BUY_ITEM_AQ]
//struct DTG_PACKET_BUY_ITEM_AQ : public PACKET_BASE
//{
//	char	cResult;
//	UINT64	i64CharCd;				
//	DWORD   dwGameConnectKey;		
//	
//	char	cItemKind;
//	INT32 		iBuyItemCd;		
//	INT32 		iBuyGameMoney;
//	short	sItemInvenArrayNum;	
//	UINT64 i64BuyCharItemCd;		// 구입하는 아이템의 캐릭터 아이템코드
//	INT32 		iCurSecondTime;			// 현재 시간을 초 단위로
//	
//	DTG_PACKET_BUY_ITEM_AQ()
//	{
//		usLength = sizeof( DTG_PACKET_BUY_ITEM_AQ );
//		usType = DTG_BUY_ITEM_AQ;
//	}
//};
//// [- GTC_BUY_ITEM_AQ]
//struct GTC_PACKET_BUY_ITEM_AQ  : public PACKET_BASE
//{
//	char	cResult;
//	INT32 		iBuyItemCd;			// 구입을 바라는 아이템
//	short	sItemInvenArrayNum;	// 구입한 아이템을 넣을 인벤에서의 번호
//	UINT64 i64BuyCharItemCd;	// 구입하는 아이템의 캐릭터 아이템코드
//	INT64		i64CurGameMoney;	// 현재 가지고 있는 게임머니.
//                                
//	GTC_PACKET_BUY_ITEM_AQ()
//	{
//		usLength = sizeof( GTC_PACKET_BUY_ITEM_AQ );
//		usType = GTC_BUY_ITEM_AQ;
//	}
//};
//
//
//// [[캐릭터 스킬 슬롯 변경]]
//// [- CTG_CHANGE_CHAR_SKILL_SLOT_RQ]
//const char CHANGE_CHAR_SKILL_SLOT_CHANHE = 1; // 슬롯간의 위치 변경
//const char CHANGE_CHAR_SKILL_SLOT_ADD = 2; // 스킬 리스트에서 슬롯에 넣을 때	
//const char CHANGE_CHAR_SKILL_SLOT_DELETE = 3; // 슬롯에서 삭제 할 때
//struct CTG_PACKET_CHANGE_CHAR_SKILL_SLOT_RQ : public PACKET_BASE
//{
//	char	cChangeKind;	// 변경 종류
//
//	// 추가 및 삭제 일 때는 sSourceSlotNum만 유효하다. 
//	short	sSourceSlotNum;	// 원 슬롯 위치
//	short	sDestSlotNum;	// 목표 슬롯 위치	
//
//	short	sSkillCd;		// 스킬 코드. 추가 할 때만 유효하다.
//
//	CTG_PACKET_CHANGE_CHAR_SKILL_SLOT_RQ()
//	{
//		usLength = sizeof( CTG_PACKET_CHANGE_CHAR_SKILL_SLOT_RQ );
//		usType = CTG_CHANGE_CHAR_SKILL_SLOT_RQ;
//	}
//};
//// [- GTC_CHANGE_CHAR_SKILL_SLOT_AQ]
//struct GTC_PACKET_CHANGE_CHAR_SKILL_SLOT_AQ : public PACKET_BASE
//{
//	char	cResult;
//	char	cChangeKind;	// 변경 종류
//
//	short	sSourceSlotNum;
//	short	sDestSlotNum;
//
//	short	sSkillCd;
//
//	GTC_PACKET_CHANGE_CHAR_SKILL_SLOT_AQ()
//	{
//		usLength = sizeof( GTC_PACKET_CHANGE_CHAR_SKILL_SLOT_AQ );
//		usType = GTC_CHANGE_CHAR_SKILL_SLOT_AQ;
//	}
//};
//
//
//// [[게임 준비 완료]]
//// [- CTG_ROOM_GAME_GAME_READY_RQ]
//struct CTG_PACKET_ROOM_GAME_READY_RQ  : public PACKET_BASE
//{
//	char	cIsReady;	// '1'이면 Ready, 그 이외는 해제
//                                
//	CTG_PACKET_ROOM_GAME_READY_RQ()
//	{
//		usLength = sizeof( CTG_PACKET_ROOM_GAME_READY_RQ );
//		usType = CTG_ROOM_GAME_READY_RQ;
//	}
//};
//// [- GTC_ROOM_GAME_READY_FAILED]
//struct GTC_PACKET_ROOM_GAME_READY_FAILED  : public PACKET_BASE
//{
//	char cResult;	// 실패 이유
//                                
//	GTC_PACKET_ROOM_GAME_READY_FAILED()
//	{
//		usLength = sizeof( GTC_PACKET_ROOM_GAME_READY_FAILED );
//		usType = GTC_ROOM_GAME_READY_FAILED;
//	}
//};
//// [- GTC_ROOM_GAME_READY_MSG]
//struct GTC_PACKET_ROOM_GAME_READY_MSG  : public PACKET_BASE
//{
//	char	cIsReady;
//	UINT64 i64CharCd; // 요청자
//	char cReadySeq; // Ready 순서. 0 부터 시작한다.
//
//	// Ready 취소를 했을 때 캐릭터 좌표로 사용한다.
//	float fX;
//	float fY;
//	float fZ;
//	float fAngle;
//	                                
//	GTC_PACKET_ROOM_GAME_READY_MSG()
//	{
//		usLength = sizeof( GTC_PACKET_ROOM_GAME_READY_MSG );
//		usType = GTC_ROOM_GAME_READY_MSG;
//	}
//};
//
//
//// [[게임 시작 요청]]
//// [- CTG_ROOM_GAME_START_RQ]
//struct CTG_PACKET_ROOM_GAME_START_RQ  : public PACKET_BASE
//{
//	CTG_PACKET_ROOM_GAME_START_RQ()
//	{
//		usLength = sizeof( CTG_PACKET_ROOM_GAME_START_RQ );
//		usType = CTG_ROOM_GAME_START_RQ;
//	}
//};
//// [- GTC_ROOM_GAME_START_FAILED]
//struct GTC_PACKET_ROOM_GAME_START_FAILED  : public PACKET_BASE
//{
//	char cResult;	// 실패 이유
//                                
//	GTC_PACKET_ROOM_GAME_START_FAILED()
//	{
//		usLength = sizeof( GTC_PACKET_ROOM_GAME_START_FAILED );
//		usType = GTC_ROOM_GAME_START_FAILED;
//	}
//};
//// [- GTC_ROOM_GAME_START_MSG]
//struct GTC_PACKET_ROOM_GAME_START_MSG  : public PACKET_BASE
//{
//	UINT32	uiSequenceNum;		// 시퀸스 번호
//	char	cRoundNum;			// 라운드 번호
//	char	cIsTimeOverAndStart; // 1 이면앞 라운드가 타임 오버 된 후 시작
//	short	sBestAnswerNum;		// 정답 범위 : 최고
//	short	sLowestAnswerNum;	// 정답 범위 : 최저
//	UINT64	ui64QuizCd;			// DB에 있는 퀴즈 코드가 아닌 가동된 코드 이다.
//	short	sQuizQuestionTextLen; // 퀴즈의 글자 길이
//	char	acQuizQuestionText[ MAX_SUBJECTIVE_QUESTION_LEN ]; // 문제
//	
//	GTC_PACKET_ROOM_GAME_START_MSG()
//	{
//		usLength = sizeof( GTC_PACKET_ROOM_GAME_START_MSG );
//		usType = GTC_ROOM_GAME_START_MSG;
//	}
//};
//// [- GTC_ROOM_CHANGE_STATE_MSG]
//struct GTC_PACKET_ROOM_CHANGE_STATE_MSG  : public PACKET_BASE
//{
//	short	sRoomNum;		// 방 번호
//	char	cState;			// 방 상태
//	short	sManUserCount;
//	short	sWomanUserCount;
//
//	GTC_PACKET_ROOM_CHANGE_STATE_MSG()
//	{
//		usLength = sizeof( GTC_PACKET_ROOM_CHANGE_STATE_MSG );
//		usType = GTC_ROOM_CHANGE_STATE_MSG;
//	}
//};
//
//
//// [[게임 - 정답 시도]]
//// [- CTG_ROOM_GAME_TRY_ANSWER_RQ]
//struct CTG_PACKET_ROOM_GAME_TRY_ANSWER_RQ  : public PACKET_BASE
//{
//	char	acAnswer[ MAX_SUBJECTIVE_QUESTION_ANSWER_LEN ]; // 답변
//	
//	CTG_PACKET_ROOM_GAME_TRY_ANSWER_RQ()
//	{
//		usLength = sizeof( CTG_PACKET_ROOM_GAME_TRY_ANSWER_RQ );
//		usType = CTG_ROOM_GAME_TRY_ANSWER_RQ;
//	}
//};
//// [- GTC_ROOM_GAME_TRY_ANSWER_MSG]
//struct GTC_PACKET_ROOM_GAME_TRY_ANSWER_MSG  : public PACKET_BASE
//{
//	char	cResult;	// 결과. 실패인 경우는 정답을 시도한 사람에게 간다. 보통은 성공
//	UINT64 i64CharCd;  // 정답을 시도한 캐릭터
//	short	sCharScore;	// 정답을 시도한 캐릭터의 현재 점수(맞추었다면 맞춘 것도 포함).
//	
//	char	cIsCorret;	// 정답 여부. 1 이면 정답.
//	short	sAnswerNum; // 정답을 맞춘 경우 정답 순서. 틀리면 0 이 들어간다.
//	int		iAnswerValue; // 정답이 받은 투표 수. 정답일 때만 유효.
//	char	acAnswer[ MAX_SUBJECTIVE_QUESTION_ANSWER_LEN ]; // 
//
//	GTC_PACKET_ROOM_GAME_TRY_ANSWER_MSG()
//	{
//		usLength = sizeof( GTC_PACKET_ROOM_GAME_TRY_ANSWER_MSG );
//		usType = GTC_ROOM_GAME_TRY_ANSWER_MSG;
//	}
//};
//
//
//// [[유저의 라이프 타임이 초과 했음을 통보]]
//// [- GTC_ROOM_GAME_LIFE_TIME_OVER_MSG]
//struct CTG_PACKET_ROOM_GAME_LIFE_TIME_OVER_MSG  : public PACKET_BASE
//{
//	UINT64 i64CharCd;  // 라이프 시간을 초과한 캐릭터
//	char	LifeCount;	// 라이프 개수.
//	
//	CTG_PACKET_ROOM_GAME_LIFE_TIME_OVER_MSG()
//	{
//		usLength = sizeof( CTG_PACKET_ROOM_GAME_LIFE_TIME_OVER_MSG );
//		usType = GTC_ROOM_GAME_LIFE_TIME_OVER_MSG;
//	}
//};
//
//
//// [[게임에서 투표를 한다]]
//// [- CTG_ROOM_GAME_VOTE_RQ]
//struct CTG_PACKET_ROOM_GAME_VOTE_RQ  : public PACKET_BASE
//{
//	UINT64 i64ReceiveCharCd; // 투표를 할 캐릭터의 코드
//
//	CTG_PACKET_ROOM_GAME_VOTE_RQ()
//	{
//		usLength = sizeof( CTG_PACKET_ROOM_GAME_VOTE_RQ );
//		usType = CTG_ROOM_GAME_VOTE_RQ;
//	}
//};
//// [- GTD_ROOM_GAME_VOTE_RQ]
//struct GTD_PACKET_ROOM_GAME_VOTE_RQ  : public PACKET_BASE
//{
//	UINT64 i64VoteCharCd;
//	DWORD   dwVoteCharGameConnectKey;
//
//	DWORD   dwReceiveCharGameConnectKey;
//	int		iPrevExp;				// (추천 이전)호감도를 받은 캐릭터의 호감도 값,
//	int		iCurExp;				// (추천 이후)호감도를 받은 캐릭터의 호감도 값
//	char	acGiverNickName[ MAX_NICKNAME ]; // 추천을 한 캐릭터 이름
//
//
//	GTD_PACKET_ROOM_GAME_VOTE_RQ()
//	{
//		usLength = sizeof( GTD_PACKET_ROOM_GAME_VOTE_RQ );
//		usType = GTD_ROOM_GAME_VOTE_RQ;
//	}
//};
//// [- DTG_ROOM_GAME_VOTE_AQ]
//struct DTG_PACKET_ROOM_GAME_VOTE_AQ  : public PACKET_BASE
//{
//	char	cResult;			
//	
//	UINT64 i64VoteCharCd;
//	DWORD   dwVoteCharGameConnectKey;
//
//	DWORD   dwReceiveCharGameConnectKey;
//	int		iPrevExp;				
//	int		iCurExp;				
//	char	acGiverNickName[ MAX_NICKNAME ];
//
//	DTG_PACKET_ROOM_GAME_VOTE_AQ()
//	{
//		usLength = sizeof( DTG_PACKET_ROOM_GAME_VOTE_AQ );
//		usType = DTG_ROOM_GAME_VOTE_AQ;
//	}
//};
//// [- GTC_ROOM_GAME_VOTE_AQ]
//struct GTC_PACKET_ROOM_GAME_VOTE_AQ  : public PACKET_BASE
//{
//	char	cResult;			// 결과
//	char	cRemainVotecount;	// 투표를 할 수 있는 남은 표수
//	UINT64 i64ReceiveCharCd; 
//
//	GTC_PACKET_ROOM_GAME_VOTE_AQ()
//	{
//		usLength = sizeof( GTC_PACKET_ROOM_GAME_VOTE_AQ );
//		usType = GTC_ROOM_GAME_VOTE_AQ;
//	}
//};
//// [- GTC_ROOM_GAME_VOTE_MSG]
//struct GTC_PACKET_ROOM_GAME_VOTE_MSG  : public PACKET_BASE
//{
//	UINT64 VoteCharCd;			// 투표를 한 캐릭터
//	UINT64 ReceiveCharCd;		// 투표를 받은 캐릭터
//	short	sCurReceiveVoteCount;	// 현재까지 받은 투표수
//
//	// 게임 유저들의 파트너 정보 
//	INT64 CharCds[ MAX_ROOM_QUIZ_USER_COUNT ];
//	INT64 PartnerCharCds[ MAX_ROOM_QUIZ_USER_COUNT ]; // 파트너의 캐릭터 코드
//	char	cIsPartnerGuards[ MAX_ROOM_QUIZ_USER_COUNT ]; // 예비 커플의 보호막 여부( 1 이 아니면 보호막이 없다 )
//
//	GTC_PACKET_ROOM_GAME_VOTE_MSG()
//	{
//		usLength = sizeof( GTC_PACKET_ROOM_GAME_VOTE_MSG );
//		usType = GTC_ROOM_GAME_VOTE_MSG;
//	}
//};
//
//
//// [[호감도 추천]]
//// [- CTG_VOTE_GOODFEEL_RQ]
//struct CTG_PACKET_VOTE_GOODFEEL_RQ  : public PACKET_BASE
//{
//	UINT64	i64ReceiveCharCd;		// 호감도를 받을 캐릭터의 코드
//	short	sSeqCdInLobby;			// 위 캐릭터의 로비의 시퀸스
//	
//	CTG_PACKET_VOTE_GOODFEEL_RQ()
//	{
//		usLength = sizeof( CTG_PACKET_VOTE_GOODFEEL_RQ );
//		usType = CTG_VOTE_GOODFEEL_RQ;
//	}
//};
//// [- GTD_VOTE_GOODFEEL_RQ]
//struct GTD_PACKET_VOTE_GOODFEEL_RQ  : public PACKET_BASE
//{
//	UINT64 i64VoteCharCd;			// 호감도를 추천 하는 캐릭터의 코드
//	DWORD   dwVoteCharGameConnectKey;
//	short	sPrevGoodFeelCardCount;	// (추천 이전)호감도 추천을 할 수 있는 카드 수
//	short	sCurGoodFeelCardCount;	// (추천 이후)호감도 추천을 할 수 있는 카드 수
//	
//	UINT64	i64ReceiveCharCd;		// 호감도를 받은 캐릭터의 코드
//	DWORD   dwReceiveCharGameConnectKey;
//	int		iPrevPopularity;		// (추천 이전)호감도를 받은 캐릭터의 호감도 값,
//	int		iCurPopularity;			// (추천 이후)호감도를 받은 캐릭터의 호감도 값
//	
//	char	acGiverNickName[ MAX_NICKNAME ]; // 추천을 한 캐릭터 이름
//
//	char	cCurDay;						// 오늘 날짜
//
//	GTD_PACKET_VOTE_GOODFEEL_RQ()
//	{
//		usLength = sizeof( GTD_PACKET_VOTE_GOODFEEL_RQ );
//		usType = GTD_VOTE_GOODFEEL_RQ;
//	}
//};
//// [- DTG_VOTE_GOODFEEL_AQ]
//struct DTG_PACKET_VOTE_GOODFEEL_AQ  : public PACKET_BASE
//{
//	char	cResult;
//	UINT64 i64VoteCharCd;			
//	DWORD   dwVoteCharGameConnectKey;
//	short	sPrevGoodFeelCardCount;
//	short	sCurGoodFeelCardCount;	
//
//	UINT64	i64ReceiveCharCd;
//	DWORD   dwReceiveCharGameConnectKey;
//	int		iPrevPopularity;
//	int		iCurPopularity;
//
//	char	acGiverNickName[ MAX_NICKNAME ];
//
//	DTG_PACKET_VOTE_GOODFEEL_AQ()
//	{
//		usLength = sizeof( DTG_PACKET_VOTE_GOODFEEL_AQ );
//		usType = DTG_VOTE_GOODFEEL_AQ;
//	}
//};
//// [- GTC_VOTE_GOODFEEL_AQ]
//struct GTC_PACKET_VOTE_GOODFEEL_AQ  : public PACKET_BASE
//{
//	char	cResult;
//	short	sCurGoodFeelCardCount;	// (추천 이후)호감도 추천을 할 수 있는 카드 수
//
//	GTC_PACKET_VOTE_GOODFEEL_AQ()
//	{
//		usLength = sizeof( GTC_PACKET_VOTE_GOODFEEL_AQ );
//		usType = GTC_VOTE_GOODFEEL_AQ;
//	}
//};
//// [- GTC_VOTE_GOODFEEL_MSG]
//struct GTC_PACKET_VOTE_GOODFEEL_MSG  : public PACKET_BASE
//{
//	int		iCurPopularity;				// (추천 이후)호감도를 받은 캐릭터의 호감도 값
//	char	acGiverNickName[ MAX_NICKNAME ]; // 추천을 한 캐릭터 이름
//	UINT64 ReceiverCharCd;			// 호감도 추천을 받은 캐릭터 코드
//
//	GTC_PACKET_VOTE_GOODFEEL_MSG()
//	{
//		usLength = sizeof( GTC_PACKET_VOTE_GOODFEEL_MSG );
//		usType = GTC_VOTE_GOODFEEL_MSG;
//	}
//};
//
//
//struct UserGameResult 
//{
//	UINT64 i64CharCd;			// 캐릭터 코드
//	short	sScore;				// 점수
//	short	sScoreRank_Money;	// 점수 랭킹에 따라 받은 돈
//	short	sVoteRank_Money;	// 투표 랭킹에 따라 받은 돈
//	short	sScoreRank_Exp;		// 점수 랭킹에 따라 받은 경험치
//	short	sVoteRank_Exp;		// 투표 랭킹에 따라 받은 경험치
//	INT64 i64GameMoney;		// 현재 게임 머니
//	INT32 	iExp;				// 현재 경험치
//	short	sReceiveVote;		// 투표를 받은 회수
//	short	sGainPopularity;	// 투표 획득 수에 따라 계산된 인기도
//	char	cScoreRank;			// 점수에 의한 순위
//	char	sVoteRank;			// 투표에 의한 순위
//};
//// [ GTC_ROOM_GAME_END_RESULT_MSG]
//struct GTC_PACKET_ROOM_GAME_END_RESULT_MSG  : public PACKET_BASE
//{
//	short			sUserCount;
//	UserGameResult	atUserGameResult[ MAX_ROOM_QUIZ_USER_COUNT ];
//
//	GTC_PACKET_ROOM_GAME_END_RESULT_MSG()
//	{
//		usLength = sizeof( GTC_PACKET_ROOM_GAME_END_RESULT_MSG );
//		usType = GTC_ROOM_GAME_END_RESULT_MSG;
//	}
//};
//
//
//// [[게임 중단 통보]]
//// [- GTC_ROOM_GAME_INTERRUPTION_MSG]
//struct GTC_PACKET_ROOM_GAME_INTERRUPTION_MSG  : public PACKET_BASE
//{
//	// 게임 중단 후 남은 캐릭터의 위치
//	float fX;	float fY;	float fZ;		float fAngle;
//
//	GTC_PACKET_ROOM_GAME_INTERRUPTION_MSG()
//	{
//		usLength = sizeof( GTC_PACKET_ROOM_GAME_INTERRUPTION_MSG );
//		usType = GTC_ROOM_GAME_INTERRUPTION_MSG;
//	}
//};
//
//
//// [[ (게임에서) 커플 승낙 ]]
//// [- CTG_ROOM_GAME_COPULE_CONSENT_RQ]
//struct CTG_PACKET_ROOM_GAME_COPULE_CONSENT_RQ  : public PACKET_BASE
//{
//	char cIsOK;	// '1' 이면 커플 승낙
//
//	CTG_PACKET_ROOM_GAME_COPULE_CONSENT_RQ()
//	{
//		usLength = sizeof( CTG_PACKET_ROOM_GAME_COPULE_CONSENT_RQ );
//		usType = CTG_ROOM_GAME_COPULE_CONSENT_RQ;
//	}
//};
//
//// [- GTC_ROOM_GAME_COPULE_CONSENT_AQ]
//struct GTC_PACKET_ROOM_GAME_COPULE_CONSENT_AQ  : public PACKET_BASE
//{
//	char cResult;
//	char cIsOK;	
//
//	GTC_PACKET_ROOM_GAME_COPULE_CONSENT_AQ()
//	{
//		usLength = sizeof( GTC_PACKET_ROOM_GAME_COPULE_CONSENT_AQ );
//		usType = GTC_ROOM_GAME_COPULE_CONSENT_AQ;
//	}
//};
//
//// [- GTC_ROOM_GAME_COPULE_CONSENT_MSG]
//struct GTC_PACKET_ROOM_GAME_COPULE_CONSENT_MSG  : public PACKET_BASE
//{
//	char	cGameUserCount;
//	UINT64 CharCds[ MAX_ROOM_QUIZ_USER_COUNT ];
//	UINT64 PartnerCharCds[ MAX_ROOM_QUIZ_USER_COUNT ]; // 파트너의 캐릭터 코드
//
//	// 각 캐릭터들이 돌아갈 위치
//	float	fX[ MAX_ROOM_QUIZ_USER_COUNT ];
//	float	fY[ MAX_ROOM_QUIZ_USER_COUNT ];
//	float	fZ[ MAX_ROOM_QUIZ_USER_COUNT ];
//	float	fAngle[ MAX_ROOM_QUIZ_USER_COUNT ];
//
//	GTC_PACKET_ROOM_GAME_COPULE_CONSENT_MSG()
//	{
//		usLength = sizeof( GTC_PACKET_ROOM_GAME_COPULE_CONSENT_MSG );
//		usType = GTC_ROOM_GAME_COPULE_CONSENT_MSG;
//	}
//};
//
//
//// [[설문 조사 선택 문항]]
//// [ GTC_ROOM_GAME_QUESTION_RESEARCH_LIST_MSG]
//struct GTC_PACKET_ROOM_GAME_QUESTION_RESEARCH_LIST_MSG  : public PACKET_BASE
//{
//	char	acQuizQuestionText[ MAX_SUBJECTIVE_QUESTION_LEN ];
//	char	acAnswer[ QUESTION_RESEARCH_ANSWER_COUNT ][ MAX_SUBJECTIVE_QUESTION_ANSWER_LEN ]; // 설문 답변
//
//	GTC_PACKET_ROOM_GAME_QUESTION_RESEARCH_LIST_MSG()
//	{
//		usLength = sizeof( GTC_PACKET_ROOM_GAME_QUESTION_RESEARCH_LIST_MSG );
//		usType = GTC_ROOM_GAME_QUESTION_RESEARCH_LIST_MSG;
//	}
//};
//
//// [[설문 조사 선택 문항 선택]] 
//// [- CTG_ROOM_GAME_QUESTION_RESEARCH_SELECT_RQ]
//struct CTG_PACKET_ROOM_GAME_QUESTION_RESEARCH_SELECT_RQ  : public PACKET_BASE
//{
//	char cIsSelectIndex; // 선택한 항목의 순번
//
//	CTG_PACKET_ROOM_GAME_QUESTION_RESEARCH_SELECT_RQ()
//	{
//		usLength = sizeof( CTG_PACKET_ROOM_GAME_QUESTION_RESEARCH_SELECT_RQ );
//		usType = CTG_ROOM_GAME_QUESTION_RESEARCH_SELECT_RQ;
//	}
//};
//
//// [- GTD_ROOM_GAME_QUESTION_RESEARCH_SELECT_RQ]
//struct GTD_PACKET_ROOM_GAME_QUESTION_RESEARCH_SELECT_RQ  : public PACKET_BASE
//{
//	UINT64 i64CharCd;
//	DWORD   iGameConnectKey;
//
//	char cIsSelectIndex;
//	char cQuizKind;		 // 퀴즈 종류
//	UINT64 i64AnswerCd; // 퀴즈 답 코드
//
//	GTD_PACKET_ROOM_GAME_QUESTION_RESEARCH_SELECT_RQ()
//	{
//		usLength = sizeof( GTD_PACKET_ROOM_GAME_QUESTION_RESEARCH_SELECT_RQ );
//		usType = GTD_ROOM_GAME_QUESTION_RESEARCH_SELECT_RQ;
//	}
//};
//// [- DTG_ROOM_GAME_QUESTION_RESEARCH_SELECT_AQ]
//struct DTG_PACKET_ROOM_GAME_QUESTION_RESEARCH_SELECT_AQ  : public PACKET_BASE
//{
//	char cResult;
//	UINT64 i64CharCd;
//	DWORD   iGameConnectKey;
//
//	char cIsSelectIndex;
//	char cQuizKind;		 // 퀴즈 종류
//	UINT64 i64AnswerCd; // 퀴즈 답 코드
//
//	DTG_PACKET_ROOM_GAME_QUESTION_RESEARCH_SELECT_AQ()
//	{
//		usLength = sizeof( DTG_PACKET_ROOM_GAME_QUESTION_RESEARCH_SELECT_AQ );
//		usType = DTG_ROOM_GAME_QUESTION_RESEARCH_SELECT_AQ;
//	}
//};
//
//// [- GTC_ROOM_GAME_QUESTION_RESEARCH_SELECT_AQ]
//struct GTC_PACKET_ROOM_GAME_QUESTION_RESEARCH_SELECT_AQ  : public PACKET_BASE
//{
//	char cResult;
//	char cIsSelectIndex;
//
//	GTC_PACKET_ROOM_GAME_QUESTION_RESEARCH_SELECT_AQ()
//	{
//		usLength = sizeof( GTC_PACKET_ROOM_GAME_QUESTION_RESEARCH_SELECT_AQ );
//		usType = GTC_ROOM_GAME_QUESTION_RESEARCH_SELECT_AQ;
//	}
//};
//
//
//// [[스킬 사용]]
//// [- CTG_USE_SKILL_RQ]
//struct CTG_PACKET_USE_SKILL_RQ  : public PACKET_BASE
//{
//	short	sSkillCd;	// 스킬 코드. 만약 아이템으로 스킬을 사용한다면 '0'을 넣어야 한다.
//	INT32	iItemCd;	// 아이템 코드
//	
//	CTG_PACKET_USE_SKILL_RQ()
//	{
//		usLength = sizeof( CTG_PACKET_USE_SKILL_RQ );
//		usType = CTG_USE_SKILL_RQ;
//	}
//};
//// [- GTC_USE_SKILL_AQ]
//struct GTC_PACKET_USE_SKILL_AQ  : public PACKET_BASE
//{
//	char cResult;
//	short	sSkillCd;
//	INT32	iItemCd;
//
//	GTC_PACKET_USE_SKILL_AQ()
//	{
//		usLength = sizeof( GTC_PACKET_USE_SKILL_AQ );
//		usType = GTC_USE_SKILL_AQ;
//	}
//};
//// [- GTC_USE_SKILL_MSG]
//struct GTC_PACKET_USE_SKILL_MSG  : public PACKET_BASE
//{
//	UINT64 i64CharCd;	// 스킬을 사용한 사람
//	short	sSkillCd;
//	
//	GTC_PACKET_USE_SKILL_MSG()
//	{
//		usLength = sizeof( GTC_PACKET_USE_SKILL_MSG );
//		usType = GTC_USE_SKILL_MSG;
//	}
//};
//
//
//// [(방의)전체 음악 듣기]
//// [- CTG_ALL_LISTEN_MUSIC_RQ]
//struct CTG_PACKET_ALL_LISTEN_MUSIC_RQ  : public PACKET_BASE
//{
//	short	sMusicNumber;	// 들을 음악 번호
//
//	CTG_PACKET_ALL_LISTEN_MUSIC_RQ()
//	{
//		usLength = sizeof( CTG_PACKET_ALL_LISTEN_MUSIC_RQ );
//		usType = CTG_ALL_LISTEN_MUSIC_RQ;
//	}
//};
//
//// [- GTC_ALL_LISTEN_MUSIC_AQ]
//struct GTC_PACKET_ALL_LISTEN_MUSIC_AQ  : public PACKET_BASE
//{
//	char cResult;
//
//	GTC_PACKET_ALL_LISTEN_MUSIC_AQ()
//	{
//		usLength = sizeof( GTC_PACKET_ALL_LISTEN_MUSIC_AQ );
//		usType = GTC_ALL_LISTEN_MUSIC_AQ;
//	}
//};
//
//// [- GTC_ALL_LISTEN_MUSIC_MSG]  
//struct GTC_PACKET_ALL_LISTEN_MUSIC_MSG  : public PACKET_BASE
//{
//	char	cIsStart;	// 전체 듣기 시작 여부(1이면 방장이 전체 듣기를 시작)
//	short	sMusicNumber;	
//
//	GTC_PACKET_ALL_LISTEN_MUSIC_MSG()
//	{
//		usLength = sizeof( GTC_PACKET_ALL_LISTEN_MUSIC_MSG );
//		usType = GTC_ALL_LISTEN_MUSIC_MSG;
//	}
//};
//
//
//// [[(방의)전체 음악 듣기 - 승낙/취소]]
//// [- CTG_ALL_LISTEN_MUSIC_SYNONYMY_RQ]
//struct CTG_PACKET_ALL_LISTEN_MUSIC_SYNONYMY_RQ  : public PACKET_BASE
//{
//	char cIsSynonymy;	// 찬성(1), 취소(0)
//	CTG_PACKET_ALL_LISTEN_MUSIC_SYNONYMY_RQ()
//	{
//		usLength = sizeof( CTG_PACKET_ALL_LISTEN_MUSIC_SYNONYMY_RQ );
//		usType = CTG_ALL_LISTEN_MUSIC_SYNONYMY_RQ;
//	}
//};
//// [- GTC_ALL_LISTEN_MUSIC_SYNONYMY_AQ]
//struct GTC_PACKET_ALL_LISTEN_MUSIC_SYNONYMY_AQ  : public PACKET_BASE
//{
//	char cIsSynonymy;
//	short	sMusicNumber;	// 들을 음악 번호
//
//	GTC_PACKET_ALL_LISTEN_MUSIC_SYNONYMY_AQ()
//	{
//		usLength = sizeof( GTC_PACKET_ALL_LISTEN_MUSIC_SYNONYMY_AQ );
//		usType = GTC_ALL_LISTEN_MUSIC_SYNONYMY_AQ;
//	}
//};
//
//
//// [[(방의)전체 음악 듣기 - 종료]]
//// [- CTG_ALL_LISTEN_MUSIC_END_RQ]
//struct CTG_PACKET_ALL_LISTEN_MUSIC_END_RQ  : public PACKET_BASE
//{
//	CTG_PACKET_ALL_LISTEN_MUSIC_END_RQ()
//	{
//		usLength = sizeof( CTG_PACKET_ALL_LISTEN_MUSIC_END_RQ );
//		usType = CTG_ALL_LISTEN_MUSIC_END_RQ;
//	}
//};
//
//// [- GTC_ALL_LISTEN_MUSIC_END_AQ]
//struct GTC_PACKET_ALL_LISTEN_MUSIC_END_AQ  : public PACKET_BASE
//{
//	char cResult;
//
//	GTC_PACKET_ALL_LISTEN_MUSIC_END_AQ()
//	{
//		usLength = sizeof( GTC_PACKET_ALL_LISTEN_MUSIC_END_AQ );
//		usType = GTC_ALL_LISTEN_MUSIC_END_AQ;
//	}
//};
//
//// [- GTC_ALL_LISTEN_MUSIC_END_MSG]
//struct GTC_PACKET_ALL_LISTEN_MUSIC_END_MSG  : public PACKET_BASE
//{
//	GTC_PACKET_ALL_LISTEN_MUSIC_END_MSG()
//	{
//		usLength = sizeof( GTC_PACKET_ALL_LISTEN_MUSIC_END_MSG );
//		usType = GTC_ALL_LISTEN_MUSIC_END_MSG;
//	}
//};
//
//
//// [[캐릭터 아이템 충전(사용 연장)]]
//// [- CTG_CHARGE_CHARITEM_RQ]
//struct CTG_PACKET_CHARGE_CHARITEM_RQ  : public PACKET_BASE
//{
//	char	cItemKind;			// 아이템의 종류
//	short	sItemInvenArrayNum; // 인벤에서의 위치
//	int		ChargeItemCd;	// 재 구매를 원하는 아이템 코드
//
//	CTG_PACKET_CHARGE_CHARITEM_RQ()
//	{
//		usLength = sizeof( CTG_PACKET_CHARGE_CHARITEM_RQ );
//		usType = CTG_CHARGE_CHARITEM_RQ;
//	}
//};
//// [- GTD_CHARGE_CHARITEM_RQ]
//struct GTD_PACKET_CHARGE_CHARITEM_RQ  : public PACKET_BASE
//{
//	UINT64	i64CharCd;				
//	DWORD   dwGameConnectKey;		
//	
//	char	cItemKind;			// 아이템의 종류
//	short	sItemInvenArrayNum;
//	int		ChargeItemCd;
//	
//	UINT64 i64CharItemCd;
//	int		iChargeMoney;		// 충전 비용
//	
//	GTD_PACKET_CHARGE_CHARITEM_RQ()
//	{
//		usLength = sizeof( GTD_PACKET_CHARGE_CHARITEM_RQ );
//		usType = GTD_CHARGE_CHARITEM_RQ;
//	}
//};
//// [- DTG_CHARGE_CHARITEM_AQ]
//struct DTG_PACKET_CHARGE_CHARITEM_AQ  : public PACKET_BASE
//{
//	char	cResult;
//	UINT64	i64CharCd;				
//	DWORD   dwGameConnectKey;		
//
//	char	cItemKind;
//	short	sItemInvenArrayNum;
//	int		ChargeItemCd;
//
//	UINT64 i64CharItemCd;
//	int		iChargeMoney;
//
//	DTG_PACKET_CHARGE_CHARITEM_AQ()
//	{
//		usLength = sizeof( DTG_PACKET_CHARGE_CHARITEM_AQ );
//		usType = DTG_CHARGE_CHARITEM_AQ;
//	}
//};
//// [- GTC_CHARGE_CHARITEM_AQ]
//struct GTC_PACKET_CHARGE_CHARITEM_AQ  : public PACKET_BASE
//{
//	char	cResult;			// 결과
//	
//	char	cItemKind;			// 아이템 종류
//	short	sItemInvenArrayNum;	// 인벤토리 인덱스
//	int		ChargeItemCd;
//	UINT64 i64CharItemCd;		// 충전된 캐릭터아이템 코드
//	INT64 i64CurGameMoney;	// 최종적인 현재 가진 돈
//
//	GTC_PACKET_CHARGE_CHARITEM_AQ()
//	{
//		usLength = sizeof( GTC_PACKET_CHARGE_CHARITEM_AQ );
//		usType = GTC_CHARGE_CHARITEM_AQ;
//	}
//};
//
//// [[ 방에서 강제 퇴장 ]]
//// [- CTG_ROOM_BAN_CHAR_RQ]
//struct CTG_PACKET_ROOM_BAN_CHAR_RQ  : public PACKET_BASE
//{
//	UINT64		BanCharCd;	// 강퇴 시킬 캐릭터의 코드
//
//	CTG_PACKET_ROOM_BAN_CHAR_RQ()
//	{
//		usLength = sizeof( CTG_PACKET_ROOM_BAN_CHAR_RQ );
//		usType = CTG_ROOM_BAN_CHAR_RQ;
//	}
//};					
//
//// [- GTC_ROOM_BAN_CHAR_AQ]
//struct GTC_PACKET_ROOM_BAN_CHAR_AQ  : public PACKET_BASE
//{
//	char cResult; // 결과
//	UINT64		BanCharCd;
//
//	GTC_PACKET_ROOM_BAN_CHAR_AQ()
//	{
//		usLength = sizeof( GTC_PACKET_ROOM_BAN_CHAR_AQ );
//		usType = GTC_ROOM_BAN_CHAR_AQ;
//	}
//};
//
//// [- GTC_ROOM_BAN_CHAR_MSG]
//struct GTC_PACKET_ROOM_BAN_CHAR_MSG  : public PACKET_BASE
//{
//	UINT64		BanCharCd; // 강퇴 당하는 캐릭터의 코드
//
//	GTC_PACKET_ROOM_BAN_CHAR_MSG()
//	{
//		usLength = sizeof( GTC_PACKET_ROOM_BAN_CHAR_MSG );
//		usType = GTC_ROOM_BAN_CHAR_MSG;
//	}
//};
//
//
//
//
//
//////////////////////////////// 메신저 ///////////////////////////////////////
//// [- GTM_LOGIN_CLIENT_GAME_MSG]
//struct GTM_PACKET_LOGIN_CLIENT_GAME_MSG : public PACKET_BASE
//{
//	short	sGameServerIndex;		// 게임서버 인덱스
//	UINT64	i64CharCd;				// 캐릭터 코드
//	short	sChannelNum;			// 채널 번호
//	char	acName[ MAX_NICKNAME ];	// 캐릭터 이름
//
//	GTM_PACKET_LOGIN_CLIENT_GAME_MSG()
//	{
//		usLength = sizeof( GTM_PACKET_LOGIN_CLIENT_GAME_MSG );
//		usType = GTM_LOGIN_CLIENT_GAME_MSG;
//	}
//};
//
//// [- GTM_CHANGE_CHANNEL_MSG]			
//struct GTM_PACKET_CHANGE_CHANNEL_MSG  : public PACKET_BASE
//{
//	UINT64	i64CharCd;				// 캐릭터 코드
//	short	sChannelNum;			// 채널 번호
//	
//	GTM_PACKET_CHANGE_CHANNEL_MSG()
//	{
//		usLength = sizeof( GTM_PACKET_CHANGE_CHANNEL_MSG );
//		usType = GTM_CHANGE_CHANNEL_MSG;
//	}
//};
//
//// [- GTM_LOGOUT_CLIENT_MSG]
//struct GTM_PACKET_LOGOUT_CLIENT_MSG : public PACKET_BASE
//{
//	short sGameServerIndex;		// 게임서버 인덱스
//	short sChannelNum;			// 유저가 들어간 채널 번호
//	UINT64	i64CharCd;
//
//	GTM_PACKET_LOGOUT_CLIENT_MSG()
//	{
//		usLength = sizeof( GTM_PACKET_LOGOUT_CLIENT_MSG );
//		usType = GTM_LOGOUT_CLIENT_MSG;
//	}
//};
//
//
//// [[친구 리스트 요청]]
//// [- CTG_FRIEND_LIST_RQ]		
//struct CTG_PACKET_FRIEND_LIST_RQ  : public PACKET_BASE
//{
//	CTG_PACKET_FRIEND_LIST_RQ()
//	{
//		usLength = sizeof( CTG_PACKET_FRIEND_LIST_RQ );
//		usType = CTG_FRIEND_LIST_RQ;
//	}
//};
//// [- GTM_FRIEND_LIST_RQ]
//struct GTM_PACKET_FRIEND_LIST_RQ  : public PACKET_BASE
//{
//	UINT64 i64CharCd;
//	INT32 		iGameConnectKey;
//
//	GTM_PACKET_FRIEND_LIST_RQ()
//	{
//		usLength = sizeof( GTM_PACKET_FRIEND_LIST_RQ );
//		usType = GTM_FRIEND_LIST_RQ;
//	}
//};
//// [- MTD_FRIEND_LIST_RQ]
//struct MTD_PACKET_FRIEND_LIST_RQ  : public PACKET_BASE
//{
//	UINT64 i64CharCd;
//	INT32 		iGameConnectKey;
//	INT32 		iGameServerConnectionIndex;
//
//	MTD_PACKET_FRIEND_LIST_RQ()
//	{
//		usLength = sizeof( MTD_PACKET_FRIEND_LIST_RQ );
//		usType = MTD_FRIEND_LIST_RQ;
//	}
//};
//// [ DTM_FRIEND_LIST_AQ]		
//struct VBuffer_DTM_PACKET_FRIEND_LIST_AQ  : public PACKET_BASE
//{
//	char cResult;
//	UINT64 i64CharCd;
//	INT32 		iGameConnectKey;
//	INT32 		iGameServerConnectionIndex;
//
//	short	sCount;
//	ARRAY_DATA_START
//		UINT64		i64FriendCd;
//		UINT64		i64FriendCharCd;
//		char		acFriendNickName[ 1 ];
//		char		cState;
//	ARRAY_DATA_END
//};
//// [ MTG_FRIEND_LIST_AQ]
//struct VBuffer_MTG_PACKET_FRIEND_LIST_AQ  : public PACKET_BASE
//{
//	char cResult;
//	UINT64 i64CharCd;
//	INT32 		iGameConnectKey;
//
//	short	sCount;
//	ARRAY_DATA_START
//		UINT64		i64FriendCd;
//		UINT64		i64FriendCharCd;
//		char		acFriendNickName[ 1 ];
//		char		cState;
//	ARRAY_DATA_END
//
//};
//// [- GTC_FRIEND_LIST_AQ]
//struct VBuffer_GTC_PACKET_FRIEND_LIST_AQ  : public PACKET_BASE
//{
//	char cResult;
//	UINT64 i64CharCd;
//	INT32 		iGameConnectKey;
//
//	short	sCount;
//	ARRAY_DATA_START
//		UINT64		i64FriendCd;
//		UINT64		i64FriendCharCd;
//		char		acFriendNickName[ 1 ];
//		char		cState;
//	ARRAY_DATA_END
//};
//
//// [ MTG_FRIEND_LOGIN_STATE_MSG]
//struct VBuffer_MTG_FRIEND_LOGIN_STATE_MSG  : public PACKET_BASE
//{
//	UINT64		i64CharCd;
//
//	short	sCount;
//	ARRAY_DATA_START
//		UINT64		i64FriendCharCd;
//	ARRAY_DATA_END
//
//};
//// [- GTC_FRIEND_LOGIN_STATE_MSG]
//struct VBuffer_GTC_FRIEND_LOGIN_STATE_MSG  : public PACKET_BASE
//{
//	UINT64		i64CharCd;
//
//	short	sCount;
//	ARRAY_DATA_START
//		UINT64		i64FriendCharCd;
//	ARRAY_DATA_END
//
//};
//
//
//// [[친구의 로그인 통보]]
//// [- MTG_LOGIN_FRIEND_MSG]
//struct MTG_PACKET_LOGIN_FRIEND_MSG  : public PACKET_BASE
//{
//	UINT64 i64LogInCharCd;		// 로그인이 된 캐릭터의 코드
//	UINT64 i64FriendCharCd;	// 친구 캐릭터 코드
//	
//	MTG_PACKET_LOGIN_FRIEND_MSG()
//	{
//		usLength = sizeof( MTG_PACKET_LOGIN_FRIEND_MSG );
//		usType = MTG_LOGIN_FRIEND_MSG;
//	}
//};
//// [- GTC_LOGIN_FRIEND_MSG]
//struct GTC_PACKET_LOGIN_FRIEND_MSG  : public PACKET_BASE
//{
//	UINT64 i64LogInCharCd;
//	
//	GTC_PACKET_LOGIN_FRIEND_MSG()
//	{
//		usLength = sizeof( GTC_PACKET_LOGIN_FRIEND_MSG );
//		usType = GTC_LOGIN_FRIEND_MSG;
//	}
//};
//
//
//// [[친구의 로그아웃 통보]]
//// [- MTG_LOGOUT_FRIEND_MSG]
//struct MTG_PACKET_LOGOUT_FRIEND_MSG  : public PACKET_BASE
//{
//	UINT64 i64LogOutCharCd;	// 로그 아웃이 된 캐릭터의 코드
//	UINT64 i64FriendCharCd;	// 친구 캐릭터 코드
//	
//	MTG_PACKET_LOGOUT_FRIEND_MSG()
//	{
//		usLength = sizeof( MTG_PACKET_LOGOUT_FRIEND_MSG );
//		usType = MTG_LOGOUT_FRIEND_MSG;
//	}
//};
//// [- GTC_LOGOUT_FRIEND_MSG]
//struct GTC_PACKET_LOGOUT_FRIEND_MSG  : public PACKET_BASE
//{
//	UINT64 i64LogOutCharCd;
//	
//	GTC_PACKET_LOGOUT_FRIEND_MSG()
//	{
//		usLength = sizeof( GTC_PACKET_LOGOUT_FRIEND_MSG );
//		usType = GTC_LOGOUT_FRIEND_MSG;
//	}
//};
//
//
//// [[친구 추가 요청]]
//// [- CTG_NEW_FRIEND_RQ]
//struct CTG_PACKET_NEW_FRIEND_RQ  : public PACKET_BASE
//{
//	char acNewFriendName[ MAX_NICKNAME ]; // 추가할 친구의 이름
//
//	CTG_PACKET_NEW_FRIEND_RQ()
//	{
//		usLength = sizeof( CTG_PACKET_NEW_FRIEND_RQ );
//		usType = CTG_NEW_FRIEND_RQ;
//	}
//};
//// [- GTM_NEW_FRIEND_RQ]
//struct GTM_PACKET_NEW_FRIEND_RQ  : public PACKET_BASE
//{
//	UINT64 i64CharCd;					 // 요청자의 캐릭터 코드
//	char	acReqCharName[ MAX_NICKNAME ]; // 요청자의 이름
//	INT32 		iGameConnectKey;
//
//	char	acNewFriendName[ MAX_NICKNAME ];
//
//	GTM_PACKET_NEW_FRIEND_RQ()
//	{
//		usLength = sizeof( GTM_PACKET_NEW_FRIEND_RQ );
//		usType = GTM_NEW_FRIEND_RQ;
//	}
//};
//// [- MTD_NEW_FRIEND_RQ]
//struct MTD_PACKET_NEW_FRIEND_RQ  : public PACKET_BASE
//{
//	UINT64 i64CharCd;					// 요청자의 캐릭터 코드
//	char	acReqCharName[ MAX_NICKNAME ];	// 요청자의 이름
//	INT32 		iGameConnectKey;
//	INT32 		iGameServerConnectionIndex;
//
//	char	acNewFriendName[ MAX_NICKNAME ];
//
//	MTD_PACKET_NEW_FRIEND_RQ()
//	{
//		usLength = sizeof( MTD_PACKET_NEW_FRIEND_RQ );
//		usType = MTD_NEW_FRIEND_RQ;
//	}
//};
//// [- DTM_NEW_FRIEND_AQ]
//const char NEW_FRIEND_ERR_MAX_FRIEND_MY = 91; // 친구의 수가 지정된 수를 넘어섰다. 내 친구 리스트가
//const char NEW_FRIEND_ERR_MAX_FRIEND_OTHER = 92; // 친구의 수가 지정된 수를 넘어섰다. 친구의 친구 리스트가
//const char NEW_FRIEND_ERR_ALREADY_FRIEND_MY = 93; // 이미 친구이다. 내 친구 리스트에
//const char NEW_FRIEND_ERR_ALREADY_FRIEND_OTHER = 94; // 이미 친구이다. 친구의 친구 리스트에
//const char NEW_FRIEND_ERR_NONE_FRIEND = 95; // 친구는 없는 캐릭터이다.
//struct DTM_PACKET_NEW_FRIEND_AQ  : public PACKET_BASE
//{
//	char	cResult;
//	UINT64 i64CharCd;					// 요청자의 캐릭터 코드
//	char	acReqCharName[ MAX_NICKNAME ];	// 요청자의 이름
//	INT32 		iGameConnectKey;
//	INT32 		iGameServerConnectionIndex;
//
//	char acNewFriendName[ MAX_NICKNAME ];
//
//	UINT64 i64FriendCd;				// 친구 코드
//	UINT64 i64FriendCharCd;			// 친구의 캐릭터 코드
//	UINT64 i64FriendCd2;				// 친구 코드. 추가된 친구의
//
//	DTM_PACKET_NEW_FRIEND_AQ()
//	{
//		usLength = sizeof( DTM_PACKET_NEW_FRIEND_AQ );
//		usType = DTM_NEW_FRIEND_AQ;
//	}
//};
//// [- MTG_NEW_FRIEND_AQ]
//struct MTG_PACKET_NEW_FRIEND_AQ  : public PACKET_BASE
//{
//	char	cResult;
//	UINT64 i64CharCd;					// 요청자의 캐릭터 코드
//	INT32 		iGameConnectKey;
//	
//	char acNewFriendName[ MAX_NICKNAME ];
//
//	UINT64 i64FriendCd;				// 친구 코드
//	UINT64 i64FriendCharCd;			// 친구의 캐릭터 코드
//
//	MTG_PACKET_NEW_FRIEND_AQ()
//	{
//		usLength = sizeof( MTG_PACKET_NEW_FRIEND_AQ );
//		usType = MTG_NEW_FRIEND_AQ;
//	}
//};
//// [- GTC_NEW_FRIEND_AQ]
//struct GTC_PACKET_NEW_FRIEND_AQ  : public PACKET_BASE
//{
//	char	cResult;
//	
//	char acNewFriendName[ MAX_NICKNAME ];
//
//	UINT64 i64FriendCd;				// 친구 코드
//	UINT64 i64FriendCharCd;			// 친구의 캐릭터 코드
//
//	GTC_PACKET_NEW_FRIEND_AQ()
//	{
//		usLength = sizeof( GTC_PACKET_NEW_FRIEND_AQ );
//		usType = GTC_NEW_FRIEND_AQ;
//	}
//};
//
//
//// [[친구로 추가됨을 통보]]
//// [- MTG_NOTICE_NEW_FRIEND_MSG]
//struct MTG_PACKET_NOTICE_NEW_FRIEND_MSG  : public PACKET_BASE
//{
//	UINT64 i64CharCd;
//
//	UINT64 i64FriendCd;				// 친구 코드
//	UINT64 i64FriendCharCd;			// 친구의 캐릭터 코드
//	char acNewFriendName[ MAX_NICKNAME ]; // 친구 이름
//	
//	MTG_PACKET_NOTICE_NEW_FRIEND_MSG()
//	{
//		usLength = sizeof( MTG_PACKET_NOTICE_NEW_FRIEND_MSG );
//		usType = MTG_NOTICE_NEW_FRIEND_MSG;
//	}
//};
//// [- GTC_NOTICE_NEW_FRIEND_MSG]
//struct GTC_PACKET_NOTICE_NEW_FRIEND_MSG  : public PACKET_BASE
//{
//	UINT64 i64FriendCd;				// 친구 코드
//	UINT64 i64FriendCharCd;			// 친구의 캐릭터 코드
//	
//	char acNewFriendName[ MAX_NICKNAME ]; // 친구 이름
//	
//	GTC_PACKET_NOTICE_NEW_FRIEND_MSG()
//	{
//		usLength = sizeof( GTC_PACKET_NOTICE_NEW_FRIEND_MSG );
//		usType = GTC_NOTICE_NEW_FRIEND_MSG;
//	}
//};
//// [- CTG_ACCEPT_NEW_FRIEND_RQ]
//struct CTG_PACKET_ACCEPT_NEW_FRIEND_RQ  : public PACKET_BASE
//{
//	char	cIsOK;			// '1'이면 친구 수락
//	UINT64 i64FriendCd;			
//	UINT64 i64FriendCharCd;		
//	
//	CTG_PACKET_ACCEPT_NEW_FRIEND_RQ()
//	{
//		usLength = sizeof( CTG_PACKET_ACCEPT_NEW_FRIEND_RQ );
//		usType = CTG_ACCEPT_NEW_FRIEND_RQ;
//	}
//};
//// [- GTM_ACCEPT_NEW_FRIEND_RQ]
//struct GTM_PACKET_ACCEPT_NEW_FRIEND_RQ  : public PACKET_BASE
//{
//	UINT64 i64CharCd;				
//	INT32 		iGameConnectKey;
//
//	char	cIsOK;
//	UINT64 i64FriendCd;			
//	UINT64 i64FriendCharCd;		
//	
//	GTM_PACKET_ACCEPT_NEW_FRIEND_RQ()
//	{
//		usLength = sizeof( GTM_PACKET_ACCEPT_NEW_FRIEND_RQ );
//		usType = GTM_ACCEPT_NEW_FRIEND_RQ;
//	}
//};
//// [- MTD_ACCEPT_NEW_FRIEND_RQ]
//struct MTD_PACKET_ACCEPT_NEW_FRIEND_RQ  : public PACKET_BASE
//{
//	UINT64 i64CharCd;				
//	INT32 		iGameConnectKey;
//	INT32 		iGameServerConnectionIndex;
//
//	char	cIsOK;
//	UINT64 i64FriendCd;			
//	UINT64 i64FriendCharCd;
//		
//	MTD_PACKET_ACCEPT_NEW_FRIEND_RQ()
//	{
//		usLength = sizeof( MTD_PACKET_ACCEPT_NEW_FRIEND_RQ );
//		usType = MTD_ACCEPT_NEW_FRIEND_RQ;
//	}
//};
//// [- DTM_ACCEPT_NEW_FRIEND_AQ]
//struct DTM_PACKET_ACCEPT_NEW_FRIEND_AQ  : public PACKET_BASE
//{
//	char	cResult;
//	UINT64 i64CharCd;				
//	INT32 		iGameConnectKey;
//	INT32 		iGameServerConnectionIndex;
//
//	char	cIsOK;
//	UINT64 i64FriendCd;
//	UINT64 i64FriendCharCd;		
//	
//	DTM_PACKET_ACCEPT_NEW_FRIEND_AQ()
//	{
//		usLength = sizeof( DTM_PACKET_ACCEPT_NEW_FRIEND_AQ );
//		usType = DTM_ACCEPT_NEW_FRIEND_AQ;
//	}
//};
//// [- MTG_ACCEPT_NEW_FRIEND_AQ]
//struct MTG_PACKET_ACCEPT_NEW_FRIEND_AQ  : public PACKET_BASE
//{
//	char	cResult;
//	UINT64 i64CharCd;				
//	INT32 		iGameConnectKey;
//	
//	char	cIsOK;
//	UINT64 i64FriendCd;
//	UINT64 i64FriendCharCd;		
//	
//	MTG_PACKET_ACCEPT_NEW_FRIEND_AQ()
//	{
//		usLength = sizeof( MTG_PACKET_ACCEPT_NEW_FRIEND_AQ );
//		usType = MTG_ACCEPT_NEW_FRIEND_AQ;
//	}
//};
//// [- CTG_ACCEPT_NEW_FRIEND_AQ]
//struct CTG_PACKET_ACCEPT_NEW_FRIEND_AQ  : public PACKET_BASE
//{
//	char	cResult;
//	
//	char	cIsOK;
//	UINT64 i64FriendCd;
//	UINT64 i64FriendCharCd;		
//	
//	CTG_PACKET_ACCEPT_NEW_FRIEND_AQ()
//	{
//		usLength = sizeof( CTG_PACKET_ACCEPT_NEW_FRIEND_AQ );
//		usType = CTG_ACCEPT_NEW_FRIEND_AQ;
//	}
//};
//
//
//// [[친구 삭제]]
//// [- CTG_DELETE_FRIEND_RQ]
//struct CTG_PACKET_DELETE_FRIEND_RQ  : public PACKET_BASE
//{
//	UINT64 i64FriendCd;
//	UINT64 i64FriendCharCd;
//	
//	CTG_PACKET_DELETE_FRIEND_RQ()
//	{
//		usLength = sizeof( CTG_PACKET_DELETE_FRIEND_RQ );
//		usType = CTG_DELETE_FRIEND_RQ;
//	}
//};
//// [- GTM_DELETE_FRIEND_RQ]
//struct GTM_PACKET_DELETE_FRIEND_RQ  : public PACKET_BASE
//{
//	UINT64 i64CharCd;				
//	INT32 		iGameConnectKey;
//
//	UINT64 i64FriendCd;
//	UINT64 i64FriendCharCd;
//	
//	GTM_PACKET_DELETE_FRIEND_RQ()
//	{
//		usLength = sizeof( GTM_PACKET_DELETE_FRIEND_RQ );
//		usType = GTM_DELETE_FRIEND_RQ;
//	}
//};
//// [- MTD_DELETE_FRIEND_RQ]
//struct MTD_PACKET_DELETE_FRIEND_RQ  : public PACKET_BASE
//{
//	UINT64 i64CharCd;				
//	INT32 		iGameConnectKey;
//	INT32 		iGameServerConnectionIndex;
//
//	UINT64 i64FriendCd;
//	UINT64 i64FriendCharCd;		
//	
//	MTD_PACKET_DELETE_FRIEND_RQ()
//	{
//		usLength = sizeof( MTD_PACKET_DELETE_FRIEND_RQ );
//		usType = MTD_DELETE_FRIEND_RQ;
//	}
//};
//// [- DTM_DELETE_FRIEND_AQ]
//const char DEL_FRIEND_ERR_NOT_FRIEND = 81; // 친구가 아니다.
//struct DTM_PACKET_DELETE_FRIEND_AQ  : public PACKET_BASE
//{
//	char	cResult;
//	
//	UINT64 i64CharCd;				
//	INT32 		iGameConnectKey;
//	INT32 		iGameServerConnectionIndex;
//
//	UINT64 i64FriendCd;
//	UINT64 i64FriendCharCd;		
//	
//	DTM_PACKET_DELETE_FRIEND_AQ()
//	{
//		usLength = sizeof( DTM_PACKET_DELETE_FRIEND_AQ );
//		usType = DTM_DELETE_FRIEND_AQ;
//	}
//};
//// [- MTG_DELETE_FRIEND_AQ]
//struct MTG_PACKET_DELETE_FRIEND_AQ  : public PACKET_BASE
//{
//	char	cResult;
//	
//	UINT64 i64CharCd;				
//	INT32 		iGameConnectKey;
//	
//	UINT64 i64FriendCd;
//	UINT64 i64FriendCharCd;		
//	
//	MTG_PACKET_DELETE_FRIEND_AQ()
//	{
//		usLength = sizeof( MTG_PACKET_DELETE_FRIEND_AQ );
//		usType = MTG_DELETE_FRIEND_AQ;
//	}
//};
//// [- GTC_DELETE_FRIEND_AQ]
//struct GTC_PACKET_DELETE_FRIEND_AQ  : public PACKET_BASE
//{
//	char	cResult;
//	UINT64 i64FriendCd;
//	
//	GTC_PACKET_DELETE_FRIEND_AQ()
//	{
//		usLength = sizeof( GTC_PACKET_DELETE_FRIEND_AQ );
//		usType = GTC_DELETE_FRIEND_AQ;
//	}
//};
//
//
//
///////////////// [ GM 및 치팅 패킷 정의 ] //////////////
//// [[캐릭터 정보 변경]]
//// [- [CTG_CHEAT_CHARACTER_INFO_RQ] 
//const char CHEAT_CHARACTER_INFO_LEVEL = 1; // 레벨	
//const char CHEAT_CHARACTER_INFO_EXP = 2; // 경험치
//const char CHEAT_CHARACTER_INFO_GAMEMONEY = 3; // 게임 머니
//const char CHEAT_CHARACTER_INFO_CASHMONEY = 4; // 캐쉬 머니
//const char CHEAT_CHARACTER_INFO_PPOUR = 5; // 인기도
//struct CTG_PACKET_CHEAT_CHARACTER_INFO_RQ  : public PACKET_BASE
//{
//	char	cIndex;	// 어떤 부분을 변경 하기를 원하는지 선택
//	INT32 		iValue; // 변경 값
//
//	CTG_PACKET_CHEAT_CHARACTER_INFO_RQ()
//	{
//		usLength = sizeof( CTG_PACKET_CHEAT_CHARACTER_INFO_RQ );
//		usType = CTG_CHEAT_CHARACTER_INFO_RQ;
//	}
//};
//// [- GTC_CHEAT_CHARACTER_INFO_AQ]
//struct GTC_PACKET_CHEAT_CHARACTER_INFO_AQ  : public PACKET_BASE
//{
//	char	cResult;
//	char	cIndex;	// 어떤 부분을 변경 하기를 원하는지 선택
//	INT32 		iValue; // 변경 값
//
//	GTC_PACKET_CHEAT_CHARACTER_INFO_AQ()
//	{
//		usLength = sizeof( GTC_PACKET_CHEAT_CHARACTER_INFO_AQ );
//		usType = GTC_CHEAT_CHARACTER_INFO_AQ;
//	}
//};
//
//
//// [[게임 중 게임 상태 변경]]
//// [- CTG_CHEAT_ROOM_GAME_RQ]
//const char CHEAT_ROOM_GAME_CORRECT_ANSWER = 1; // 맞춘 게임 수
//const char CHEAT_ROOM_GAME_RECV_VOTE = 2; // 투표를 받은 수
//const char CHEAT_ROOM_GAME_END = 3; // 게임을 강제 종료
//const char CHEAT_ROOM_GAME_NEXT_ROUND = 4; // 다음 라운드로 
//const char CHEAT_ROOM_GAME_NEXT_QUIZ = 5; // 다음 라운드에서 풀 퀴즈 번호와 답 영역 
//struct CTG_PACKET_CHEAT_ROOM_GAME_RQ  : public PACKET_BASE
//{
//	char	cIndex;		// 어떤 부분을 변경 하기를 원하는지 선택
//	UINT64 i64CharCd;	// 변경 되기를 바라는 캐릭터의 코드
//	INT32 	iValue; // 변경 값
//	INT32	NextRoundQuizCd; // 다음 라운드에서 풀고 싶은 퀴즈의 번호
//	INT32	QuizAnswer; // 0(1-10), 1(91-100), 2(41-50), 3(21-30), 4(31-40), 5(11-20)
//
//	CTG_PACKET_CHEAT_ROOM_GAME_RQ()
//	{
//		usLength = sizeof( CTG_PACKET_CHEAT_ROOM_GAME_RQ );
//		usType = CTG_CHEAT_ROOM_GAME_RQ;
//	}
//};
//// [- GTC_CHEAT_ROOM_GAME_AQ]
//struct GTC_PACKET_CHEAT_ROOM_GAME_AQ  : public PACKET_BASE
//{
//	char	cResult;
//	char	cIndex;		// 어떤 부분을 변경 하기를 원하는지 선택
//	UINT64 i64CharCd;	// 변경 되기를 바라는 캐릭터의 코드
//	INT32 	iValue; // 변경 값
//	INT32	NextRoundQuizCd;
//	INT32	QuizAnswer;
//
//	GTC_PACKET_CHEAT_ROOM_GAME_AQ()
//	{
//		usLength = sizeof( GTC_PACKET_CHEAT_ROOM_GAME_AQ );
//		usType = GTC_CHEAT_ROOM_GAME_AQ;
//	}
//};
//
//
//// [[ 주관식 퀴즈 답변 요구 ]]
//// [ CTG_CHEAT_REQ_SUBJECTIVE_QUIZ_ANSWER_RQ ]
//struct CTG_PACKET_CHEAT_REQ_SUBJECTIVE_QUIZ_ANSWER_RQ : public PACKET_BASE
//{	
//	CTG_PACKET_CHEAT_REQ_SUBJECTIVE_QUIZ_ANSWER_RQ()
//	{
//		usLength = sizeof( CTG_PACKET_CHEAT_REQ_SUBJECTIVE_QUIZ_ANSWER_RQ );
//		usType = CTG_CHEAT_REQ_SUBJECTIVE_QUIZ_ANSWER_RQ;
//	}
//};
//
//const int GTC_CHEAT_REQ_SUBJECTIVE_QUIZ_ANSWER_ANSWER_COUNT = 10;
//// [ GTC_CHEAT_REQ_SUBJECTIVE_QUIZ_ANSWER_AQ ]
//struct GTC_PACKET_CHEAT_REQ_SUBJECTIVE_QUIZ_ANSWER_AQ : public PACKET_BASE
//{	
//	char cResult;
//	char acAnswerText[ GTC_CHEAT_REQ_SUBJECTIVE_QUIZ_ANSWER_ANSWER_COUNT ][ MAX_SUBJECTIVE_QUESTION_ANSWER_LEN ];
//
//	GTC_PACKET_CHEAT_REQ_SUBJECTIVE_QUIZ_ANSWER_AQ()
//	{
//		usLength = sizeof( GTC_PACKET_CHEAT_REQ_SUBJECTIVE_QUIZ_ANSWER_AQ );
//		usType = GTC_CHEAT_REQ_SUBJECTIVE_QUIZ_ANSWER_AQ;
//	}
//};
//
//
//
//
//
//
///////////////// [ 서버 관리 패킷 정의 ] ///////////////
//// [ SMTD_LOAD_BRIDGE_INFO_RQ]
//struct SMTD_PACKET_LOAD_BRIDGE_INFO_RQ  : public PACKET_BASE
//{
//	SMTD_PACKET_LOAD_BRIDGE_INFO_RQ()
//	{
//		usLength = sizeof( SMTD_PACKET_LOAD_BRIDGE_INFO_RQ );
//		usType = SMTD_LOAD_BRIDGE_INFO_RQ;
//	}
//};
//
//const INT32 MAX_LOAD_BRIDGE_INFO_COUNT = 64;
//// [ SMTD_LOAD_BRIDGE_INFO_AQ]
//struct SMTD_PACKET_LOAD_BRIDGE_INFO_AQ : public PACKET_BASE
//{
//	INT32 		iCount;
//
//	INT32 		aiCode[ MAX_LOAD_BRIDGE_INFO_COUNT ];
//	char	acIP[ MAX_IP_LEN ][ MAX_LOAD_BRIDGE_INFO_COUNT ];
//	
//	SMTD_PACKET_LOAD_BRIDGE_INFO_AQ()
//	{
//		usLength = sizeof( SMTD_PACKET_LOAD_BRIDGE_INFO_AQ );
//		usType = SMTD_LOAD_BRIDGE_INFO_AQ;
//	}
//};
//
//
//// [ SMTD_LOAD_APPSERVER_INFO_RQ]
//struct SMTD_PACKET_LOAD_APPSERVER_INFO_RQ  : public PACKET_BASE
//{
//	SMTD_PACKET_LOAD_APPSERVER_INFO_RQ()
//	{
//		usLength = sizeof( SMTD_PACKET_LOAD_APPSERVER_INFO_RQ );
//		usType = SMTD_LOAD_APPSERVER_INFO_RQ;
//	}
//};
//
//const INT32 MAX_LOAD_APPSERVER_INFO_COUNT = 64;
//// [ SMTD_LOAD_APPSERVER_INFO_AQ]
//struct SMTD_PACKET_LOAD_APPSERVER_INFO_AQ : public PACKET_BASE
//{
//	INT32 		iCount;
//
//	INT32 		aiCode[ MAX_LOAD_APPSERVER_INFO_COUNT ];
//	char	acName[ MAX_LOAD_APPSERVER_INFO_COUNT ][ MAX_SERVER_NAME_LEN ];
//	INT32 		aiKind[ MAX_LOAD_APPSERVER_INFO_COUNT ];
//	char	acIP[ MAX_LOAD_APPSERVER_INFO_COUNT ][ MAX_IP_LEN ];
//	INT32 		aiPort[ MAX_LOAD_APPSERVER_INFO_COUNT ];
//	short	asBridgeCd[ MAX_LOAD_APPSERVER_INFO_COUNT ];
//	char	cArrDir[ MAX_LOAD_APPSERVER_INFO_COUNT ][ MAX_SERVER_EXEC_DIRECTORY_LEN ];
//	char	acFileNmae[ MAX_LOAD_APPSERVER_INFO_COUNT ][ MAX_SERVER_FILE_NAME_LEN ];
//	
//	SMTD_PACKET_LOAD_APPSERVER_INFO_AQ()
//	{
//		usLength = sizeof( SMTD_PACKET_LOAD_APPSERVER_INFO_AQ );
//		usType = SMTD_LOAD_APPSERVER_INFO_AQ;
//	}
//};
//
//// [- BTSM_CONNECT_BRIDGE_RQ]
//struct BTSM_PACKET_CONNECT_BRIDGE_RQ : public PACKET_BASE
//{ 
//	char acIP[ MAX_IP_LEN ]; 
//	
//	BTSM_PACKET_CONNECT_BRIDGE_RQ()
//	{
//		usLength = sizeof( BTSM_PACKET_CONNECT_BRIDGE_RQ );
//		usType = BTSM_CONNECT_BRIDGE_RQ;
//	}
//};
//// [- BTSM_CONNECT_BRIDGE_AQ]
//struct VBuffer_BTSM_CONNECT_BRIDGE_AQ : public PACKET_BASE
//{ 
//	INT32 iCount;
//	
//	ARRAY_DATA_START
//		short	asServerCode;
//		char	acServerKind;
//		INT32 		aiServerPort;
//		char	cArrDir[ 1 ];
//		char	acFileNmae[ 1 ];
//		char	cStatus;
//	ARRAY_DATA_END
//};
//
//
//// [- CTSM_CONNECT_CLIENT_RQ]
//struct CTSM_PACKET_CONNECT_CLIENT_RQ : public PACKET_BASE
//{ 
//	char acID[ MAX_USERID ]; 
//	char acPW[ MAX_USERPW ];
//
//	CTSM_PACKET_CONNECT_CLIENT_RQ()
//	{
//		usLength = sizeof( CTSM_PACKET_CONNECT_CLIENT_RQ );
//		usType = CTSM_CONNECT_CLIENT_RQ;
//	}
//};
//// [- SMTD_CONNECT_CLIENT_RQ]
//struct SMTD_PACKET_CONNECT_CLIENT_RQ  : public PACKET_BASE
//{
//	char	acID[ MAX_USERID ];
//	char	acPW[ MAX_USERPW ];
//	DWORD	dwGameConnectKey;
//
//	SMTD_PACKET_CONNECT_CLIENT_RQ()
//	{
//		usLength = sizeof( SMTD_PACKET_CONNECT_CLIENT_RQ );
//		usType = SMTD_CONNECT_CLIENT_RQ;
//	}
//};
//// [- SMTD_CONNECT_CLIENT_AQ]
//struct SMTD_PACKET_CONNECT_CLIENT_AQ  : public PACKET_BASE
//{
//	char	cResult;
//	char	cAdminLevel;
//
//	char	acID[ MAX_USERID ];
//	DWORD	dwGameConnectKey;
//	UINT64 i64UKey;
//
//	SMTD_PACKET_CONNECT_CLIENT_AQ()
//	{
//		usLength = sizeof( SMTD_PACKET_CONNECT_CLIENT_AQ );
//		usType = SMTD_CONNECT_CLIENT_AQ;
//	}
//};
//// [- SMTC_CONNECT_CLIENT_AQ]
//struct SMTC_PACKET_CONNECT_CLIENT_AQ : public PACKET_BASE
//{ 
//	char	cResult;
//	char	cAdminLevel;
//
//	SMTC_PACKET_CONNECT_CLIENT_AQ()
//	{
//		usLength = sizeof( SMTC_PACKET_CONNECT_CLIENT_AQ );
//		usType = SMTC_CONNECT_CLIENT_AQ;
//	}
//};
//
//// [- CTSM_SERVER_LIST_RQ]
//struct CTSM_PACKET_SERVER_LIST_RQ  : public PACKET_BASE
//{
//	CTSM_PACKET_SERVER_LIST_RQ()
//	{
//		usLength = sizeof( CTSM_PACKET_SERVER_LIST_RQ );
//		usType = CTSM_SERVER_LIST_RQ;
//	}
//};
//// [- SMTC_SERVER_LIST_AQ]
//struct VBuffer_SMTC_SERVER_LIST_AQ : public PACKET_BASE
//{ 
//	INT32 iCount;
//	
//	ARRAY_DATA_START
//		short	sServerCode;
//		char	acServerKind;
//		char	acName[ MAX_SERVER_NAME_LEN ];
//		char	acIP[ MAX_IP_LEN ];
//		INT32 		iServerPort;
//		short	sBridgeCd;
//		char	cServerStatus;
//		short	sUserCount;
//	ARRAY_DATA_END
//};
//
//
//// [[서버 시작 요청]]
//// [- CTSM_START_SERVER_RQ]
//struct CTSM_PACKET_START_SERVER_RQ  : public PACKET_BASE
//{
//	short	sBridgeCd; // 브릿지 코드 
//	short	sServerCd; // 서버 코드
//
//	CTSM_PACKET_START_SERVER_RQ()
//	{
//		usLength = sizeof( CTSM_PACKET_START_SERVER_RQ );
//		usType = CTSM_START_SERVER_RQ;
//	}
//};
//
//// [- BTSM_START_SERVER_RQ]
//struct BTSM_PACKET_START_SERVER_RQ : public PACKET_BASE
//{ 
//	short	sServerCd; // 서버 코드
//
//	BTSM_PACKET_START_SERVER_RQ()
//	{
//		usLength = sizeof( BTSM_PACKET_START_SERVER_RQ );
//		usType = BTSM_START_SERVER_RQ;
//	}
//};
//
//const char SERVER_STATE_UNKNOWN = 0; // 브릿지에 미 등록
//const char SERVER_STATE_NOT_RUN = 1; // 프로세스 미 실행
//const char SERVER_STATE_STOP = 2; // 프로세스 실행 - 서버 중단
//const char SERVER_STATE_WAKEUP = 3; // 프로세스 실행 - 서버 준비 중
//const char SERVER_STATE_RUNNING = 4; // 프로세스 실행 - 서버 실행
//const char SERVER_STATE_ABNORMAL = 5; // 프로세스 실행 - 서버 오 작동 중
//const char SERVER_STATE_BRIDGE_CONNECTING = 6; // 프로세스 실행 - 브릿지가 접속 중
//const char SERVER_STATE_CHECKING = 7; // 서버 점검 중
//const char SERVER_STATE_DISABLE_RUN = 8; // 프로세스 실행 - 실행이 안되는 상태
//
//// [- BTSM_NOTICE_SERVER_STATUS_MSG]
//struct BTSM_PACKET_NOTICE_SERVER_STATUS_MSG : public PACKET_BASE
//{ 
//	short	sServerCd;  // 서버 코드
//	short	sUsercount; // 유저 수
//	char	cStatus;	// 상태
//
//	BTSM_PACKET_NOTICE_SERVER_STATUS_MSG()
//	{
//		usLength = sizeof( BTSM_PACKET_NOTICE_SERVER_STATUS_MSG );
//		usType = BTSM_NOTICE_SERVER_STATUS_MSG;
//	}
//};
//
//// [서버 상태 통보]
//// [- CTSM_NOTICE_SERVER_STATUS_MSG]
//struct CTSM_PACKET_NOTICE_SERVER_STATUS_MSG : public PACKET_BASE
//{ 
//	short	sServerCd;  // 서버 코드
//	short	sUsercount; // 유저 수
//	char	cStatus;	// 상태
//
//	CTSM_PACKET_NOTICE_SERVER_STATUS_MSG()
//	{
//		usLength = sizeof( CTSM_PACKET_NOTICE_SERVER_STATUS_MSG );
//		usType = CTSM_NOTICE_SERVER_STATUS_MSG;
//	}
//};
//
//// [[서버 종료 요청]]
//// [-  CTSM_TERMINATE_SERVER_RQ]
//struct CTSM_PACKET_TERMINATE_SERVER_RQ : public PACKET_BASE
//{ 
//	short	sBridgeCd;
//	short	sServerCd;  
//
//	CTSM_PACKET_TERMINATE_SERVER_RQ()
//	{
//		usLength = sizeof( CTSM_PACKET_TERMINATE_SERVER_RQ );
//		usType = CTSM_TERMINATE_SERVER_RQ;
//	}
//};
//// [- SMTB_TERMINATE_SERVER_RQ]			
//struct SMTB_PACKET_TERMINATE_SERVER_RQ : public PACKET_BASE
//{ 
//	short	sServerCd;  
//
//	SMTB_PACKET_TERMINATE_SERVER_RQ()
//	{
//		usLength = sizeof( SMTB_PACKET_TERMINATE_SERVER_RQ );
//		usType = SMTB_TERMINATE_SERVER_RQ;
//	}
//};
//
//// [- SMTC_CLOSE_BRIDGE_MSG]
//struct SMTB_PACKET_CLOSE_BRIDGE_MSG  : public PACKET_BASE
//{
//	short sBridgeCd;
//
//	SMTB_PACKET_CLOSE_BRIDGE_MSG()
//	{
//		usLength = sizeof( SMTB_PACKET_CLOSE_BRIDGE_MSG );
//		usType = SMTC_CLOSE_BRIDGE_MSG;
//	}
//};
//
//
//
/////////////////////// [ 개발자용 패킷 ] /////////////////////
//const char GAME_SERVER_DATA_RELOAD_RQ_RELOAD_DATA_KIND_ITEM = 1; // 아이템
//const char GAME_SERVER_DATA_RELOAD_RQ_RELOAD_DATA_KIND_SKILL = 2; // 스킬
//const char GAME_SERVER_DATA_RELOAD_RQ_RELOAD_DATA_KIND_SUBJECTQUIZ = 3; // 퀴즈
//// [[게임서버의 DB 데이터 재 로딩 요청]]
//// [ CTG_DEV_SERVER_DATA_RELOAD_RQ]
//struct CTG_PACKET_DEV_SERVER_DATA_RELOAD_RQ  : public PACKET_BASE
//{
//	char	cReloadDataKind;	// 로딩할 데이터 종료
//	
//	CTG_PACKET_DEV_SERVER_DATA_RELOAD_RQ()
//	{
//		usLength = sizeof( CTG_PACKET_DEV_SERVER_DATA_RELOAD_RQ );
//		usType = CTG_DEV_SERVER_DATA_RELOAD_RQ;
//	}
//};
//
//// [ GTC_DEV_SERVER_DATA_RELOAD_AQ]
//struct GTC_PACKET_DEV_SERVER_DATA_RELOAD_AQ  : public PACKET_BASE
//{
//	char	cResult;
//	char	cReloadDataKind;
//	
//	GTC_PACKET_DEV_SERVER_DATA_RELOAD_AQ()
//	{
//		usLength = sizeof( GTC_PACKET_DEV_SERVER_DATA_RELOAD_AQ );
//		usType = GTC_DEV_SERVER_DATA_RELOAD_AQ;
//	}
//};


// [[ 로그인 서버와의 접속 종료 요청 ]]
// [- CTL_DEV_LOGIN_CLOSE_RQ ]
struct CTL_PACKET_DEV_LOGIN_CLOSE_RQ  : public PACKET_BASE
{
	CTL_PACKET_DEV_LOGIN_CLOSE_RQ()
	{
		usLength = sizeof( CTL_PACKET_DEV_LOGIN_CLOSE_RQ );
		usType = CTL_DEV_LOGIN_CLOSE_RQ;
	}
};


//// [[ 공지 요청 ]]
//// [- CTSM_NOTICE_RQ ]
//struct CTSM_PACKET_NOTICE_RQ  : public PACKET_BASE
//{
//	short sBridgeCd; // '0' 이면 모든 게임 서버에 통보
//	short sServerCd; // 서버 코드 
//	
//	char	acNoticeText[ MAX_NOTICE_LEN ];	
//	
//	CTSM_PACKET_NOTICE_RQ()
//	{
//		usLength = sizeof( CTSM_PACKET_NOTICE_RQ );
//		usType = CTSM_NOTICE_RQ;
//	}
//};
//
//// [- SMTB_NOTICE_RQ ]
//struct SMTB_PACKET_NOTICE_RQ  : public PACKET_BASE
//{
//	short sBridgeCd; 
//	short sServerCd; 
//
//	short	sTextLen;
//	char	acNoticeText[ MAX_NOTICE_LEN ];
//	
//	SMTB_PACKET_NOTICE_RQ()
//	{
//		usLength = sizeof( SMTB_PACKET_NOTICE_RQ );
//		usType = SMTB_NOTICE_RQ;
//	}
//};
//
//
//// [[ 서버 상태 변경 ]]
//// [- CTSM_CHANGE_SERVER_STATE_RQ ]
//struct CTSM_PACKET_CHANGE_SERVER_STATE_RQ  : public PACKET_BASE
//{
//	short sBridgeCd;
//	short sServerCd;
//	char	cServerState;
//	
//	CTSM_PACKET_CHANGE_SERVER_STATE_RQ()
//	{
//		usLength = sizeof( CTSM_PACKET_CHANGE_SERVER_STATE_RQ );
//		usType = CTSM_CHANGE_SERVER_STATE_RQ;
//	}
//};
//
//// [- SMTB_CHANGE_SERVER_STATE_RQ ]
//struct SMTB_PACKET_CHANGE_SERVER_STATE_RQ  : public PACKET_BASE
//{
//	short sServerCd;
//	char	cServerState;
//	
//	SMTB_PACKET_CHANGE_SERVER_STATE_RQ()
//	{
//		usLength = sizeof( SMTB_PACKET_CHANGE_SERVER_STATE_RQ );
//		usType = SMTB_CHANGE_SERVER_STATE_RQ;
//	}
//};
//
//
//// [- GTC_MANAGE_NOTICE_MSG ]
//struct GTC_PACKET_MANAGE_NOTICE_MSG : public PACKET_BASE
//{
//	short	sTextLen;
//	char	acNoticeText[ MAX_NOTICE_LEN ];
//	
//	GTC_PACKET_MANAGE_NOTICE_MSG()
//	{
//		usLength = sizeof( GTC_PACKET_MANAGE_NOTICE_MSG );
//		usType = GTC_MANAGE_NOTICE_MSG;
//	}
//};





#pragma pack()