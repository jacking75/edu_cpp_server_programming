#pragma once

#include <unordered_map>
#include <deque>

#include "../../CQNetLib/singleton.h"

#define CHECK
#include "LoginServerDefine.h"
#include "User.h"
#include "md5class.h"



class UserManager :  public CQNetLib::Singleton< UserManager >
{
public:
	UserManager(void);
	virtual ~UserManager(void);
	
	typedef std::pair<DWORD, User*>			USER_PAIR;
	typedef std::unordered_map<DWORD, User*>		USER_MAP;
	//typedef USER_MAP::iterator				USER_IT;	
	
	typedef std::pair<UINT64, AuthenticUser*>		AUTH_USER_PAIR;
	typedef std::unordered_map<UINT64 , AuthenticUser*>	AUTH_USER_MAP;
	//typedef AUTH_USER_MAP::iterator					AUTH_USER_IT;

	
protected:
	INT32			m_nMaxConnectionCnt;

	// concurrencyRuntime 컨테이너로 바꾸기
	USER_MAP		m_mapUser;			// 인증된 유저 관리  
	AUTH_USER_MAP	m_mapAuthUser;		// 접속된 유저 관리
	
	User*			m_pUserObj;			// 유저 객체들
	AuthenticUser	m_atAuthenticUsers[MAX_AUTH_USER_COUNT]; // 인증자 객체들
	std::deque<INT32>	m_NotUseAuthUserSeqNums;	// 사용하지 않은 인증자 객채의 순서 번호			
	UINT32 	m_uiConnKeyCnt;				// 접속 횟수를 카운트 하는 변수.
		
public:
	//TODO initConfig 애플리케이션 설정 정보로 바꾸기
	// 유저 객체를 초기화 한다.
	bool CreateUser( __deref_in CQNetLib::INITCONFIG &initConfig , __deref_in INT32  nMaxConnectionCnt );

	// 접속되어있는 connection중 connkey로 cUser객체를 찾아서 포인터를 반환한다.
	User*		IsVaildConnKey( __in  UINT32  uiConnKey );
	
	// Connection을 사용자 m_mapUser에 추가한다.
	bool		AddUser_Aysc(__deref_inout User* pUser );
	
	// m_mapUser에서 connection을 제거한다.
	bool		RemoveUser_Aysc(__deref_inout User* pUser );
	
	// KeepAlive Time이 Over된것을 Close처리
	void		CheckCloseUser( __in DWORD dwServerTick );

	// 유저 객체 오브젝트의 포인터 반환
	User*	GetConnObj() { return m_pUserObj; }
	
	// 저장된 유저 수 반환
	INT32 		GetUserCnt() { return static_cast<INT32 >(m_mapUser.size()); }
	
	// 재접속 체크 - 현재 비 사용
	bool	CheckReLogin( __deref_in User* pUser );

	// 인증유저맵에 추가한다.
	bool	AddAuthUser( __deref_in User* pkUser );
	
	// 인증유저맵에서 제거한다.
	bool	RemoveAuthUser( __in bool bDisConnect, __in bool IsDupliCation, __in UINT64 i64UKey );
	
	// 아이디를 넣으면 보안키를 만든다.
	void CreateMD5Code( __deref_in char *src, __deref_out char* pcSecureSTR );		

	// 인증 유저를 찾는다.
	AuthenticUser* GetAuthUser( __in UINT64 i64UKey );
	
	// 인증자들이 오류가 없는지 체크한다.
	void CheckAuthUsers();


private:
	CMD5 m_kMD5;
	CQNetLib::SpinLock m_Lock;
};


#define g_pUserManager				UserManager::GetSingleton()