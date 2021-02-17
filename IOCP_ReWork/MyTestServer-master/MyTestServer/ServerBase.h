#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <process.h>

#include "vector"
#include "Listener.h"
#include "Transfer.h"
#include "Sender.h"
#include "Receiver.h"
#include "SessionManager.h"
#include "ThreadRegister.h"
#include "PacketHeder.h"
#include "ServerTask.h"
#include <google/protobuf/io/zero_copy_stream_impl_lite.h>

///////////////////////////////////////////////////////////////////////////
// 서버 기본 class
///////////////////////////////////////////////////////////////////////////
class CServerBase :
	public CSessionManager,
	public CTaskThreadRegister<CServerBase, CServerTask>
{
public:
	typedef enum SERVER_TYPE_
	{
		LOGIN_SERVER = 0,
		GAME_SERVER,
	} SERVER_TYPE;

	struct _StInitInfo {
		int nRecvThreadCount;
		int nSendThreadCount;
		int nTransferThreadCount;
		int nListenPortNumber;
		SERVER_TYPE serverType;
	};

private:
	SOCKADDR_IN m_servAddr;										///< 소켓 주소 구조체
	SOCKET m_hServSock;											///< listen 소켓
	HANDLE m_hListenIOPort;										///< listen 관련IOCP 핸들
	VEC_IOCP_HANDER m_vecRecvIOPort;							///< recv 관련 IOCP 핸들 백터
	VEC_IOCP_HANDER m_vecSendIOPort;							///< send 관련 IOCP 핸들 백터

protected:
	_StInitInfo m_serverInfo;           ///< 서버 정보 구조체
	CListener m_listen;                 ///< listen 클래스
	CTransfer m_transfer;               ///< transfer 클래스
	CSender   m_sender;                 ///< send 클래스
	CReceiver m_receiver;               ///< recv 클래스
	bool      m_bInit;                  ///< 초기화 완료 유무
	bool      m_bIsRunning;             ///< 쓰레드 실행중 유무

public:
	/// send 클래스의 레퍼런스를 가지고 온다
	inline CSender& GetSenderRef() { return m_sender; }

	/// recv 쓰레드의 IOCP 핸들 백터를 가지고 온다
	inline VEC_IOCP_HANDER const& GetRecvIOPortHandler() { return m_vecRecvIOPort; }

	/// send 쓰레드의 IOCP 핸들 백터를 가지고 온다
	inline VEC_IOCP_HANDER const& GetSendIOPortHandler() { return m_vecSendIOPort; }

	/// serverTask Thread 등록 및 시작
	virtual bool Start();

	/// 초기화
	virtual void Init(_StInitInfo const& serverInfo_);

	/// 쓰레드 종료
	void Close();

	/// 세션이 닫혔을 때의 처리
	virtual void SessionClose(CSession* session_);

	/// serverTask 처리
	virtual void Process(CServerTask* task_);

	/// serverTask를 모두 flush함
	virtual void FlushAllMessage() override;

	/// serverTask 추가
	void PushTask(CServerTask* task_);

	/// serverTask 처리 프로세스
	static unsigned CALLBACK MessageProc(void* data_);
	
public:
	CServerBase();
	virtual ~CServerBase();
};