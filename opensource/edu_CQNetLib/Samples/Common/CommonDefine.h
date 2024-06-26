#pragma once

// C#에서 사용할 상수 ///////////////////////////////////////////
//const INT32 MAX_READ_SIZE = 32048; //64096; // 읽을 수 있는 최대 크기
//const INT32 MAX_PACKET_DATA = 8096;	    // 패킷 하나의 최대 크기

// 패킷 구조
//const INT32 SZ_HEADER = 4;				// 헤더 사이즈.
//const INT32 SZ_HEADER_AT_BODY = 2;		// 헤더에서 바디의 크기 사이즈.
//const INT32 SZ_HEADER_AT_TYPE = 2;		// 헤더에서 타입의 사이즈.
//const INT32 BODYSIZE_HEADER_AT_POS = 0;// 헤더에서 패킷 몸체의 크기가 있는 위치
//const INT32 TYPE_HEADER_AT_POS = 2;    // 헤더에서 패킷 몸체의 크기가 있는 위치
//const INT32 MAX_PACKET_NUM = 2000;		// 패킷의 마지막 번호
///////////////////////////////////////////////////////////////////



// 모든 결과 성공
const char RESULT_OK = 0;
// 모든 결과에 대한 실패
const char RESULT_FAIL = 1;
// 서버와 DB의 연결 되지 않았음
const char RESULT_NOT_DB_CONNECT = 31;


// 문자열 길이  + 1은 널스트링을 위해서
const INT32 MAX_USERID = 13;
const INT32 MAX_USERPW = 13;
//const INT32 MAX_USERNAME = 13;
//const INT32 MAX_JUMIN_NUMBER_LEN = 14;			// 주민등록번호 최대 수.
//const INT32 MAX_NICKNAME = 21;			// 닉네임
const INT32 LEN_SECURE_STR = 33;			// MD5 인코딩 후 출력 문자열의 길이.
//const INT32 MAX_CONNSERVER = 4;			// 최대접속서버개수
//const INT32 MAX_ODBCNAME = 30;			// ODBC에 사용되는 이름
//const INT32 MAX_CHANNEL_NAME_LEN = 65;			// 채널 이름의 길이
const INT32 MAX_SERVER_NAME_LEN = 33;			// 서버의 이름
//const INT32 MAX_ROOM_NAME_LEN = 41;  			// 방 이름
//const INT32 MAX_ROOM_PASSWORD_LEN = 9;  			// 방 암호
//const INT32 MAX_ROOM_CHAT_USER_COUNT = 100;		// 채팅 방의 최대 수용 인원
//const INT32 MAX_ROOM_GAME_USER_COUNT = 6;		// 게임 방의 최대 수용 인원
//const INT32 MAX_ITEM_NAME_LEN = 31;			// 아이템 이름 길이
//const INT32 MAX_SUBJECTIVE_QUESTION_LEN = 201;	// 주관식 문제의 길이.
//const INT32 MAX_SUBJECTIVE_QUESTION_ANSWER_LEN = 21;	// 주관식 문제 답변의 길이.
//const INT32 MAX_LOBBY_CHAT_LEN = 81;			// 로비 채팅 문자 최대 길이
//const INT32 MAX_ROOM_CHAT_LEN = 81;			// 방 채팅 문자 최대 길이
//const INT32 MAX_LOBBY_NOTE_LEN = 401;			// 로비 쪽지 길이
//const INT32 MAX_NOTICE_LEN = 201;		// 공지의 길이
const INT32 MAX_FILENAME = 151;			// 파일 이름
//const INT32 MAX_IP_LEN = 17;			// IP의 길이
//const INT32 MAX_SERVER_EXEC_DIRECTORY_LEN = 129;	// 서버의 실행파일이 있는 풀 디렉토리
//const INT32 MAX_SERVER_FILE_NAME_LEN = 65;	// 서버 실행 파일 이름


// 유저의 접속 상태
const char CONN_STATE_OFFLINE = 0;	// 오프라인 상태
const char CONN_STATE_ONLINE = 1;	// 온라인 상태
const char CONN_STATE_LOGIN_AUTH = 2;	// 로그인 서버 인증 완료
const char CONN_STATE_LOGIN_AUTH_DUPLICATION = 3;	// 로그인 서버 중복 접속 상태
//const char CONN_STATE_GMAE_AUTH = 4;	// 게임서버 인증 완료

// 게임서버에서 유저가 있는 위치
//const char GAMESERVER_POSITION_NONE = 0;
//const char GAMESERVER_POSITION_LOBBY = 1; // 로비
//const char GAMESERVER_POSITION_ROOM = 2; // 룸

// 캐릭터 아바타 착용 아이템의 위치
//const INT32 WEAR_POS_CAP = 1;
//const INT32 WEAR_POS_HEAD = 2;
//const INT32 WEAR_POS_FACE = 3;
//const INT32 WEAR_POS_COAT = 4;
//const INT32 WEAR_POS_ARM = 5;
//const INT32 WEAR_POS_HAND = 6;
//const INT32 WEAR_POS_PANT = 7;
//const INT32 WEAR_POS_LEG = 8;
//const INT32 WEAR_POS_SHOE = 9;
//const INT32 WEAR_POS_SET = 10;
//const INT32 WEAR_POS_EYE = 11;
//const INT32 WEAR_POS_BACK = 12;
//const INT32 WEAR_POS_CARRIAGE = 13;
//const INT32 WEAR_POS_PET = 14;
//const INT32 WEAR_POS_EYE2 = 15;	// 눈위에 붙는 것. 안경 등
//const INT32 WEAR_POS_EAR = 16; // 귀
//const INT32 WEAR_POS_RIGHT_HAND = 17; // 오른 손
//const INT32 WEAR_POS_LEFT_HAND = 18; // 왼손
//const INT32 WEAR_POS_WAIST = 19; // 허리
//const INT32 WEAR_POS_END = 20;


// 아이템 구분
//const char ITEM_KIND_WEAR = 1;
//const char ITEM_KIND_GAME = 2;
//const char ITEM_KIND_STAGE = 3;
//const char ITEM_KIND_ARRANGEMENT = 4;

// 성별
//const char SEX_MAN = 1;
//const char SEX_WOMAN = 2;
//const char SEX_NONE = 3; // 성별 무시


// 나이
//const char AGE_0_10 = 0;
//const char AGE_11_15 = 1;
//const char AGE_16_19 = 2;
//const char AGE_20_23 = 3;
//const char AGE_24_28 = 4;
//const char AGE_29_32 = 5;
//const char AGE_33_36 = 6;
//const char AGE_37_40 = 7;
//const char AGE_41_44 = 8;
//const char AGE_45_48 = 9;
//const char AGE_49_52 = 10;
//const char AGE_53_56 = 11;
//const char AGE_57_60 = 12;
//const char AGE_61_64 = 13;
//const char AGE_65_68 = 14;
//const char AGE_69_72 = 15;
//const char AGE_73_300 = 16;
//
//
//// 지역
//const char DISTRICT_SEOUL = 0; // 서울
//const char DISTRICT_KYEONGGI = 1; // 경기	
//const char DISTRICT_INCHON = 2; // 인천
//const char DISTRICT_CHUNGNAM = 3; // 충남
//const char DISTRICT_DAEJON = 4; // 대전
//const char DISTRICT_CHUNGBUK = 5; // 충북
//const char DISTRICT_KANGWON = 6; // 강원
//const char DISTRICT_KYEONGBUK = 7; // 경북
//const char DISTRICT_KYEONGNAM = 8; // 경남
//const char DISTRICT_DAEGU = 9; // 대구
//const char DISTRICT_ULSAN = 10; // 울산
//const char DISTRICT_BUSAN = 11; // 부산
//const char DISTRICT_JEONBUK = 12; // 전북
//const char DISTRICT_JEONNAM = 13; // 전남
//const char DISTRICT_GWANGJU = 14; // 광주
//const char DISTRICT_JEJU = 15; // 제주
//const char DISTRICT_FOREIGN = 16; // 해외

// 혈액형
//const char BLOOD_TYPE_A = 0;
//const char BLOOD_TYPE_B = 1;
//const char BLOOD_TYPE_AB = 2;
//const char BLOOD_TYPE_O = 3;

// 취미
//const char INTEREST_READING = 0;
//const char INTEREST_MOVIE = 1;
//const char INTEREST_FISHING = 2;
//const char INTEREST_GAME = 3;
//const char INTEREST_SPORTS = 4;
//const char INTEREST_BADUK = 5;
//const char INTEREST_COLLECT = 6;
//const char INTEREST_TRAVEL = 7;

// 주관식 문제 종류
//const char SUBJECTIVE_KIND_LOVE = 0; // 연애
//const char SUBJECTIVE_KIND_STAR = 1; // 스타
//const char SUBJECTIVE_KIND_COMMONSENSE = 2; // 상식
//const char SUBJECTIVE_KIND_FAMILY = 3; // 가족
//const char SUBJECTIVE_KIND_DESIRE = 4; // 희망
//const char SUBJECTIVE_KIND_ETC = 5; // 기타
//const char SUBJECTIVE_KIND_COUNT = 6; // 퀴즈 종류 개수

// 방의 모드
//const char ROOM_MODE_CHAT = 1; // 채팅
//const char ROOM_MODE_JUBJECTIVE_QUIZ_GAME = 2; // 주관식 퀴즈

// 퀴즈 방의 최대 맵 개수
//const INT32 MAX_QUIZ_ROOM_MAP_COUNT = 4;
// 채팅 방의 최대 맵 개수
//const INT32 MAX_CHAT_ROOM_MAP_COUNT = 3;

// 방의 인원
//const INT32 ROOM_QUIZ_USER_COUNT_SEL_INDEX_COUNT = 5;
//const char ROOM_QUIZ_USER_COUNT[ ROOM_QUIZ_USER_COUNT_SEL_INDEX_COUNT ] = { 2, 3, 4, 5, 6 };
//const char MAX_ROOM_QUIZ_USER_COUNT = 6; // 위의 ROOM_QUIZ_USER_COUNT의 최대 값과 일치해야 된다.
//const INT32 ROOM_CHATTING_USER_COUNT_SEL_INDEX_COUNT = 8;
//const char ROOM_CHATTING_USER_COUNT[ ROOM_CHATTING_USER_COUNT_SEL_INDEX_COUNT ] = { 30, 40, 50, 60, 70, 80, 90, 100 };

// 방의 상태
//const char ROOM_STATE_WAIT = 1; // 대기 상태.
//const char ROOM_STATE_GAME_ING = 2; // 시작 상태.
//const char ROOM_STATE_GAME_RESULT = 3; // 시작 상태.

// 주관식 문제의 정답 순위에 따른 점수
//const INT32 SUBJECTIVE_QUIZ_ANSWER_COUNT = 10;
//const char SUBJECTIVE_QUIZ_ANSWER_POINT[ SUBJECTIVE_QUIZ_ANSWER_COUNT + 2 ] = { 0, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };

// 주관식 문제의 한 라운드당 최대 오답 횟수
//const INT32 SUBJECTIVE_QUIZ_ERROR_ANSWER_COUNT_OF_ROUND = 3;

// 정답 축출 종류
//const INT32 SUBJECTIVE_QUIZ_ANSWER_KIND_ASC = 0; // 정답 범위는 내림 차순
//const INT32 SUBJECTIVE_QUIZ_ANSWER_KIND_DESC = 1; // 정답 범위는 오름 차순	
//const INT32 SUBJECTIVE_QUIZ_ANSWER_KIND_RAND = 2; // 정답 범위는 무작위

// 하나의 주관식 문제당 가지고 있는 답의 개수
//const INT32 SUBJECTIVE_QUIZ_ANSWER_TOTAL_COUNT = 10;

// 한 라운드의 최대 시간(초)
//const UINT32 MAX_GAME_ROUND_TIME = 120;	// 2분

// 문제 답 설문 시 표시 항목 수
//const INT32 QUESTION_RESEARCH_ANSWER_COUNT = 10;

// 설문 대기 시간(초)
//const UINT32 QUESTION_RESEARCH_ANSWER_WAIT_TIME = 15;	

// 게임 종료 후 커플 승낙 대기 시간
//const UINT32 MAX_GAME_COUPLE_CONSENT_TIME = 15;

// 메신저 친구 최대 수
//const INT32 MAX_MSG_FRIEND_COUNT = 50;

// 메신저 친구 상태
//const char FRIEND_STATE_WAIT = 1; // 친구 수락 대기중(추가한 사람)
//const char FRIEND_STATE_WAIT_ACCEPT = 2; // 친구 수락 대기중(추가된 사람)
//const char FRIEND_STATE_COMPLETE_OK = 3; // 친구 완료 - 승낙
//const char FRIEND_STATE_COMPLETE_NO = 4; // 친구 완료 - 반대
//const char FRIEND_STATE_DELETED = 5; // 친구 삭제 되었음(상대방이 삭제한 경우) 



// 관리자 레벨 등급.
//const char ADMIN_LEVEL_1 = 1;
//const char ADMIN_LEVEL_2 = 2; // 캐릭터의 정보 변경 가능.
//const char ADMIN_LEVEL_3 = 3;
//const char ADMIN_LEVEL_CHEAT = 5; // 치트 : 관리자 레벨
//const char ADMIN_LEVEL_DEV = 7;  // 개발 : 관리자 레벨


// 관리자 기능 사용 가능 레벨
//const char ADMIN_LEVEL_GAME_DATA_RELODE = 7;


// 시간 Tick
const INT32 CHECKTIME_SERVERTICK = 250;	// 서버에서 틱 쓰레드의 틱 설정(1/250초)
const INT32 CHECKTIME_CUR_DATE = 7200;	// 1시간 마다 현재 시간을 설정한다.
const INT32 CHECKTIME_UPDATEGAMEROOM = 1; // 게임 룸을 업데이트한다.
const INT32 CHECKTIME_LOGIN = ( 1000 / CHECKTIME_SERVERTICK )	* 60;// 로긴 유지 시간 - 로긴 인증이 이 시간이 지나도록 안되면 접속 끊는다.



// 최대 개수	
const INT32 MAX_PROCESSFUNC = 2000;		//패킷 처리를 위한 함수 포인터
const INT32 MAX_CHANNEL_COUNT = 10;		// 최대 채널 수
//const INT32 MAX_RESERVED_CHANNEL_USER_COUNT = 10;		// 각 채널당 예약된 유저 수(유저 인증 시의 딜레이 때문에 빈 공간 예약)
//const INT32 MAX_ITEM_COUNT = 10000;	// 최대 아이템 개수.
//const short MAX_FIRST_WEAR_CHARITEM_COUNT = 50;		// 최초 착용 캐릭터 아이템 소유 가능 개수.
//const short MAX_LAST_WEAR_CHARITEM_COUNT = 100;		// 최대 착용 캐릭터 아이템 소유 가능 개수.
//const short MAX_FIRST_GAME_CHARITEM_COUNT = 50;		// 최초 게임 캐릭터 아이템 소유 가능 개수.
//const short MAX_LAST_GAME_CHARITEM_COUNT = 100;		// 최대 게임 캐릭터 아이템 소유 가능 개수.
//const short MAX_FIRST_STAGE_CHARITEM_COUNT = 10;		// 최초 배경 캐릭터 아이템 소유 가능 개수.
//const short MAX_LAST_STAGE_CHARITEM_COUNT = 20;		// 최대 배경 캐릭터 아이템 소유 가능 개수.
//const short MAX_FIRST_ARRANGE_CHARITEM_COUNT = 40;		// 최초 배치 캐릭터 아이템 소유 가능 개수.
//const short MAX_LAST_ARRANGE_CHARITEM_COUNT = 80;		// 최대 배치 캐릭터 아이템 소유 가능 개수.
//const INT32 MAX_LOBBY_USER_LIST_COUNT_PAGE = 128;		// 로비의 유저 리스트 전송시 한번에 보낼 수 있는 최대 개수
//const INT32 MAX_LOBBY_ROOM_LIST_COUNT_PAGE = 128;		// 로비의 방 리스트 전송시 한번에 보낼 수 있는 최대 개수
//const char MAX_GAME_ROUND_COUNT = 5;		// 게임 라운드 수.
//const INT32 MAX_QUIZ_COUNT = 10000;	// 최대 퀴즈 개수.
//const INT32 MAX_SKILL_COUNT = 3000;		// 스킬 최대 개수.
//const INT32 MAX_CHAR_SKILL_SLOT_COUNT = 12;		// 캐릭터의 최대 스킬 슬롯 개수.
//#define MAX_UDPPACKET							100			//최대 UDP패킷 수


												
//const char MIN_GAME_ROUND_COUNT = 3;		// 게임 라운드 수.
//
////사이즈
//#define SIZE_CHARACTER_STAGE						MAX_CHARACTER_STAGE
//#define SIZE_CHARACTER_PARTS_ITEMKEY				sizeof( UINT64 ) * MAX_CHARACTER_PARTS 
//#define SIZE_CHARACTER_INSTRUMENTSLOT_ITEMKEY		sizeof( UINT64 ) * MAX_CHARACTER_INSTRUMENTSLOT 
//#define SIZE_CHARACTER_PARTS_ITEMCODE				sizeof( INT32 ) * MAX_CHARACTER_PARTS
//#define SIZE_CHARACTER_INSTRUMENTSLOT_ITEMCODE		sizeof( INT32 ) * MAX_CHARACTER_INSTRUMENTSLOT
//#define SIZE_USER_TREND								sizeof( DWORD ) * TREND_COUNT
//
//
//// 확장 값 
//#define EXT_SCORE						100		//다른 클라이언트들이 보낸 스코어의 평균과 내가 보낸 스코어와의 차이가 이거 이상이면 평균으로 대체한다.
//
//// 함수
//#define GET_ITEMTYPE_CODE( nItemCode ) ( nItemCode / 10000000 )		//아이템 코드에서 타입을 읽어온다.
//#define GET_CATEGORY1_CODE( nItemCode ) ( ( nItemCode / 1000000 ) % 10 )	//아이템 코드에서 카테고리를 가져온다.
//#define GET_CATEGORY2_CODE( nItemCode ) ( ( nItemCode / 100000 ) % 10 )	//아이템 코드에서 카테고리를 가져온다.
//
//


//유저 로그인 상태
//enum eUserLoginState
//{
//	USER_LOGINSTATE_LOGOUT = 0,				// 로그아웃
//	USER_LOGINSTATE_LOGIN = 1,				// 로그인
//	USER_LOGINSTATE_CHANGESERVER = 2,				// 서버 변경중 
//
//	USER_LOGINSTATE_COUNT,
//};

//게임이 가질 수 있는 상태
//enum eGameState
//{
//	GAMESTATE_NONE = 0,				//아무 상태도 아니다.
//	GAMESTATE_PLAY = 1,				//게임 플레이 상태
//	GAMESTATE_END = 2,				//개암 끝난상태
//};



//enum eChangeGamePlayState
//{
//	CROOM_PLAYSTATE_REPAY = 0,			//게임 플레이 상태를 리페이로 바꾼다.
//
//};



// 인벤토리 카테고리
//enum eInventoryCategory
//{
//	INVENTORYCATEGORY_NONE = 0,			// 없음
//	INVENTORYCATEGORY_SHIRT = 1,			// 상의
//	INVENTORYCATEGORY_PANTS = 2,			// 하의
//	INVENTORYCATEGORY_ACCESSORY = 3,			// 악세사리
//	INVENTORYCATEGORY_INSTRUMENT = 4,			// 악기
//	INVENTORYCATEGORY_GAMEITEM = 5,			// 아이템
//
//	INVENTORYCATEGORY_COUNT,
//};

// 인벤토리 카테고리
//enum eShopCategory
//{
//	SHOPCATEGORY_NONE = 0,			// 없음
//	SHOPCATEGORY_SHIRT = 1,			// 상의
//	SHOPCATEGORY_PANTS = 2,			// 하의
//	SHOPCATEGORY_ACCESSORY = 3,			// 악세사리
//	SHOPCATEGORY_INSTRUMENT = 4,			// 악기
//	SHOPCATEGORY_GAMEITEM = 5,			// 아이템
//
//	SHOPCATEGORY_COUNT,
//};


// 악세사리 타입
//enum eAccessoryType
//{
//	ACCESSORYTYPE_HAIR = 0,			// 헤어
//	ACCESSORYTYPE_ACCESSORIES = 1,			// 장신구
//	ACCESSORYTYPE_DECORATOR = 2,			// 꾸미기
//	ACCESSORYTYPE_EFFECT = 3,			// 이팩트
//	ACCESSORYTYPE_SET = 4,			// 셋트
//
//	ACCESSORYTYPE_COUNT,
//};


// 게임 아이템 타입
//enum eItemType
//{
//	ITEMTYPE_CHARACTER = 1,			// 캐릭터 아이템
//	ITEMTYPE_CONSUMPTION = 2,			// 소모성 아이템
//	ITEMTYPE_PLAY = 3,			// 플래이 아이템
//	ITEMTYPE_PACKAGE = 4,			// 패키지 아이템
//	ITEMTYPE_MUSIC = 5,			// 음악
//	ITEMTYPE_ETC = 6,			// 기타
//	ITEMTYPE_BACKGROUND = 7,			// 배경
//	ITEMTYPE_COMMUNITY = 8,			// 커뮤니티 아이템
//	ITEMTYPE_PREMIUM = 9,			// 프리미엄 아이템
//
//	ITEMTYPE_COUNT
//};


// 캐릭터 아이템 어빌리티 타입
//enum eCharItemAbililty
//{
//	CHAR_ITEMABILITY_NONE = 0,			// 없음
//	CHAR_ITEMABILITY_GOUP_POINT = 1,			// 포인트 획득량 상승
//	CHAR_ITEMABILITY_INCREASE_DECISION_RANGE = 2,			// 판정 범위 확대
//	CHAR_ITEMABILITY_DECREASE_GROOVE_GAUGE = 3,			// 그루브 게이지 감소량 완충
//	CHAR_ITEMABILITY_USE_EFFECT = 4,			// 이펙트 효과 사용
//	CHAR_ITEMABILTTY_NOTE_IMAGE = 5,			// 노트 이미지 
//	CHAR_ITEMABILITY_COUNT
//};

// 소모성 아이템 어빌리티 타입
//enum eConsItemAbility
//{
//	CONS_ITEMABILITY_NONE = 0,			// 없음
//	CONS_ITEMABILITY_CHANGE_NICKNAME = 1,			// 닉네임 변경
//	CONS_ITEMABILITY_RESET_PLAYINFO = 2,			// 플레이 정보 초기화
//	CONS_ITEMABILITY_CASH_COUPON = 3,			// 캐쉬 상품권
//	CONS_ITEMABILITY_EXERGAME_COUPON = 4,			// 연습 게임 쿠폰
//	CONS_ITEMABILITY_RECORD_COUPON = 5,			// 리플레이 녹화 쿠폰
//
//	CONS_ITEMABILITY_COUNT
//};


// 판매 타입
//enum eSellType
//{
//	SELLTYPE_NONE = 0,			//파는 타입 아님
//	SELLTYPE_CASH = 1,			// 캐쉬 아이템
//	SELLTYPE_POINT = 2,			// 포인트 아이템
//	SELLTYPE_RENTAL = 3,			// 렌탈 아이템 
//
//	SELLTYPE_COUNT
//};

// 판매 아이템 상태
//enum eSellItemState
//{
//	SELLITEMSTATE_NONE = 0,			// 상태 없음
//	SELLITEMSTATE_TEST = 1,			// 테스트 	(관리자만 사용)
//	SELLITEMSTATE_HIT = 2,			// 히트
//	SELLITEMSTATE_HOT = 3,			// 핫
//	SELLITEMSTATE_NEW = 4,			// 뉴
//	SELLITEMSTATE_NORMAL = 5,			// 노멀
//
//	SELLITEMSTATE_COUNT
//};

// 판매 아이템 렌탈 기간
//enum eRentalPeriod
//{
//	RENTALPERIOD_NONE = 0,
//	RENTALPERIOD_DAY = 1,			// 하루
//	RENTALPERIOD_WEEK = 2,			// 일주일
//	RENTALPERIOD_MONTH = 3,			// 한달
//	RENTALPERIOD_THREEMONTH = 4,			// 3달
//	RENTALPERIOD_YEAR = 5,			// 1년
//
//	RENTALPERIOD_COUNT
//};


//서버타입
//enum eServerType
//{
//	ST_LOGINSERVER = 1,					//로긴서버
//	ST_GAMESERVER = 2,					//게임서버
//	ST_COMMUNITYSERVER = 3,					//커뮤니티서버
//	ST_LOGINDBSERVER = 4,					//로긴디비서버
//	ST_GAMEDBSERVER = 5,					//게임디비서버
//	ST_RELAYSERVER = 6,					//릴레이서버
//	ST_MANAGESERVER = 7,					//관리서버
//
//	ST_COUNT
//};

//서버연결상태
//enum eServerConnStatus
//{
//	SCS_NONE = 0,
//	SCS_ERROR = 1,					//연결에러
//	SCS_CONNECTED = 2,					//연결중
//	SCS_NOTCONNECTED = 3,					//연결끊김
//};

//서버상태
//enum eServerStatus
//{
//	SS_NONE = 0,
//	SS_START = 1,					//서버시작
//	SS_ERROR = 2,					//서버에러
//	SS_END = 3,					//서버종료
//};

//enum eSaveType
//{
//	ST_LOGOUT_GRACEFUL = 0,					//자연스런 로그아웃
//	ST_LOGOUT_FORCE = 1,					//강제 로그아웃
//	ST_BACKUP = 2,					//데이터 백업
//};


//공유기 종류
//enum eNATType
//{
//	NAT_NONE,						//알수 없는 타입
//	NAT_FULL,
//	NAT_RESTRICTED,
//	NAT_PORTRESTRICTED,
//	NAT_SYMMETRIC,
//};





// 패킷처리를 할 때 에러 처리
#define CHECK_START		char __cResult=0;
#define CHECK_ERROR(f)	__cResult=f; goto CHECK_ERR;




