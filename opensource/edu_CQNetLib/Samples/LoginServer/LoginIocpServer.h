#pragma once

#include <vector>

#include "../../CQNetLib/IocpServer.h"
#include "../../CQNetLib/serverSessionMgr.h"

#include "TickThread.h"
#include "User.h"

#define LOGIN_SERVER


class CQNetLib::Session;

class LoginIocpServer : public CQNetLib::IocpServer , public CQNetLib::Singleton< LoginIocpServer >
{
public:
	LoginIocpServer(void);
	virtual ~LoginIocpServer(void);

	union FuncProcess
	{
		bool (*funcProcessClientPacket)( User* pUser,  DWORD dwSize , char* pRecvedMsg );
		bool (*funcProcessServerPacket)(CQNetLib::Session* pConnection,  DWORD dwSize , char* pRecvedMsg );

		FuncProcess()
		{
			funcProcessClientPacket = NULL;
			funcProcessServerPacket = NULL;
		}
	};

	// client가 접속 수락이 되었을 때 호출되는 함수
	virtual	bool	OnAccept( OUT CQNetLib::Session*pConnection );
	
	// client에서 packet이 도착했을 때 순서 성 있게 처리되어지는 패킷처리
	virtual	bool	OnRecv(__deref_in CQNetLib::Session* lpConnection,  __in DWORD dwSize, __deref_in char* pRecvedMsg);
	
	// client에서 packet이 도착했을 때 순서 성 없이 곧바로 처리 되는 패킷처리
	virtual	bool	OnRecvImmediately(__deref_in CQNetLib::Session* pConnection, __in DWORD dwSize, __deref_in char* pRecvedMsg);
	
	// client와 연결이 종료되었을 때 호출되는 함수
	virtual	void	OnClose(__deref_in CQNetLib::Session* pConnection);
	
	virtual bool	OnSystemMsg( __deref_in void* pOwnerKey, __in DWORD dwMsgType, __in LPARAM lParam );


	// 서버 시작
	virtual bool	ServerStart();
	// INI에서 서버 연결 정보를 읽어와 서버 연결을 생성한다.
	bool			CreateServerConnection();

	// 틱쓰레드 포인터 반환
	inline TickThread*     GetTickThread() { return m_pTickThread; }
	inline DWORD			GetServerTick() { return m_pTickThread->GetTickCount(); }
	
	// 패킷처리 함수 설정
	void			InitProcessFunc();
		
	// 고유한 키를 얻어낸다.
	DWORD			GeneratePrivateKey() { return ++m_dwPrivateKey; }

	bool			ReadGameServerInfo( __inout CQNetLib::INITCONFIG& initConfig );

	// 서비스 모드 설정
	void			SetIsService( __in bool bIsService ) { m_bIsService = bIsService; }
	bool			GetIsService() { return m_bIsService; }

	// 패킷처리 쓰레드에 패킷을 등록한다.
	void			ProcessSystemMsg( void* pOwnerKey , DWORD dwMsgType , LPARAM lParam );




	// 게임서버의 정보를 저장한다.
	void AddGameServerInfo( __in LOGIN_GAMESERVER_INFO tGameServerInfo );
	// 게임서버 정보를 제거한다.
	void RemoveGameServerInfo( __in INT32  iConnectionIndex );
	
	// 게임서버 인덱스로 연결 중인 게임서버 컨넥트 반환
	CQNetLib::Session* GetGameServerConnection( __in INT32  iGameServerIndex );
	// 게임서버는 연결 중인가?
	bool	IsConnectedGameServer( LOGIN_GAMESERVER_INFO& tGameServer );

	// 유저의 게임서버 인증 완료 및 인증만료 처리를 한다.
	void SetUserGameServerAuth( __in INT32  iConnectIndex, __in bool bLogIn, __in UINT64 i64UKey, __in UINT64 i64CharCd, 
													__in short sGameServerIndex );

	// 게임 서버 채널의 유저 수
	void ChannelUserCount( __in short sGameServerIndex, __in short sChannelNum, __in short UserCount );

	// 채널의 정보를 클라이언트에 통보 한다.
	void SendChannelInfo( __in short PacketInxex, __deref_in CQNetLib::Session* pkConnection );
	// 채널의 정보를 게임서버에 통보 한다.
	void SendChannelInfo( __deref_in CQNetLib::Session* pkConnection, __in UINT64 i64CharCd, __in short sCurChannelNum, __in short sSeqCdInLobby );

	// 현재 시간을 설정한다.   * cTickThread에서 지정된 시간마다 시간을 갱신한다.
	void SetCurTime();
	// 현재 시간 반환
	SYSTEMTIME* GetCurTime() { return &m_tCurTime; }

	// 현재 시간을 초 단위로 반환
	UINT32  GetCurSecondTime() { time_t CurTime;	time( &CurTime );	return static_cast<UINT32 >( CurTime ); }

	// 인증된 유저가 다른 게임서버에 접속 시작 시간을 설정
	void SetAuthUserChangeGameServerSecondTime( __in UINT64 i64UKey );
	


private:
	char m_szLogFileName[ MAX_FILENAME ];
	
	//틱 쓰레드
	TickThread*			m_pTickThread;
	
	//패킷 처리함수 포인터
	FuncProcess				m_FuncProcessClient[ MAX_PROCESSFUNC ];
	FuncProcess				m_FuncProcessServer[ MAX_PROCESSFUNC ];

	//패킷 사이즈를 가지고 있다. 비교
	INT32 						m_nClientPacketSizes[ MAX_PROCESSFUNC ];		

	DWORD					m_dwPrivateKey;

	DWORD					m_nMaxGameServerConnCnt;
	DWORD					m_nMaxConnectionCnt;

	SYSTEMTIME				m_tCurTime;
	
	//윈도우서비스모드
	bool					m_bIsService;	

	// 게임서버 정보 관리
	std::vector<LOGIN_GAMESERVER_INFO> m_GameServerInfos;

	CQNetLib::ServerConnectionManager m_ServerConnectionManager;
};

#define g_pLoginIocpServer				LoginIocpServer::GetSingleton()

