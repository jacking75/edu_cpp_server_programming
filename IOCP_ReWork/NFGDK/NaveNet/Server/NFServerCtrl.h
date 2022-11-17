/** 
 *  @file  		NFServerCtrl.h
 *  @brief 		IOCP Base 컨트롤러 클래스
 *  @remarks 
 *  @author  	강동명(edith2580@gmail.com)
 *  @date  		2009-05-09
 */
#pragma once

#include "NFConnection.h"
#include <vector>
#include <thread>

namespace NaveNetLib {

//	extern INT iMaxRecvPacket;

	typedef struct LISTENER
	{
		SOCKET	s;		// Listen Socket
		INT		nPort;	// 포트
		CHAR	cBindQue;	// 바인드 큐

		void Init()
		{
			s = INVALID_SOCKET;		// Listen Socket
			nPort = 0;	// 포트
			cBindQue = 0;
		}

		void Release()
		{
			// 클로즈 소켓 전에 큐된 데이타를 보낼지 말지 결정하는 옵션
			struct linger li = {1, 0};	// Default: SO_DONTLINGER
			shutdown(s, SD_BOTH);
			setsockopt(s, SOL_SOCKET, SO_LINGER, (CHAR *)&li, sizeof(li));
			closesocket(s );

			Init();
		}
	}*LPLISTENER;

	class NFServerCtrl
	{
	public:
		NFServerCtrl();
		virtual ~NFServerCtrl();

	public:
		SOCKET						CreateListenSocket(INT nServerPort, CHAR cBindQue = 32);		// Listen Socket 생성 
		HANDLE						ConnectIOCPSocket(SOCKET sckListener);		// Listen Socket 을 iocp 소켓에 연결

		inline HANDLE				GetIOCP() { return m_hIOCP; };

		inline bool					IsRun() { return m_bServerRun; };
		inline bool					IsPause() { return m_bPause; };

		//static unsigned __stdcall	WINAPI	Thread_MainEx(LPVOID lpvoid);		// IOCP 구동 메인 Thread
		//static unsigned __stdcall	WINAPI	Process_MainEx(LPVOID lpvoid);		// Process 처리 Thread
		//static unsigned __stdcall	WINAPI	Packet_MainEx(LPVOID lpvoid);		// Packet 처리 Thread
		void Thread_MainEx();		// IOCP 구동 메인 Thread
		void Process_MainEx();		// Process 처리 Thread
		void Packet_MainEx();		// Packet 처리 Thread

		virtual void				Update();									// Process 처리 함수

		virtual void				ShowServerInfo();							// 시작시 화면에 서버 정보 표시

		virtual bool				CreateSession(SOCKET sckListener) { return true; }		// Client Session 을 생성한다.
		virtual bool				ReleaseSession() { return true; }	// Client Session 을 삭제한다.

	protected:

		HANDLE						CreateIOCP();								// IOCP 핸들 생성
		bool						InitSocket();								// 소켓 라이브러리 활성
		INT							GetNumberOfProcess();						// CPU수를 구한다 => 스레드 구동시 이용 
		INT							GetNumberOfThread() { return m_nMaxThreadNum; };	// 생성된 스레드의 개수를 구한다.

		bool						Start(INT nPort, INT nMaxConn, INT nMaxThreadNum = 0);	// 서버 구동
		void						Pause(bool bPause);
		bool						Stop();										// 서버 정지 

		void WSAGetLastErrorToTextMessage(char *pMsg);

	protected:
		INT							m_nMaxThreadNum;							// 최대 스레드 수 결정 
		//HANDLE*						m_pWorkThread;								// IOCP Work 스레드 핸들 
		HANDLE						m_hIOCP;									// IOCP 핸들 

		std::vector<LISTENER>		m_vecListener;								// 리스너 리스트 여러개의 리스너를 생성할 수도 있다.

		//HANDLE						m_hProcThread;								// 프로세싱을 위한 스레드
		//HANDLE						m_hPacketThread;							// PacketUpdate을 위한 스레드

		std::vector<std::thread> m_MainThreadList;
		std::thread m_ProcessThread;
		std::thread m_PacketThread;

		INT							m_iPort;
		INT							m_iMaxConn;

		bool						m_bPause;
		bool						m_bServerRun;								// 서버 시작 변수 실행되면 true변경.

		NFUpdateManager*			m_pUpdateManager;
	};

}