#pragma once

#include "../../CQNetLib/session.h"

#include "../Common/CommonDefine.h"


class User : public CQNetLib::Session
{
public:
	User(void);
	~User(void);

	// 유저 초기화
	void			Init();		

	char*			GetUserID() { return m_szUserID; }
	
	void			SetUserID(const char* szUserID) 
	{ 
		strncpy( m_szUserID , szUserID , MAX_USERID-1 ); 
	}
	
	UINT32 	GetConnKey() { return m_uiConnKey; }
	void  			SetConnKey( __in UINT32  uiConnKey ) { m_uiConnKey = uiConnKey; }
	
	UINT64			GetUKey() { return m_i64UKey; }
	void  			SetUKey( __in UINT64 i64UKey ) { m_i64UKey = i64UKey; }
		
	char			GetSex() { return m_cSex; }
	short			GetAge() { return m_sAge; }

	void			SetAdminLevel( char cAdminLevel ) { m_cAdminLevel = cAdminLevel; }
	char			GetAdminLevel() { return m_cAdminLevel; }
	
	
	// 유저의 접속 상태 설정
	void			SetState( __in char cState ) { m_cState = cState; }
	// 중복 접속 중인가 ?
	bool			IsDupliCationState() {
		if( CONN_STATE_LOGIN_AUTH_DUPLICATION == m_cState ) {
			return true;
		}
		return false;
	}
	
	void 			IncreaseSendReLoginCnt() { ++m_nSendReLoginCnt; }
	void 			DecreaseSendReLoginCnt() { --m_nSendReLoginCnt; }
	INT32   			GetSendReLoginCnt() { return m_nSendReLoginCnt; }
	
	char*			GetSecureStr() { return m_acSecureStr; }

	bool			SendBuffer(const char* pBuffer, __in INT32  nLen );
	bool			Send_CTLPACKET_LOGIN_SN( __in DWORD dwResult, __in UINT64 i64UKey );


private:
	char			m_szUserID[ MAX_USERID ];
	UINT32 	m_uiConnKey;
	UINT64			m_i64UKey;
	char			m_cAdminLevel;					// 관리자 레벨
	char			m_cState;
	char			m_acSecureStr[LEN_SECURE_STR];	// 보안 문자
	char			m_cSex;							// 성별
	short			m_sAge;							// 나이

	INT32 				m_nSendReLoginCnt;
};
