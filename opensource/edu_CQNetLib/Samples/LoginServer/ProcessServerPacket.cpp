#include "../../CQNetLib/session.h"
#include "../../CQNetLib/vBuffer.h"

#include "../Common/protocol.h"

#include "LoginIocpServer.h"
#include "UserManager.h"
#include "ProcessServerpacket.h"

//#pragma warning (disable: 4100)


LoginIocpServer* ProcessServerPacket::m_pLoginServer = nullptr;
UserManager* ProcessServerPacket::m_pUserManager = nullptr;

void ProcessServerPacket::SetLoginServer(LoginIocpServer* pLoginServer,
										UserManager* pUserManager)
{
	m_pLoginServer = pLoginServer;
	m_pUserManager = pUserManager;
}

bool ProcessServerPacket::funcGTL_PACKET_GAMESERVER_BASIC_INFO_RQ(CQNetLib::Session* pConnection,  DWORD dwSize , char* pRecvedMsg )
{
	short sChannelCount = 0;			
	LOGIN_GAMESERVER_INFO tLoginGameServerInfo;
	
	CQNetLib::g_pVBuffer->SetBuffer( pRecvedMsg );

	CQNetLib::g_pVBuffer->GetShort( tLoginGameServerInfo.sPort );
	CQNetLib::g_pVBuffer->GetShort( tLoginGameServerInfo.sSeverIndex );
	CQNetLib::g_pVBuffer->GetString( tLoginGameServerInfo.acSeverName );

	CQNetLib::g_pVBuffer->GetShort( sChannelCount );
	tLoginGameServerInfo.sChannelCount = sChannelCount;

	for( INT32  i = 0; i < sChannelCount; ++i )
	{
		CQNetLib::g_pVBuffer->GetShort( tLoginGameServerInfo.asMaxChannelUserCount[i] );
	}
	
	
	if( sChannelCount > MAX_CHANNEL_COUNT ) {
		//LOG( LOG_INFO_HIGH, "GAMESERVER | cProcessServerPacket::funcSTSPACKET_CHANNELS_BASIC_INFO_CN() | [RECV] 채널 개수가 지정된 개수를 넘었습니다. - (%d),", sChannelCount );
	}

	strncpy( tLoginGameServerInfo.acIP, pConnection->GetConnectionIp(), MAX_IP_LENGTH );
	tLoginGameServerInfo.pConnection = pConnection;
	
	m_pLoginServer->AddGameServerInfo( tLoginGameServerInfo );

	//LOG(LOG_INFO_NORMAL, "GAMESERVER | cProcessServerPacket::funcSTSPACKET_CHANNELS_BASIC_INFO_CN() | [RECV] 서버번호(%d), 서버이름(%s)", tLoginGameServerInfo.sSeverIndex, tLoginGameServerInfo.acSeverName );
	//LOG(LOG_INFO_NORMAL, "GAMESERVER | cProcessServerPacket::funcSTSPACKET_CHANNELS_BASIC_INFO_CN() | [RECV] Channel Count(%d)",									sChannelCount );
	return true;
}

bool ProcessServerPacket::funcSTD_PACKET_LOGININFO_AQ(CQNetLib::Session* pConnection,  DWORD dwSize , char* pRecvedMsg )
{
	auto pLoginInfoSn = (STD_PACKET_LOGININFO_AQ*)pRecvedMsg;
	
	//LOG(LOG_INFO_LOW, "LOGINSERVER | cProcessServerPacket::funcSTDPACKET_LOGININFO_SN() | [RECV] ConnKey(%d) UKey(%I64d)", pLoginInfoSn->dwConnKey , pLoginInfoSn->i64UKey);
	
	//받은 키가 유효한 키인지 검사한다.
	auto pSendUser = m_pUserManager->IsVaildConnKey( pLoginInfoSn->dwConnKey );
	if( NULL == pSendUser ) 
	{
		//LOG( LOG_INFO_LOW, "LOGINSERVER | cProcessServerPacket::funcSTDPACKET_LOGININFO_SN() | Not User ConnKey(%d)", pLoginInfoSn->dwConnKey );
		return false;
	}

	UINT64 i64UKey = 0;
	if( RESULT_OK == pLoginInfoSn->dwResult ) 
	{
		i64UKey = pLoginInfoSn->i64UKey;

		pSendUser->SetState(CONN_STATE_LOGIN_AUTH);
		pSendUser->SetUKey( i64UKey );
		pSendUser->SetUserID( pLoginInfoSn->acUserID );
		pSendUser->SetAdminLevel( pLoginInfoSn->cAdminLevel );
		
		if( false == m_pUserManager->AddAuthUser( pSendUser ) )
		{
			pSendUser->SetState(CONN_STATE_LOGIN_AUTH_DUPLICATION);
			pLoginInfoSn->dwResult = LOGININFO_AQ_RESULT_DUPLICATION;
		}
	}
	
	// 클라이언트에 통보한다.
	pSendUser->Send_CTLPACKET_LOGIN_SN( pLoginInfoSn->dwResult, i64UKey );
	
	// 성공인 경우 채널 정보를 통보 한다.
	if( RESULT_OK == pLoginInfoSn->dwResult ) 
	{
		m_pLoginServer->SendChannelInfo( CTL_CHANNEL_INFO_MSG, pSendUser );
	}

	return true;
}

bool ProcessServerPacket::funcGTL_PACKET_CLIENT_AUTH_CONFIRM_RQ(CQNetLib::Session* pkConnection,  DWORD dwSize , char* pRecvedMsg )
{
	GTL_PACKET_CLIENT_AUTH_CONFIRM_RQ* ptReq = (GTL_PACKET_CLIENT_AUTH_CONFIRM_RQ*)pRecvedMsg;

	//LOG( LOG_INFO_LOW, "GAMESERVER | cProcessServerPacket::funcGTL_PACKET_CLIENT_AUTH_CONFIRM_RQ() | [RECV] UKey(%I64d)", ptReq->i64UKey );
	
	// 보안문자가 같은지 체크 한다.
	char cResult = RESULT_OK;
	AuthenticUser* ptAuthUser = m_pUserManager->GetAuthUser(ptReq->i64UKey);
	
	if( NULL == ptAuthUser )
	{ 
		//LOG( LOG_ERROR_NORMAL, "GAMESERVER | cProcessServerPacket::funcLTG_PACKET_CLIENT_AUTH_CONFIRM_RQ() | UserKey(%I64d)", ptReq->i64UKey );

		cResult = 1;
	}
	else if( 0 != strncmp( ptAuthUser->acSecureStr, ptReq->acSecureStr, LEN_SECURE_STR-1 ) ) 
	{
		//LOG(LOG_ERROR_NORMAL, "GAMESERVER | cProcessServerPacket::funcLTG_PACKET_CLIENT_AUTH_CONFIRM_RQ() | Original SecureStr(%s), Received SecureStr(%s)", ptAuthUser->acSecureStr, ptReq->acSecureStr );

		cResult = 2;
	}
	

	LTG_PACKET_CLIENT_AUTH_CONFIRM_AQ tRetPkt;
	tRetPkt.cResult = cResult;
	tRetPkt.i64UKey = ptReq->i64UKey;
	tRetPkt.dwGameConnectKey = ptReq->dwGameConnectKey;
	tRetPkt.sChannelNum = ptReq->sChannelNum;
	
	if( RESULT_OK == cResult ) 
	{
		tRetPkt.cAdminLevel = ptAuthUser->cAdminLevel;
		strncpy( tRetPkt.acID, ptAuthUser->acID, MAX_USERID-1 );	tRetPkt.acID[MAX_USERID-1] = '\0';
		tRetPkt.cSex = ptAuthUser->cSex;
		tRetPkt.sAge = ptAuthUser->sAge;
	}
		
	pkConnection->SendBuffer( (char*)&tRetPkt, tRetPkt.usLength );

	return true;
}

bool ProcessServerPacket::funcGTL_PACKET_CLIENT_GAME_AUTH_MSG(CQNetLib::Session* pkConnection,  DWORD dwSize , char* pRecvedMsg )
{
	GTL_PACKET_CLIENT_GAME_AUTH_MSG* ptMsg = (GTL_PACKET_CLIENT_GAME_AUTH_MSG*)pRecvedMsg;

	//LOG( LOG_INFO_LOW, "GAMESERVER | cProcessServerPacket::funcGTL_PACKET_CLIENT_GAME_AUTH_MSG() | [RECV] UKey(%I64d)", ptMsg->i64UKey );
	
	// 게임서버의 채널에 숫자를 카운트 한다.
	m_pLoginServer->SetUserGameServerAuth( pkConnection->GetIndex(), 
											true, 
											ptMsg->i64UKey, 
											ptMsg->i64CharCd,
											ptMsg->sGameServerIndex );
	return true;
}

bool ProcessServerPacket::funcLTG_PACKET_LOGOUT_CLIENT_MSG(CQNetLib::Session* pkConnection,  DWORD dwSize , char* pRecvedMsg )
{
	GTL_PACKET_LOGOUT_CLIENT_MSG* ptMsg = (GTL_PACKET_LOGOUT_CLIENT_MSG*)pRecvedMsg;

	//LOG( LOG_INFO_LOW, "GAMESERVER | cProcessServerPacket::funcLTG_PACKET_LOGOUT_CLIENT_MSG() | [RECV] UKey(%I64d)", ptMsg->i64UKey );
	
	// 게임서버에 접속한 유저를 로그 아웃 처리한다.
	m_pLoginServer->SetUserGameServerAuth(pkConnection->GetIndex(), 
										false, 
										ptMsg->i64UKey, 
										ptMsg->i64CharCd,
										ptMsg->sGameServerIndex );
	return true;
}

bool ProcessServerPacket::funcGTL_PACKET_CHANNEL_LIST_RQ(CQNetLib::Session* pkConnection,  DWORD dwSize , char* pRecvedMsg )
{
	GTL_PACKET_CHANNEL_LIST_RQ* ptReq = (GTL_PACKET_CHANNEL_LIST_RQ*)pRecvedMsg;
	m_pLoginServer->SendChannelInfo( pkConnection, ptReq->i64CharCd, ptReq->sCurChannelNum, ptReq->sSeqCdInLobby);
	
	return true;
}

bool ProcessServerPacket::funcGTL_PACKET_CHANGE_CHANNEL_MSG(CQNetLib::Session* pkConnection,  DWORD dwSize , char* pRecvedMsg )
{
	GTL_PACKET_CHANGE_CHANNEL_MSG* ptMsg = (GTL_PACKET_CHANGE_CHANNEL_MSG*)pRecvedMsg;

	m_pLoginServer->SetAuthUserChangeGameServerSecondTime( ptMsg->i64UKey );
	return true;
}

bool ProcessServerPacket::funcGTL_PACKET_LOBBY_CUR_USER_COUNT_MSG(CQNetLib::Session* pkConnection,  DWORD dwSize , char* pRecvedMsg )
{
	GTL_PACKET_LOBBY_CUR_USER_COUNT_MSG* ptMsg = (GTL_PACKET_LOBBY_CUR_USER_COUNT_MSG*)pRecvedMsg;

	m_pLoginServer->ChannelUserCount( ptMsg->sGameServerIndex, ptMsg->sChannelNum, ptMsg->sUserCount );
	
	return true;
}