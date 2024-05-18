/*-------------------------------------------------------------------------------------------------*/
//	File			:	GameStruct.h
//  Date			:	08.02.11
//  Author			:	최흥배
//	Description		:	Struct 관련
/*-------------------------------------------------------------------------------------------------*/


#pragma once
struct P2P_PACKET_BASE
{
	unsigned short usLength;
	unsigned short usType;
};

//Peer To Peer 기본 헤더
struct PTPPACKET_BASE : public P2P_PACKET_BASE
{
	DWORD		   dwSequenceNo;		//UDP패킷의 순서 보장
	DWORD		   dwPKey;				//패킷을 보낸 유저의 PKey
	DWORD		   dwOtherPKey;			//패킷을 받을 유저의 PKey
	
	PTPPACKET_BASE()
	{
		dwSequenceNo = 0;
		dwPKey = 0;
		dwOtherPKey = 0;
	}
};



/*--------------------------------------------------------------------------------------------------*/
//서버 관리 관련 구조체
/*--------------------------------------------------------------------------------------------------*/

//서버 시작 속성
struct SERVERSTART_ATTRIBUTE
{
	//DWORD			dwServerKey;					//서버키
	eServerType		ServerType;						//서버타입 (키로 사용)
	
	char			szLogFileName[MAX_FILENAME];	//로그파일이름
	
	INT32 				nProcessPacketCnt;
	INT32 				nSendBufCnt;
	INT32 				nRecvBufCnt;
	INT32 				nSendBufSize;
	INT32 				nRecvBufSize;
	
	INT32 				nMaxConnectionCnt;
	INT32 				nMaxServerConnCnt;
	INT32 				nMaxTempUserInfoCnt;
	
	//최대 사용자 수 (커뮤니티서버 사용)
	INT32 				nMaxUserCnt;

	INT32 				nStartClientPort;
	INT32 				nStartServerPort;
	INT32 				nStartUDPPort;

	//최대 UDP  개수 (릴레이서버 사용)
	INT32 				nMaxUDPCnt;

	INT32 				nWorkerThreadCnt;
	INT32 				nProcessThreadCnt;

	//게임 관련 (게임서버 사용)
	INT32 				nMaxGameRoomPageCnt;
	INT32 				nMaxGameRoomPerPageCnt;

	//ODBC 관련
	char			szODBCNickName[MAX_ODBCNAME];
	char			szODBCId[MAX_ODBCNAME];
	char			szODBCPw[MAX_ODBCNAME];
	
	//게임서버 소켓 정보 (로긴서버 사용)
	INT32 				nGS_ProcessPacketCnt;
	INT32 				nGS_SendBufCnt;
	INT32 				nGS_RecvBufCnt;
	INT32 				nGS_SendBufSize;
	INT32 				nGS_RecvBufSize;

	SERVERSTART_ATTRIBUTE()
	{
		Init();
	}
	
	void Init()
	{
		ZeroMemory( this, sizeof( SERVERSTART_ATTRIBUTE ) );
	}
};

//서버 프로세스 연결 속성
struct SERVERCONNECT_ATTRIBUTE
{
	eServerType		ServerType;						
	char			szConnectIP[MAX_IP_LENGTH];
	INT32 				nConnectPort;

	INT32 				nProcessPacketCnt;			
	INT32 				nSendBufCnt;
	INT32 				nRecvBufCnt;
	INT32 				nSendBufSize;
	INT32 				nRecvBufSize;

	SERVERCONNECT_ATTRIBUTE()
	{
		Init();
	}

	void Init()
	{
		ZeroMemory( this, sizeof( SERVERCONNECT_ATTRIBUTE ) );
	}

};


//서버 연결 상태 속성 (서버 관리시 사용)
struct CONNSERVERSTATUS_ATTRIBUTE
{
	eConnectionType			ConnType;						//연결타입
	char					szConnIP[ MAX_IP_LENGTH ];		//연결IP
	INT32 						nConnPort;						//연결포트
	short					sConnCnt;						//연결된소켓개수
 
	CONNSERVERSTATUS_ATTRIBUTE()
	{
		Init();
	}
	
	void Init()
	{
		ZeroMemory( this, sizeof( CONNSERVERSTATUS_ATTRIBUTE ) );
	}
};


//서버 프로세스 상태 속성 (서버 관리시 사용)
struct SERVERSTATUS_ATTRIBUTE
{
	eServerType		ServerType;								//서버타입 (키로사용)
	eServerStatus	ServerStatus;							//서버상태

	INT32 				nUsedCpu;								//CPU 사용량
	INT32 				nUsedRam;								//메모리사용량
	INT32 				nUserCnt;								//동시접속자수(현재접속된서버개수)
	
	CONNSERVERSTATUS_ATTRIBUTE	ConnServerStatusAttr[ MAX_CONNSERVER ];
	
	SERVERSTATUS_ATTRIBUTE()
	{
		Init();
	}
	
	void Init()
	{
		ZeroMemory( this, sizeof( SERVERSTATUS_ATTRIBUTE ) );
	}
};

// 캐릭터 아이템
struct CharItem
{
	void ClearCharItem()
	{
		cIsLocked		= 0;
		i64CharItemCd	= 0;
		i64CharCd		= 0;
		iItemCd			= 0;
		iGetTime		= 0;
		iUseStartTime	= 0;
		iUsedTime		= 0;
	}

	char			cIsLocked;		// 락킹 여부.
	UINT64			i64CharItemCd;	// 캐릭터 아이템 코드
	UINT64			i64CharCd;		// 캐릭터 코드
	INT32 			iItemCd;		// 아이템 코드		
	INT32 			iGetTime;		// 입수 시간(초)
	INT32 			iUseStartTime;	// 사용을 시작한 시간 시간(초)
	INT32			iUsedTime;		// 사용한 시간.(사용하지 않을 때 계산)
};
// 캐릭터 아이템 - 게임
struct CharItemGame : CharItem
{
	void Clear()
	{
		ClearCharItem();

		sCurUseCount = 0;
	}

	short			sCurUseCount;	// 현재 사용 횟수	
};
// 캐릭터 아이템 - 배경
struct CharItemState : CharItem
{
	void Clear()
	{
		ClearCharItem();

		cIsCurUse = 0;
	}

	char			cIsCurUse;		// 현재 사용 여부	
};
// 캐릭터 아이템 - 착용
struct CharItemArrangement : CharItem
{
	void Clear()
	{
		ClearCharItem();

		cIsCurUse	= 0;
		fPosX		= 0;
		fPosY		= 0;
		fPosZ		= 0;
	}

	char			cIsCurUse;		// 현재 사용 여부	
	float			fPosX;			// 위치 X
	float			fPosY;			// 위치 Y
	float			fPosZ;			// 위치 Z
};

// 착용중인 아이템 정보
struct WearCharItem
{
	UINT64	i64CharItemCd;
	short	sInvenArrayNum;	// 아이템 인벤에서의 위치.
	INT32 		iItemCd;		// 아이템 코드.
};


// 주관식 퀴즈 답변
struct SubjectiveQuizAnswer
{
	SubjectiveQuizAnswer()
	{
		i64Code = 0;
		memset( acAnswer, 0, sizeof(acAnswer) );
		iValue = 0;
	}

	UINT64 i64Code;
	char	acAnswer[MAX_SUBJECTIVE_QUESTION_ANSWER_LEN];
	INT32 		iValue;
};

// 주관식 퀴즈 문제
struct SubjectiveQuiz
{
	void Init()
	{
		bEnableUse = true;
		iQuizCd = 0;
		cKind = 0;
		sQuestionLen = 0;
		memset(acQuestion, 0, MAX_SUBJECTIVE_QUESTION_LEN);
		Answers.clear();
	}

	bool	bEnableUse;								// 사용 가능 여부
	INT32 	iQuizCd;								// 퀴즈 코드
	char	cKind;									// 퀴즈 종류
	short	sQuestionLen;							// 질문 문자 길이
	char	acQuestion[MAX_SUBJECTIVE_QUESTION_LEN];// 질문

	char	acMixedAnswerNum[ SUBJECTIVE_QUIZ_ANSWER_TOTAL_COUNT+1 ]; // 답변 인덱스를 섞어 놓음
	vector<SubjectiveQuizAnswer> Answers;			// 답변
};