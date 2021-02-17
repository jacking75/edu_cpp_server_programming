#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <process.h>
#include "list"
#include "SessionManager.h"
#include "ThreadRegister.h"


class CAttachModule;
class CSender;

///////////////////////////////////////////////////////////////////////////
// Transfer Class
// (Send Class, 종속 서버)
///////////////////////////////////////////////////////////////////////////
class CTransfer :
	public CIOCPThreadRegister<CTransfer>
{
public:
	typedef std::list<CAttachModule*> ATTACH_MODULE_LIST;

private:
	ATTACH_MODULE_LIST m_moduleList;     ///< 서버 모듈 리스트(세션의 접속, 종료, 메시지 보내기, 메시지 받기시에 동작되는 모듈)
	VEC_IOCP_HANDER m_vecWorkerHandle;   ///< transfer 관련 IOCP 핸들 백터
	CServerBase* m_pServer;              ///< 종속되는 서버
	CSender*  m_pSender;                 ///< send class 포인터
	int m_nRecvThreadCount;              ///< 워크 쓰레드 개수(Recv Thread 개수)

private:
	/// attach된 모듈 중 recv에 관련된 것들을 모두 수행한다
	void RunRecvHandler(CSession* session_, DWORD bytes_);

	/// attach된 모듈 중 send에 관련된 것들을 모두 수행한다
	void RunSendHandler(CSession* session_, DWORD bytes_);

	/// attach된 모듈 중 sessionClose에 관련된 것들을 모두 수행한다
	void RunCloseHandler(CSession* session_, int reason_);

public:
	/// Transfer Thread 등록 및 시작
	bool Start(HANDLE hCompletionPort_, int nTransperThreadCount_, VEC_IOCP_HANDER& hWorkerHanders_);

	/// 접속, 세션종료, 보내기, 받기 등이 일어났을 때에 대한 모듈을 등록
	bool AttachModule(CAttachModule* module_);

	/// 쓰레드 종료
	void Close();

	/// 쓰레드 지역 아이디에 대한 IOCP 핸들을 얻어온다
	HANDLE GetWorkThreadHandle(THREAD_REGION threadRegion_);

	/// send class을 얻어온다
	CSender* GetSender() const;

	/// 종속되는 서버를 가져온다
	CServerBase* GetServer();

	/// 패킷 recv, send의 패킷 transfer를 처리 프로세스
	static unsigned CALLBACK TransferProc(void* data_);

public:
	CTransfer(CSender* sender_, CServerBase* server_);
	virtual ~CTransfer();
};