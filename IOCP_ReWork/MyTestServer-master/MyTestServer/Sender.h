#pragma once

#include <google/protobuf/message.h>
#include "Session.h"
#include "ThreadRegister.h"
#include "PacketHeder.h"
#include "Define.h"

using namespace google;
using namespace packetdef;

///////////////////////////////////////////////////////////
// Send Class
///////////////////////////////////////////////////////////
class CSender : 
	public CIOCPThreadRegister<CSender>
{
private:
	VEC_IOCP_HANDER m_vecWorkerHandle;  ///< send 관련 IOCP 핸들 백터
	int m_nWorkThreadCount;             ///< 워크 쓰레드 개수(Send Thread 개수)
	
public:
	/// send가 필요한 session을 push 한다(반드시 recv 쓰레드에서 호출해야 한다)
	bool Push(CSession* session);

	/// sessionID에 대한 send의 Workthread 핸들러를 가지고 온다
	HANDLE GetWorkThreadHandle(SESSION_ID sessionId_);

	/// Send Thread 등록 및 시작
	bool Start(VEC_IOCP_HANDER& hWorkerHanders_);

	/// session의 sendQueue 중 첫번째를 send한다
	bool SendPacketUnit(CSession *session_);

	/// session의 sendQueue를 MAX_PACKETSIZE 크기 만큼 모아서 send한다 
	/// (마지막 패킷을 병합하는 중에 최대 MAX_PACKETSIZE * 2가 될 수 있다)
	bool SendPacketUnits(CSession *session_);

	/// 쓰레드 종료
	void Close();
	
	/// 패킷 send 처리 관련 프로시져
	static unsigned CALLBACK SendProc(void* data_);

public:
	CSender();
	virtual ~CSender();
};