#include "../Common/protocol.h"
#include "User.h"

User::User(void)
{
	Init();
}

User::~User(void)
{
}

//Discription	: 유저 초기화.
void User::Init()
{
	ZeroMemory( m_szUserID, MAX_USERID );
	m_nSendReLoginCnt = 0;
	m_i64UKey = 0;
	m_uiConnKey = 0;
	m_cAdminLevel = 0;
	SetState( CONN_STATE_OFFLINE );
	ZeroMemory( m_acSecureStr , LEN_SECURE_STR );
	m_cSex = 0;
	m_sAge = 0;
}

//Discription	: 디비에서 받은 유저 캐릭터 정보를 보낸다.
//Parameters	:	pBuffer		- 보낼 데이터
//				nLen		- 데이터의 길이
bool User::SendBuffer(const char* pBuffer, __in INT32  nLen )
{
	CQNetLib::Session::SendBuffer( pBuffer , nLen );
	return true;
}

//Discription	: 인증 결과를 통보.
//Parameters	:	dwResult	- 인증 결과
//				dwUKey		- 유저 키
bool User::Send_CTLPACKET_LOGIN_SN( __in DWORD dwResult, __in UINT64 i64UKey )
{
	PREPARE_SENDPACKET( CTL_PACKET_LOGIN_AQ, CTL_LOGIN_AQ, pLoginResult )
	pLoginResult->dwResult = dwResult;
	pLoginResult->i64UKey = i64UKey;
	pLoginResult->cSex = m_cSex;
	pLoginResult->cAdminLevel = GetAdminLevel();

	if( RESULT_OK == dwResult ) {
		CopyMemory( pLoginResult->acSecureString, GetSecureStr(), LEN_SECURE_STR );
	}

	SendPost( pLoginResult->usLength );
	return true;
}

