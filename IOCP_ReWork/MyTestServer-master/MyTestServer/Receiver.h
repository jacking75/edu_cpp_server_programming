#pragma once

#include "list"
#include "map"
#include "ThreadRegister.h"
#include "PacketHeder.h"
#include "Session.h"
#include <google/protobuf/io/zero_copy_stream_impl_lite.h>

class CMessageContext;
class CAttachModule;
class CServerTask;
class CServerTaskExecutor;

///////////////////////////////////////////////////////////
// Recv Class
// (종속 서버)
///////////////////////////////////////////////////////////
class CReceiver : 
	public CIOCPThreadRegister<CReceiver>
{
public:
	typedef std::map<packetdef::PacketRegion, CMessageContext*> CONTEXT_HANDLER_MAP;
	typedef std::list<CAttachModule*> ATTACH_MODULE_LIST;

private:
	CONTEXT_HANDLER_MAP m_workerHandlerMap;    ///< packet메시지 처리에 대한 핸들러 맵
	ATTACH_MODULE_LIST m_attachModuleList;     ///< 서버 모듈 리스트(세션의 접속, 종료, 메시지 보내기, 메시지 받기시에 동작되는 모듈)
	VEC_IOCP_HANDER  m_vecWorkerHandle;        ///< recv 관련 IOCP 핸들 백터
	CServerBase* m_pServer;                    ///< 종속 서버
	CServerTaskExecutor* m_pTaskExecutor;      ///< ServerTask 수행 클래스 포인터
	int m_nWorkThreadCount;                    ///< 워크 쓰레드 개수(Recv Thread 개수)

private:
	/// 버퍼를 remain버퍼에 복사한다
	bool CopyToRemainBuffer(CSession* session_, const void* buffer_, int size_);

	/// transfer Thread에서 pqcs으로 날아온 패킷 메시지를 해석한다
	bool TranslatePacket(CSession* session_, protobuf::io::CodedInputStream& cs_, int totalsize_);

	/// workerHandler에 등록된 함수를 통해 메시지를 수행한다
	bool RunWorkerHandler(CSession* session_, packetdef::PacketID id_, protobuf::io::ArrayInputStream& io_);

	/// attach된 모듈 중 sessionClose에 관련된 것들을 모두 수행한다
	void RunCloseHandler(CSession* session_, int reason_);

	/// serverTask를 thread 지역번호에 맞추어 수행한다
	void ExecuteTask(THREAD_REGION region_, CServerTask* task);

public:
	/// Recv Thread 등록 및 시작
	bool Start(VEC_IOCP_HANDER& hWorkerHanders_);

	/// packet 메시지 처리를 위한 workerHandler의 역할을 하는 messageContext 등록
	bool AttachWorkerHandler(packetdef::PacketRegion region_, CMessageContext* message_);

	/// 접속, 세션종료, 보내기, 받기 등이 일어났을 때에 대한 모듈을 등록
	bool AttachModule(CAttachModule* attach_);

	/// 서버 테스크를 처리하기 위한 taskExecutor 등록
	bool AttachTaskExecutor(CServerTaskExecutor* attach_);

	/// 쓰레드 종료
	void Close();

	/// 패킷 recv 처리 관련 프로시져 (이곳의 thread가 게임 로직의 work thread와 같다)
	static unsigned CALLBACK RecvProc(void* data_);

	/// 생성자, 소멸자
public:
	CReceiver(CServerBase* server_);
	virtual ~CReceiver();
};