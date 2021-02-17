#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_

/*
 모든 패킷에는 패킷의 첫 부분에는 int(4 Byte)로 패킷의 총 크기를 지정하고
 char(1 Byte)로 패킷의 내용을 지정한다.

  예) 로그인 요청
		struct {
					int		size
					char	type
					char	idLen
					string  id
					char	pwdLen
					string	pwd
					short	version
				}

	1: char		- 1 Byte
	2: short	- 2 Byte

 클라이언트의 요청에 의한 서버에서의 결과 통보 시에는 요청에 대해 성공 일 때에만
 부가적인 정보를 패킷에 붙여 보낸다.
*/

enum
{
	// 유저의 로그인 처리 -------------------------------------
	
	REQUEST_LOGIN = 0,				//	C -> S	로그인 요청
	// idLen(1), id(text),  pwdLen(1), pwd(text), version(2) 
	
	ANSWER_LOGIN,					//	S -> C 로그인 결가 통보  
	// result(1), [ nicLen(1), NicName(text), sex(1), DBIndex(4)
	//		       level(1), win(4), lose(4), disconnect(4), 상위 돈(4), 하위 돈(4), 
	//			비공개 생성방아이템(1), 점프아이템(1), 아바타레이어(32),아바타타입(1) ]	
	// : 아이템들은 TRUE이면 사용 가능, 아바타 정보는 short 형으로 16배열값과 아바타 타입이다.	

	// ---------------------------------------------------------


	
	
	// 채널의 리스트 요청   -------------------
	
	REQUEST_CHANNEL_LIST,
	//  
	
	ANSWER_CHANNEL_LIST,
	// LowNum(1),MidNum(1), HighNum(1), MaxUserNum(2), [CurUserNum(shorts)], 돈(INT64) 
	// usernum 스트링을 2바이트씩 읽어오면 각 채널의 현재 인원을 알수 있다.
	
	// --------------------------------------

	
	

	// 유저 검색 (친구 찾기 ) -------------------------------

	REQUEST_FIND_FRIEND,
	// 찾는 친구 아이디 길이(1), 찾는 친구 아이디 문자열(text) 

	ANSWER_FIND_FRIEND,
	// result(1), idLen(1), id(text), [ channelNum(-1), roomNum(-1) ]
	// roomNum의 경우 Byte와 char의 차이에 의해 0 이하의 값이 들어 갈수 있음을 뜻한다.
	
	// ------------------------------------------------



	// 로그 아웃  ---------------------------
	
	REQUEST_LOGOUT,
	// DBIndex(4)

	ANSWER_LOGOUT,

	REQUEST_LOGOUT_DB,		// S -> S 로그 아웃시의 DB 처리

	
	// --------------------------------------



	// 라이프 신호 ( 유저의 소켓이 살아 있음을 통보 ) ------

	REQUEST_LIFECOUNT,
	 

	// -------------------------------------------------------



	// 채널 입장 ------------------------------

	REQUEST_JOINCHANNEL,
	// channelNum(1)

	ANSWER_JOINCHANNEL,
	// result(1),[ channelNum(1), maxroomNum(1) ]

	// -----------------------------------------

	
	// 채널에 있는 룸, 유저 리스트를 보내준다.

	ANSWER_NOTIFY_ROOMLIST,		// 룸 리스트 
	// roomNum(1), roomSerial(1), ownerLen(1), owner(text), titleLen(1), title(text),
	// roomInuserNum(1), type(1), status(1)

	
	ANSWER_NOTIFY_USERLIST,		// 채널 유저 리스트
	// usernum(1), idLen(1),id(text), position(1), nicLen(1), 
	// NicName(text), sex(1), DBIndex(4), level(1), 방번호(1), 방에서순번(1)

	// ---------------------------------------------
	

	
	// 로비에서의 채팅 ---------------------------------------
	
	REQUEST_CHATINLOBBY,
	// 보낸 사람:DBIndex(4), 메세지 길이(1), 메세지(text)

	ANSWER_CHATINLOBBY,
	// 보낸 사람:DBIndex(4), 메세지 길이(1), 메세지(text)

	// ---------------------------------------------------



	// 귓속말 (로비에서의 채팅시의 귓속말을 뜻한다.) -----------------------------

	REQUEST_WHISPER,
	// ToDBIndex(4), FromDBIndex(4), msgLen(1), msg(text)

	ANSWER_WHISPER,
	// ToDBIndex(4), FromDBIndex(4), msgLen(1), msg(text)

	ANSWER_WHISPER_FAIL,
	// 상대방(B)가 로비에 없어 귓속말을 받지 경우 요청자(A)에게 알려준다.

	// ----------------------------------------------------------------------------



	// 초대 및 1:1 대화 거부 ---------------------------
	
	REQUEST_ALLINVITE_REJECT,
	// onoff(1), 거부일 때는 - 0, 받을 때는 1

	ANSWER_ALLINVITE_REJECT,
	// onoff(1)

	REQUEST_ALLCHAT_REJECT,
	// onoff(1)

	ANSWER_ALLCHAT_REJECT,
	// onoff(1)
	
	// -----------------------------------------------



	// 1:1 대화 ------------------------------

	REQUEST_1ON1CHAT_DEMANDA,	
	// 상대방(B):DBIndex(4), 코멘트길이(1), 코멘트(text)

	ANSWER_1ON1CHAT_DEMANDB,	
	// 요청자(A):DBIndex(4),채널번호(1), 코멘트길이(1), 코멘트(text) 

	REQUEST_1ON1CHAT_RESULTB,
	// 결과(1), 요청자(A):DBIndex(4), 채널번호(1)

	ANSWER_1ON1CHAT_RESULTA, 
	// 결과(1), 상대방(B):DBIndex(4)
	
	REQUEST_1ON1CHAT,
	// 상대방(B):DBIndex(4),  메세지길이(1), 메세지(text)

	ANSWER_1ON1CHAT,											// 상대방만 받는다
	// 요청자(A):DBIndex(4), 메세지길이(1), 메세지(text)

	REQUEST_1ON1CHAT_CLOSE,
	// 상대방(B):DBIndex(4)

	ANSWER_1ON1CHAT_CLOSE,
	// 요청자(A): DBIndex(4)

	// ------------------------------------------


	
	// 채널 변경  -------------------------------------
	
	REQUEST_CHANGE_CHANNEL,
	
	
	ANSWER_CHANGE_CHANNEL,
	 
	
	// ------------------------------------------------------



	// 패스워드 요청 (로비에 있는 사람이 비공개 방의 방장에게 룸에 접속을 요청함) -------

	REQUEST_ROOMPASSWORDA,
	// roomSerial(1)

	ANSWER_ROOMPASSWORDB,
	// 신청자 : 신청자DBIndex(4) - 서버가 방장에게 보낸다.

	REQUEST_ROOMPASSWORD_RESULTB,
	// result(1), 신청자DBIndex(4) - 방장이 보낸다.

	ANSWER_ROOMPASSWORD_RESULTA,
	// result(1), [ roomSerial(1), pwdLen(1), pwd(text) ]

	// -----------------------------------------------------------------------



	// 룸을 만들기    --------------------------------------------------
	
	REQUEST_CREATEROOM,
	// type(1), titleLen(1), title(text), passwordLen(1), password(text)

	ANSWER_CREATEROOM,
	// result(1), roomSerial(1), level(1), BaseBettingMoney(4)
	
	// -------------------------------------------------------------------



	// 룸에 참가 -----------------------------------------------
	
	REQUEST_JOINROOM,
	// roomSerial(1), pwdLen(1), pwd(text)

	ANSWER_JOINROOM,
	// result(1), roomSerial(1), level(1), BaseBettingMoney(4), 
	// 요청자를 포함한 모든 유저수(1),[ DBIndex(4),순서(1), 돈(INT64), 아바타레이어(32),아바타타입(1) ]
	/// : 요청자를 포함한 모든 유저의 정보 중 아바타 정보는 본인것은 주지 않는다.

	// 유저가 룸의 참가하면 룸의 다른 유저에게 통보한다.
	ANSWER_NOTIFY_USERJOINROOM,
	// DBIndex(4), 순서(1), 돈(INT64), 아바타레이어(32),아바타타입(1)

	// ----------------------------------------------------------


	


	
	// 룸에서 로비로 나감 --------------------------

	REQUEST_EXITROOM,  
	// 방을 나가는 사유 (1) : 클라이언트는 무조건 '0'을 줘야 된다.

	ANSWER_EXITROOM,
	// 방을 나가는 사유 (1)
	// 0 : 일반적인, 1 : 돈이 없었어


	// 방이 삭제 되었을 때 받는 메세지
	ANSWER_NOTIFY_ROOMDELETE,
	// 방 번호(1)
	

	// 유저가 룸의 떠나면 룸의 다른 유저에게 통보한다.
	ANSWER_NOTIFY_USEREXITROOM,
	// DBIndex(4), 순서(1)

	// -------------------------------------------------


	// 유저가 나가기 예약을 했음을 알림.
	ANSWER_NOTIFY_RESERVATE_EXITROOM,
	// DBindex(4), 상태(1) : 0 - 나가기 예약 취소, 1 - 나가기 예약 




	// 방 이름 변경
	REQUEST_CHANGE_ROOMTITLE,
	// 방번호(1), 방 이름 문자수(1), 방이름(text)
	// 방번호는 무조건 0 이다.

	ANSWER_NOTIFY_CHANGEROOMTITLE,
	// 방 번호(1), 방 이름 문자수(1), 방 이름(text)



	// 방 공개/비 공개 변경, 패스워드 변경
	REQUEST_CHANGE_ROOMTYPE,
	// 방 타입(1), 패스워드 문자수(1), 패스워드(text)
	// 방 타입 : 0(공계), 1(비 공개) - 비 공개 일때만 패스 워드 변경.      

	ANSWER_NOTIFY_CHANGEROOMTYPE,
	// 방 번호(1), 방 타입(1)


	

	// 룸에서의 채팅 ---------------------------------------
	
	REQUEST_CHATINROOM,
	// 보낸 사람:DBIndex(4), 메세지 길이(1), 메세지(text)

	ANSWER_CHATINROOM,
	// 보낸 사람:DBIndex(4), 메세지 길이(1), 메세지(text)

	// ---------------------------------------------------



	// 방에 변경이 있을 때 채널에 있는 유저가 받는 메세지(유저가 들어가고 나올 때)
	ANSWER_NOTIFY_ROOMSETINFO,
	// 방장이 나간 경우 : 방 번호(1), 유저수(1), DBIndex(4) - DBIndex는 새로운 방장의 것.
	// 일반 유저가 들어가던지 나올 때 : 방번호(1), 유저수(1), 0(4)

	
	
	// 방의 상태를 통보
	ANSWER_NOTIFYROOMSTATE,
	// 방 번호(1), 방의 상태(1) - GAME_OFF(대기), GAME_ING(게임 중)



	// 룸에서의 초대 -----------------------------------------

	REQUEST_INVITEA,
	// 초청자 : DBIndex(4) 

	ANSWER_INVITEB,
	// DB인덱스(4), channelNum(1), roomSerial(1),
	// 패스워드길이(1), [ 패스워드(text) ]

	REQUEST_INVITE_RESULTB,
	// result(1), DB인덱스(4), channelNum(1), roomSerial(1)

	ANSWER_INVITE_RESULTA,
	// result(1)
	
	// -----------------------------------------------------------



	// 채널에 있는 유저가 룸에서 채널, 채널에서 룸으로 이동했을 때 채널의 타 유저가 받는 메세지
	ANSWER_NOTIFY_USERSETINFO, 
	// DBIndex(4), 위치(1), [ 방번호(1) ]
	

	// 채널에서 유저가 나간 경우 채널의 타 유저들에게도 통보
	ANSWER_NOTIFY_USERDELETE,
	// DBIndex(4)
	
	
	// 유저의 아이템 정보를 수정하기를 요청.
	REQUEST_RELOADUSERITEM,


	// 서버가 유저에게 새로운 아이템 정보를 보내준다.
	ANSWER_RELOADUSERITEM,
	// 비 공개방 생성 아이템(1), 점프 아이템(1) - 아이템의 사용 가능 여부를 알려준다.TRUE, FALSE


	// 유저가 나가기 예약을 한 경우 게임을 끝나면 유저에게 보낼 패킷
	ANSWER_RESERVATE_EXITROOM,
};


//   <<<< 게임 패킷 정의 >>>>
enum {

	// 게임 시작 통보 
	ANSWER_GAMESTART = 101,
	

	
	// 유저가 배팅 금액 및 선택 위치를 알려준다.
	REQUEST_BEGINGAME,
	// 배팅단계(1) : 1 - X1, 2 - X2, 3 - X3, 4 - X4, 5 - X5
	// 위치(1)     - 유저의 배팅 위치.   0 - 뱅크, 1 - 플레이어, 2 - 타이

	ANSWER_BEGINGAME,
	// 사람수(1), [ DBIndex(4), 배팅금액(4), 위치(1) ] - 모든 유저의 선택을 모든 유저에게 통보한다.
	// 일정 시간내에 응답이 없는 유저는 서버가 임의로 정해서 통보 한다.

	
	// 뱅커와 플레이어의 카드를 보낸다.
	ANSWER_STARTCARDS,
	// 뱅커(2), 뱅커의 점수(1), 플레이어(2), 플레이어의 점수(1).    뱅커, 플레이어 각각 카드 2장씩 받는다.




	// 3번째 카드를 받을 수 있다면 보낸다.
	ANSWER_SENDTHREECARD,
	// 플레이어(1), 뱅크(1).    카드 번호 52라면 무시한다.


	
	// 게임 결과 통보
	ANSWER_RESULT,
	// 이긴쪽(1), 뱅크 점수(1), 플레이어점수(1), 사람수(1),[ DBIndex(4),금액(4) ], 
	// 금액은 유저들이 받거나 차감될 금액을 말한다.



	// 게임 중지 통보
	ANSWER_GAMEOFF,



	// 마지막 패킷 정의 ( 서버에서 패킷 처리를 위해 이용 ).게임에서는 사용 안함
	FINAL_PACKETDEFINE

};




//  Server To client - reuslt(1)  -----------------------------------------------

// 로그인시의 결과 상세 메세지
enum
{
	LOGIN_SUCCESS,			//	S ->	로그인 성공
	LOGFAIL_NOID,			//	S ->	로그인 실패 : ID 미등록
	LOGFAIL_DUPLEID,		//	S ->	로그인 실패 : ID 중복
	LOGFAIL_STOPID,			//	S ->	로그인 실패 : 정지 ID
	LOGFAIL_PASSWORD,		//	S ->	로그인 실패 : 패스워드 틀림
	LOGFAIL_NOMONEY,		//	S ->	로그인 실패 : 소지금 없음
	LOGFAIL_NOVERSION,      //	S ->	로그인 실패 : 클라이언트 버전이 틀림
	LOGFAIL_NOLEVEL,        //  S ->    로그인 실패 : 유저의 레벨이 틀림.
	LOGFAIL_DBERROR			//  S ->	DB 에러. 데이타베이스에 문제 있음
};


// 로그 아웃 결과 표시
enum
{
	LOGOUT_NORMAL,			// 일반적인 클라이언트의 요청에 의한 로그 아웃.
	LOGOUT_NOTMONEY,		// 돈이 없어 서버의 요처으로 로그 아웃.
};


// 유저 검색 요청 결과 상세 메세지
enum
{
	FIND_SUCCESS,			// 검색 성공
	FIND_FAIL				// 검색 실패
};


// 채널 입장 요청 결과 상세 메세지
enum
{
	JOINCHANNEL_SUCCESS,		// 채널 입장 성공
	JOINCHANNEL_FAIL_FULL,		// 채널에 유저가 만땅임.
	JOINCHANNEL_FAIL_NOTLEVEL	// 유저가 들어 갈수 없는 채널임
};


// 패스워드 요청에 대한 결과 상세 메세지
enum
{
	ROOMPASSWORD_SUCCESS,				// 게임 참가 요청 성공
	ROOMPASSWORD_FAIL_NOINJECT,			// 게임 참가 거부
	ROOMPASSWORD_FAIL_NOROOM,			// 방이 없음
	ROOMPASSWORD_FAIL_FULLUSER			// 방의 최대 인원을 초가
};


// 1:1 대화 요청에 대한 결과 상세 메세지
enum
{
	ONECHAT_SUCCESS,					// 1:1 대화 승락
	ONECHAT_FAIL_ALLREJECT,				// 모든 1:1 대화를 거부하고 있다.
	ONECHAT_FAIL_REJECT,				// 1:1 대화 거부
	ONECHAT_FAIL_NOTUSER,				// 로비에 상대방이 없다.
};


// 룸 참가 요청 결과 상세 메세지
enum
{

	ROOM_SUCCESS_JOINROOM,		// 룸에 접속 성공
	ROOM_FAIL_MAXUSER,			// 룸에 최대인원 초과
	ROOM_FAIL_PWD,				// 비공개 방의 암호 틀림
	ROOM_FAIL_EXIST,			// 존재하지 않는 방
};


// 룸 만들기 요청 결과 상세 메세지
enum
{
	CREATEROOM_SUCCESS,			// 룸 생성 성공 
	CREATEROOM_FAIL_MAXROOM,	// 룸 생성 실패 - 채널의 최대 룸 갯수 보다 많음
};


// 룸에서의 게임 초대 요청 결과 상세 메세지
enum
{
	INVITE_SUCCESS,
	INVITE_FAIL_ALLREJECT,		// 상대방이 모든 초대를 거부하고 있음
	INVITE_FAIL_REJECT,			// 상대방이 초대에 대해 거부
	INVITE_FAIL_NOTUSER,		// 상대방이 로비에 없다.
};


// 방의 타입( 공개, 비공개 )
enum
{
	PUBLICTYPE, 
	PRIVATETYPE,
};


// 방을 나가는 이유
enum
{
	EXITROOM_NORMAL = 0,
	EXITROOM_NOTMONEY
};


// 방과 유저의 현재 상태
enum
{
	GAME_CLOSE,				// 접속하지 않은 유저를 뜻 한다.
	GAME_OFF,
	GAME_READY,
	GAME_WAIT,				//  이상태 부터 룸에 접근 거부.
	GAME_ING,				// 방의 상태는 GAME_OFF -> GAME_READY -> GAME_WAIT -> GAME_ING  순으로 변화한다.
	GAME_SCORECALCULATE,    // 방에서 점수 계산 중인 경우의 상태.
	GAME_AGENCY,			// 클라이언트가 게임 도중 비 정상 종료일 경우를 뜻한다.
	GAME_NOTMONEY			// 돈이 없음.
};


// 유저의 위치
enum
{
	WH_CHANNELSELECT = 0,
	WH_LOBBY,
	WH_ROOM,
	WH_NONE					// 클라이언트에서 관리 목적으로 사용
};


#endif

// 최종 변경일 : 2003년 4월 8일