#include "../../CQNetLib/log.h"
#include "../../CQNetLib/SpinLock.h"
#include "UserManager.h"


UserManager::UserManager(void)
{
	m_uiConnKeyCnt = 0;		//키를생성하기위한것
	m_nMaxConnectionCnt = 0;
	m_pUserObj = NULL;

	for(INT32 i = 0; i < MAX_AUTH_USER_COUNT; ++i )
	{
		m_atAuthenticUsers[i].Init(i);
		m_NotUseAuthUserSeqNums.push_back(i);
	}
}

UserManager::~UserManager(void)
{
	LOG(CQNetLib::LOG_INFO_NORMAL , "SYSTEM | cUserManager::~cUserManager() | cUserManager 소멸자 호출" );

	CQNetLib::SpinLockGuard lock(m_Lock);
		
	if( false == m_mapAuthUser.empty())
	{
		m_mapAuthUser.clear();
	}

	if( false == m_mapUser.empty() )
	{
		m_mapUser.clear();
	}

	if( m_pUserObj)
	{
		SAFE_DELETE_ARRAY( m_pUserObj );
	}
}

//Discription	: 유저 객체를 초기화 한다.
//Parameters	:	initConfig			- 서버 설정 정보
//				nMaxConnectionCnt	- 최대 접속 가능한 수	
bool UserManager::CreateUser( __deref_in CQNetLib::INITCONFIG &initConfig , __deref_in INT32  nMaxConnectionCnt )
{
	CQNetLib::SpinLockGuard lock(m_Lock);

	m_nMaxConnectionCnt = nMaxConnectionCnt;
	m_pUserObj = new User[ nMaxConnectionCnt ];
	
	for( INT32  i=0 ; i< nMaxConnectionCnt ; i++ )
	{
		initConfig.nIndex = i ;
		initConfig.ConnectionType = CQNetLib::CT_CLIENT;

		if (m_pUserObj[i].CreateConnection(initConfig) == false)
		{
			return false;
		}
	}

	return true;

}

//Discription	: 접속되어있는 connection중 connkey로 cUser객체를 찾아서 포인터를 반환한다.
//Parameters	:	uiConnKey			- 접속 키 
User* UserManager::IsVaildConnKey( __in UINT32  uiConnKey )
{
	CQNetLib::SpinLockGuard lock(m_Lock);
	
	auto user_it = m_mapUser.find( uiConnKey );
	if( m_mapUser.end() == user_it )
	{
		LOG(CQNetLib::LOG_ERROR_LOW , " SYSTEM | cUserManager::IsVaildConnKey() | PKey(%d) 는 m_mapUser 에 존재안함", uiConnKey );
		return nullptr;
	}

	return (User*)user_it->second;
}

//Discription	: Connection을 사용자 m_mapUser에 추가한다.
//Parameters	:	pUser			- cUser의 인스턴스 
bool UserManager::AddUser_Aysc( __deref_inout User* pUser )
{
	CQNetLib::SpinLockGuard lock(m_Lock);

	//커넥션 키 생성
	pUser->SetConnKey( ++m_uiConnKeyCnt );

	//오류 체크
	auto user_it = m_mapUser.find( m_uiConnKeyCnt );
	if( m_mapUser.end() != user_it )
	{
		//심각한 문제를 초래할수도 있음
		LOG(CQNetLib::LOG_ERROR_NORMAL , " SYSTEM | cUserManager::AddUser() | ConnectKey(%d) 는 이미 m_mapUser 에 있음", m_uiConnKeyCnt );
		m_mapUser.erase( user_it );
	}

	//사용자를 추가 시킴
	pUser->SetState(CONN_STATE_ONLINE);
	m_mapUser.insert( USER_PAIR ( m_uiConnKeyCnt , pUser ) );
	
	return true;
}

//Discription	: m_mapUser에서 connection을 제거한다.
//Parameters	:	pUser			- cUser의 인스턴스 
bool UserManager::RemoveUser_Aysc( __deref_inout User* pUser )
{
	CQNetLib::SpinLockGuard lock(m_Lock);
	
	DWORD dwUKey = pUser->GetUKey();

	auto user_it = m_mapUser.find( pUser->GetConnKey() );
	if( m_mapUser.end() == user_it )
	{
		LOG(CQNetLib::LOG_ERROR_NORMAL , " SYSTEM | cUserManager::RemoveUser() | ConnectKey[%d]의 객체는 m_mapUser에 존재하지 않습니다"
				,pUser->GetConnKey() );
			
		return false;
	}

	pUser->Init();
	m_mapUser.erase( user_it );
	
	RemoveAuthUser( true, false, dwUKey );	
	return true;

}

//Discription: KeepAlive Time이 Over된것을 Close처리.
//Parameters:	dwServerTick - 지정된 시간마다 연결된 클라이언트 상태 체크. 잘못된 클라이언트는 종료 시킴. 
void UserManager::CheckCloseUser( __in DWORD dwServerTick )
{
	CQNetLib::SpinLockGuard lock(m_Lock);

	//오류 체크
	for( auto user_it = m_mapUser.begin() ; user_it != m_mapUser.end() ; user_it++ )
	{
		User* pUser = user_it->second;
		//시간이 다했다면 연결을 끊는다.
		if( pUser->GetServerTick() < dwServerTick - CHECKTIME_LOGIN )
		{
			//게임서버 커넥션은 빼놓는다.
			if (pUser->GetConnectionType() != CQNetLib::CT_CLIENT)
			{
				continue;
			}

			LOG(CQNetLib::LOG_INFO_HIGH , " SYSTEM | cUserManager::CheckCloseUser() | usertick(%d) servertick(%d)유지 시간 만료 ", pUser->GetServerTick() , dwServerTick );

			//g_pLoginIocpServer->CloseConnection( pUser);
		}
	}
}

bool UserManager::CheckReLogin( __deref_in User* pUser )
{
	CQNetLib::SpinLockGuard lock(m_Lock);

	//오류 체크
	for( auto user_it = m_mapUser.begin() ; user_it != m_mapUser.end() ; user_it++ )
	{
		auto pTempUser = user_it->second;

		//자신은 제외한다.
		if( pTempUser == pUser ) 
			continue;

		//로긴서버에 로긴되어 있다면..
		if( strcmpi( pTempUser->GetUserID() , pUser->GetUserID() ) == 0 )
		{
			LOG(CQNetLib::LOG_INFO_NORMAL , "SYSTEM | cUserManager::CheckRelogin() | 유저(%s)사용자가 리로긴하였음" , pUser->GetUserID() );			
			//g_pLoginIocpServer->CloseConnection( pTempUser );
			return true;
		}
	}
	return false;
}

//Discription : 아이디를 넣으면 보안키를 만든다.
//Parameters	:	src				- 소스
//				pcSecureSTR		- 만들어진 보안키를 담을 저장소
void UserManager::CreateMD5Code( __deref_in char *src, __deref_out char* pcSecureSTR )
{
	time_t tm;
	time(&tm);

	char acChangeSrc[128] = {0,};
	sprintf_s( acChangeSrc, 128, "%s%d", src, static_cast<INT32 >(tm) );

	m_kMD5.setPlainText( acChangeSrc );
	CopyMemory( pcSecureSTR, m_kMD5.getMD5Digest(), LEN_SECURE_STR-1 );
}

/*
Discription : 인증유저맵에 추가한다. 호출하는 곳에서 동기화를 해주고 있음
Parameters  :	pkUser			- 유저 객체
*/
bool UserManager::AddAuthUser( __deref_in User* pkUser )
{
	CQNetLib::SpinLockGuard lock(m_Lock);
	
	auto i64UKey = pkUser->GetUKey();

	auto authuser_it = m_mapAuthUser.find( i64UKey );
	if( m_mapAuthUser.end() != authuser_it )
	{
		// 중복 접속
		LOG(CQNetLib::LOG_ERROR_NORMAL, " SYSTEM | cUserManager::AddAuthUser() | UKey(%I64d) 는 이미 m_mapAuthUser 에 있음", i64UKey );
		return false;
	}

	//사용자를 추가 시킴
	CreateMD5Code( pkUser->GetUserID(), pkUser->GetSecureStr());

	//pUser->SetState(CONN_STATE_ONLINE);
	if( false == m_NotUseAuthUserSeqNums.empty())
	{
		INT32 UseIndex = m_NotUseAuthUserSeqNums[0];
		m_NotUseAuthUserSeqNums.pop_front();

		m_atAuthenticUsers[ UseIndex ].pkConnection = (CQNetLib::Session*)pkUser;
		m_atAuthenticUsers[ UseIndex ].i64UKey = i64UKey;
		m_atAuthenticUsers[ UseIndex ].cAdminLevel = pkUser->GetAdminLevel();
		strncpy( m_atAuthenticUsers[ UseIndex ].acID, pkUser->GetUserID(), MAX_USERID-1 );
		strncpy( m_atAuthenticUsers[ UseIndex ].acSecureStr, pkUser->GetSecureStr(), LEN_SECURE_STR-1 );
		m_atAuthenticUsers[ UseIndex ].cSex = pkUser->GetSex();
		m_atAuthenticUsers[ UseIndex ].sAge = pkUser->GetAge();

		m_mapAuthUser.insert( AUTH_USER_PAIR ( i64UKey, &m_atAuthenticUsers[ UseIndex ] ) );
		LOG(CQNetLib::LOG_INFO_NORMAL, " SYSTEM | cUserManager::AddAuthUser() | UKey(%I64d), SecureText(%s) m_mapAuthUser 에 추가", i64UKey, m_atAuthenticUsers[ UseIndex ].acSecureStr );
	}
	else
	{
		LOG(CQNetLib::LOG_ERROR_CRITICAL, " SYSTEM | cUserManager::AddAuthUser() | 사용하지 않는 인증자 객체가 없습니다.");
	}
	
	return true;

}

//Discription : 인증유저맵에서 제거한다. 호출하는 곳에서 동기화를 해주고 있음
//Parameters  :	bDisConnect		- 로그인 서버와 연결이 끊어짐
//				dwUKey			- 유저키	
bool UserManager::RemoveAuthUser( __in bool bDisConnect, __in bool IsDupliCation, __in UINT64 i64UKey )
{
	if( i64UKey <= 0 ) {
		return true;
	}
		
	auto authuser_it = m_mapAuthUser.find( i64UKey );
	if( m_mapAuthUser.end() == authuser_it )
	{
		LOG(CQNetLib::LOG_ERROR_NORMAL, " SYSTEM | cUserManager::RemoveAuthUser() | UKey[%I64d]의 객체는 m_mapAuthUser에 존재하지 않습니다", i64UKey );
		return false;
	}
	
		
	if( false == authuser_it->second->bGameServerConnect || 
		(false == bDisConnect && authuser_it->second->bGameServerConnect) )
	{
		// 다른 게임서버에 접속 예정인 유저는 제거 하지 않는다.
		if( 0 == authuser_it->second->uiChangeGamseServerSecondTime )
		{
			INT32 Index = authuser_it->second->Index;
			m_NotUseAuthUserSeqNums.push_back(Index);

			m_mapAuthUser.erase( authuser_it );
			m_atAuthenticUsers[ Index ].Clear(IsDupliCation);
			LOG(CQNetLib::LOG_INFO_NORMAL, " SYSTEM | cUserManager::RemoveAuthUser() | UKey[%I64d]의 객체는 m_mapAuthUser에서 제거", i64UKey );
		}
	}

	return true;

}

//Discription : 인증 유저를 찾는다.
//Parameters  :	dwUKey			- 유저 키
//Return		:	AuthenticUser	- 객체
AuthenticUser* UserManager::GetAuthUser( __in UINT64 i64UKey )
{
	CQNetLib::SpinLockGuard lock(m_Lock);

	if( i64UKey <= 0 ) {
		return nullptr;
	}

	// 검색이므로 동기화 하지 않아도 됨.
	auto authuser_it = m_mapAuthUser.find( i64UKey );
	if( m_mapAuthUser.end() == authuser_it ) {
		return nullptr;
	}

	return authuser_it->second;
}

//Discription	: 인증자들이 오류가 없는지 체크한다.
void UserManager::CheckAuthUsers()
{
	static INT32 CheckCount = 0;
	if( CheckCount >= MAX_AUTH_USER_COUNT ) {
		CheckCount = 0;
	}

	//TODO 현재 시간 얻도록 하기
	UINT32 CurTime = 0;// g_pLoginIocpServer->GetCurSecondTime();

	// 한번에 300번만 조사한다.
	INT32 LimitNum = CheckCount + 300;	
	for( INT32 i = CheckCount; i < LimitNum; ++i )
	{
		if( 0 == m_atAuthenticUsers[ i ].i64UKey ) {
			continue;
		}

		// 게임서버를 요청한 유저가 아직 다른 서버에 접속하지 않았는지 체크
		if( 0 != m_atAuthenticUsers[ i ].uiChangeGamseServerSecondTime && 
			(m_atAuthenticUsers[ i ].uiChangeGamseServerSecondTime + MAX_CHANGE_GAMESERVER_WAIT_TIME) <= CurTime )
		{
			m_atAuthenticUsers[ i ].bGameServerConnect = false;
			m_atAuthenticUsers[ i ].uiChangeGamseServerSecondTime = 0;
			RemoveAuthUser( false, false, m_atAuthenticUsers[ i ].i64UKey);
		}
	}
}


