#include "../../CQNetLib/commonDef.h"
#include "../../CQNetLib/log.h"
#include "../../CQNetLib/serverSessionMgr.h"

#include "../Common/protocol.h"

#include "User.h"
#include "UserManager.h"
#include "LoginIocpServer.h"
#include "ProcessClientPacket.h"

//#pragma warning (disable: 4100)

LoginIocpServer* ProcessClientPacket::m_pLoginServer = nullptr;
CQNetLib::ServerConnectionManager* ProcessClientPacket::m_pServerConnectionMgr = nullptr;

void ProcessClientPacket::SetRefObject(LoginIocpServer* pLoginServer, 
							UserManager* pUserManager, 
								CQNetLib::ServerConnectionManager* pServerConnectionMgr)
{
	m_pLoginServer = pLoginServer;
	m_pUserManager = pUserManager;
	m_pServerConnectionMgr = pServerConnectionMgr;
}

bool ProcessClientPacket::funcCTL_PACKET_LOGIN_RQ( User* pUser,  DWORD dwSize , char* pRecvedMsg )
{
	auto pLoginCn = (CTL_PACKET_LOGIN_RQ*)pRecvedMsg;
	
	//문자열은 패킷의 끝에 NULL을 넣어주어야 한다. 안그러면 버그 가능성 
	pLoginCn->acUserID[ MAX_USERID - 1 ] = '\0';
	pLoginCn->acUserPW[ MAX_USERPW - 1 ] = '\0';
	
	//LOG(LOG_INFO_LOW, "CLIENT | cProcessClientPacket::funcCTSPACKET_LOGIN_CN() | [RECV] ConnKey(%d) ID(%s) PW(%s)", pUser->GetConnKey() , pLoginCn->acUserID , pLoginCn->acUserPW );

	auto pDBConn = m_pServerConnectionMgr->GetNextConnection(CQNetLib::CT_LOGINDBSERVER );
	if( NULL == pDBConn )
	{
		//LOG(LOG_ERROR_NORMAL, "SYSTEM | cProcessClientPacket::funcCTSPACKET_LOGIN_CN() | LoginLoginDBServer와 연결이 끊겨있습니다.");
		
		pUser->Send_CTLPACKET_LOGIN_SN( 1, 0 );
		return false;
	}

	//패킷을 준비한다.
	PREPARE_SENDPACKET_CONN( pDBConn , STD_PACKET_LOGIN_RQ, STD_LOGIN_RQ, pSendLoginCn )
	//패킷을 전부다 복사한뒤에 싸이즈와 소켓 ID를 다시 입력한다.
	strncpy( pSendLoginCn->acUserID, pLoginCn->acUserID, MAX_USERID-1 );
	strncpy( pSendLoginCn->acUserPW, pLoginCn->acUserPW, MAX_USERPW-1 );
	pSendLoginCn->dwConnKey = pUser->GetConnKey();
	pDBConn->SendPost( pSendLoginCn->usLength );
	
	return true;
}

bool ProcessClientPacket::funcCTL_PACKET_LOGIN_QUIT_FORCE_RQ( User* pkUser,  DWORD dwSize , char* pRecvedMsg )
{
	PREPARE_SENDPACKET_CONN( pkUser, CTL_PACKET_LOGIN_QUIT_FORCE_AQ, CTL_LOGIN_QUIT_FORCE_AQ, ptRet );

	char __cResult = 0;

CHECK_START
	// 중복 접속 상태인가 ?
	if( false == pkUser->IsDupliCationState() ) {
		ptRet->cResult = 1;
		pkUser->SendPost(ptRet->usLength);
		return false;
	}

	auto ptAuthenticUser = g_pUserManager->GetAuthUser(pkUser->GetUKey());
	if( NULL == ptAuthenticUser ) {
		ptRet->cResult = 2;
		pkUser->SendPost(ptRet->usLength);
		return false;
	}

	bool bConnectedGameServer = ptAuthenticUser->bGameServerConnect;
	short sGameServerIndex = ptAuthenticUser->sGameServerIndex;
	UINT64 RemoveUserUKey = ptAuthenticUser->i64UKey;
	UINT64 RemoveCharCd = ptAuthenticUser->i64CharCd;


	//  인증 맵에서 제거 한다.
	g_pUserManager->RemoveAuthUser( false, true, ptAuthenticUser->i64UKey );


	if (NULL != ptAuthenticUser->pkConnection)
	{
		g_pLoginIocpServer->CloseConnection(ptAuthenticUser->pkConnection);
		ptAuthenticUser->pkConnection = NULL;
	}

	//// 게임 서버에 통보한다.
	//if( bConnectedGameServer )
	//{
	//	auto pkGameServer = g_pLoginIocpServer->GetGameServerConnection(sGameServerIndex);
	//	if( NULL != pkGameServer && INVALID_SOCKET != pkGameServer->GetSocket() )
	//	{
	//		PREPARE_SENDPACKET_CONN( pkGameServer, LTG_PACKET_LOGIN_QUIT_FORCE_MSG, LTG_LOGIN_QUIT_FORCE_MSG, ptMsg );
	//		ptMsg->i64UKey = RemoveUserUKey;
	//		ptMsg->i64CharCd = RemoveCharCd;

	//		pkGameServer->SendPost( ptMsg->usLength );
	//	}
	//}
	//else
	//{
	//	if( NULL != ptAuthenticUser->pkConnection )
	//	{
	//		g_pLoginIocpServer->CloseConnection( ptAuthenticUser->pkConnection);
	//		ptAuthenticUser->pkConnection = NULL;
	//	}
	//}

	
	// 클라이언트에 통보한다.
	ptRet->cResult = RESULT_OK;
	pkUser->SendPost( ptRet->usLength );
	return true;

CHECK_ERR:
	ptRet->cResult = __cResult;
	pkUser->SendPost( ptRet->usLength );
	return false;
}

bool ProcessClientPacket::funcCTL_PACKET_DEV_LOGIN_CLOSE_RQ( User* pkUser,  DWORD dwSize , char* pRecvedMsg )
{
	
	auto ptAuthenticUser = g_pUserManager->GetAuthUser(pkUser->GetUKey());
	if( NULL == ptAuthenticUser ) {
		LOG(CQNetLib::LOG_ERROR_NORMAL, "SYSTEM | cProcessClientPacket::funcCTL_PACKET_DEV_LOGIN_CLOSE_RQ() | UKey(%I64d) 인증 받지 않은 유저.", pkUser->GetUKey() );
		return false;
	}

	/*if( ptAuthenticUser->cAdminLevel < CQNetLib::ADMIN_LEVEL_DEV ) {
		LOG(CQNetLib::LOG_ERROR_NORMAL, "SYSTEM | cProcessClientPacket::funcCTL_PACKET_DEV_LOGIN_CLOSE_RQ() | UKey(%I64d) 관리자가 아닙니다.", pkUser->GetUKey() );
		return false;
	}*/


	//  인증 맵에서 제거 한다.
	g_pUserManager->RemoveAuthUser( false, true, ptAuthenticUser->i64UKey );

	
	if( NULL != ptAuthenticUser->pkConnection )
	{
		g_pLoginIocpServer->CloseConnection( ptAuthenticUser->pkConnection);
		ptAuthenticUser->pkConnection = NULL;
	}
	
	return true;

}


