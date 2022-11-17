/** 
 *  @file		NFNetClientS.h
 *  @brief		Network Client 싱글스레드 클래스
 *  @remarks	
 *  @author		강동명(edith2580@gmail.com)
 *  @date		2009-04-02
 */

#pragma once

#include "NFIOBuffer.h"

namespace NaveClientNetLib {

	/** 
	 *  @class        NFNetClientS
	 *  @brief        클라이언트에서 서버로 접속하기 위한 Network객체				\r\n
	 *                싱글 스레드용으로 제작된 클래스로 Update에서 패킷Recv,Send처리\r\n
	 *                MainLoop에서 Update함수 호출시 패킷큐가 갱신됨				\r\n
	 *  @remarks      
	 *                
	 *  @par          
	 *  @author  Edith
	 *  @date    2009-04-05
	 */
	class NFNetClientS 
	{
	public:
		/// NFNetClient 생성자
		NFNetClientS();
		/// NFNetClient 소멸자
		virtual ~NFNetClientS();

	public:
		/// 로컬 IP를 얻어옵니다.
		void			GetLocalIP(CHAR* LocalIP);

		/// Connect상태를 얻어옵니다.
		BOOL			IsConnect() { return m_bConnect; }

		/// 패킷을 Send 합니다.
		BOOL			SendPost(NFPacket& Packet);

		/**
		 * @brief		클래스 초기화.
		 * @param szIP	접속할 아이피
		 * @param nPort  접속할 포트
		 * @return			성공여부
		 */
		BOOL			Init(LPCSTR szIP, int nPort);

		/// Disconnect 시킵니다.
		virtual void	Disconnect();

		/// Network를 업데이트 합니다.
		virtual void	Update();

		/// 클라이언트 정지 
		void			Stop();					
		/// 모든 객체를 Close 합니다.
		void			CloseAll();

	private:
		/// 소켓 이벤트 
		virtual void	OnSocketEvent(DWORD dID, DWORD dEvent) {};
		/// 패킷 프로세싱
		virtual void	ProcessPacket(NFPacket* Packet, int PacketLen) { };

		/// 소켓 이벤트 핸들러 
		BOOL			NetworkEventHanlder(LONG lEvent);						

	private:
		/// Event Thread 
		static DWORD WINAPI EventThreadProc(LPVOID lParam);	

		/// Send 이벤트 처리
		VOID	OnSendPacketData();
		/// Recv 이벤트 처리
		VOID	OnReadPacketData();
	
		/// RecvQue의 개수
		int		GetQueCnt();
		/// 첫 패킷 얻어오기
		int		GetPacket(NFPacket** Packet);
		/// 큐에서 패킷을 꺼낸다.
		void	PopPacket();
		
		/// 큐를 업데이트 한다.
		void	UpdateQue();

		/// Connect 소켓이벤트
		BOOL	OnConnect();					
		/// Close 소켓이벤트
		BOOL	OnClose();						

		/// 서버와 Connect를 합니다.
		BOOL	Connect();
		/// 윈속 초기화
		BOOL	WinSockInit();

	private:
		/// 클라이언트 소켓 
		SOCKET				m_hSocket;			
		/// 포트 
		UINT				m_nPort;			
		/// Server IP저장 
		CHAR				m_strIPAddr[32];	

		/// Recv Queue
		NFPacketQueue		m_RecvQueue;		

		/// Send Queue
		NFPacketQueue		m_SendQueue;		

		/// 네트워크 이벤트 핸들러 
		WSAEVENT			m_hEvent;			

		/// Close 된상태인지 
		BOOL				m_bClose;
		/// 접속 상태 플래그 
		BOOL				m_bConnect;			

		/// RecvIO Buffer (완성된 패킷이 저장됨)
		NFPacketIOBuffer		m_RecvIO;
		/// RecvPacket 변수 (완성된 한개의 패킷)
		NFPacket				m_RecvPacket;

		/// Recv Buffer (스레드에서 패킷을 받을때 사용)
		CHAR				m_RecvBuffer[DEF_MAXPACKETSIZE];

		/// Recv 스레드 핸들 
		HANDLE				m_hEventThread;		
	};


}