#pragma once

#define WIN32_LEAN_AND_MEAN  
#include <windows.h>


#include "../../CQNetLib/commonDef.h"
#include "../../CQNetLib/session.h"

#include "../Common/CommonDefine.h"


enum eLoginServerSystemMsg {
	SYSTEM_REMOVE_AUTH_USER,
	SYSTEM_CHECK_AUTH_USER
};


// 로그인 서버의 최대 인증자 수
const INT32 MAX_AUTH_USER_COUNT = 100000;

// 게임 서버 변경 시 최대 유효 대기 시간(초)
const INT32 MAX_CHANGE_GAMESERVER_WAIT_TIME = 7;



// 로그인 서버에서 관리하는 게임서버 정보
struct LOGIN_GAMESERVER_INFO
{
	LOGIN_GAMESERVER_INFO()
	{
		sPort = 0;
		memset( acIP, 0, MAX_IP_LENGTH);
		sSeverIndex = -1;
		memset( acSeverName, 0, MAX_SERVER_NAME_LEN);
		sChannelCount = 0;			
		memset( asMaxChannelUserCount, 0, MAX_CHANNEL_COUNT);
		memset( asCurChannelUserCount, 0, MAX_CHANNEL_COUNT);
		pConnection = NULL;
	}

	short sPort;									// 포트 번호
	char acIP[MAX_IP_LENGTH];						// IP
	short sSeverIndex;								// 게임서버 인덱스 번호
	char acSeverName[MAX_SERVER_NAME_LEN];			// 서버 이름
	
	short sChannelCount;							// 채널 개수
	short asMaxChannelUserCount[MAX_CHANNEL_COUNT];	// 채널의 최대 수용인원
	short asCurChannelUserCount[MAX_CHANNEL_COUNT];	// 채널의 현재 인원

	CQNetLib::Session* pConnection;
};


// 인증 유저
struct AuthenticUser
{
	void Init(INT32 index)
	{
		Index = index;
		Clear(false);
	}

	void Clear(bool IsDupliCation)
	{
		if( false == IsDupliCation ) {
			pkConnection			= NULL;
		}

		bGameServerConnect = false;
		iGameServerConnectIndex = 0;
		sGameServerIndex = 0;
		i64UKey = 0;
		cAdminLevel	= 0;
		memset( acID, 0, MAX_USERID );
		memset( acSecureStr, 0, LEN_SECURE_STR );
		cSex = 0;
		sAge = 0;
		i64CharCd = 0;
		uiChangeGamseServerSecondTime = 0;
	}

	INT32	Index;
	CQNetLib::Session* pkConnection;
	bool	bGameServerConnect;				// 게임서버 접속 여부
	INT32 		iGameServerConnectIndex;		// 유저가 접속한 게임서버의 연결 인덱스( 로그인 서버에 연결된 게임서버 연결 객체의 인덱스 ) 
	short	sGameServerIndex;				// 유저가 접속한 게임서버의 인덱스 번호
	UINT64	i64UKey;						// 유저 키
	char	cAdminLevel;					// 관리자 레벨
	char	acID[MAX_USERID];				// 아이디
	char	acSecureStr[LEN_SECURE_STR];	// 보안문자
	char	cSex;							// 성별
	short	sAge;							// 나이
	UINT64	i64CharCd;						// 캐릭터 코드
	UINT32  uiChangeGamseServerSecondTime; // 클라이언트가 다른 게임서버에 접속을 시작하는 시간
};


