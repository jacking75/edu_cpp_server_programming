#pragma once


#include "../../CQNetLib/Delegate.h"
#include "../../CQNetLib/thread.h"

#include "LoginServerDefine.h"


class TickThread : public CQNetLib::Thread
{
	const INT32 CHECKTIME_CUR_DATE = 7200;	// 1시간 마다 현재 시간을 설정한다.

	const INT32  AUTOTIME_CONNECT_SERVER = 4; // 자동으로 다른 서버에 연결 할때까지의 대기 시간
	const INT32  AUTOTIME_LOADING_DATA = 10; // 자동으로 데이터 로딩을 할때까지의 대기 시간
	const INT32  AUTH_USER_CHECK = 1; // 인증자 오류가 없는지 체크 하는 시간

	const short SERVER_KIND = CQNetLib::CT_LOGINSERVER;

public:
	TickThread(void)
	{
	}

	virtual ~TickThread(void)
	{
		Stop();
	}
	
	//TODO 호출하고 있나?
	void SetDelegate(const SA::delegate<void()> OnSetCurTimeFunc, 
					const SA::delegate<void(void*, DWORD, LPARAM)> OnProcessSystemMsgFunc)
	{
		OnSetCurTimeDelegate = OnSetCurTimeFunc;
		OnProcessSystemMsgDelegate = OnProcessSystemMsgFunc;
	}

	// dwWaitTick시간만큼의 간격을 두고 루프를 돈다.
	virtual void OnProcess()
	{
		if ((m_dwTickCount % CHECKTIME_CUR_DATE) == 0)
		{
			OnSetCurTimeDelegate();
		}

		if ((m_dwTickCount % AUTH_USER_CHECK) == 0)
		{
			OnProcessSystemMsgDelegate((void*)1, SYSTEM_CHECK_AUTH_USER, 0);
		}


		AutoServerInit();

		//300초 마다 한번씩 체크한다.
		/*if( ( m_dwTickCount % CHECKTIME_LOGIN ) == 0 )
		{
			g_pUserManager->CheckCloseUser( m_dwTickCount );
		}
		*/
	}

	// 지정된 순서대로 초기화를 한다.
	void AutoServerInit()
	{
		static bool bFinishedAutoProcess = false;
		static bool bAutonConnectServer = false;
		static bool bAutoLoadingData = false;

		if (false == bFinishedAutoProcess)
		{			
			/*
			if( false == bAutonConnectServer && ( m_dwTickCount % AUTOTIME_CONNECT_SERVER ) == 0 )
			{
				bAutonConnectServer = true;
				g_pServerConnectionManager->ConnectToServer( CT_LOGINDBSERVER );
			}
			*/
		}
	}



private:
	SA::delegate<void()> OnSetCurTimeDelegate;
	SA::delegate<void(void*, DWORD, LPARAM)> OnProcessSystemMsgDelegate;
};
