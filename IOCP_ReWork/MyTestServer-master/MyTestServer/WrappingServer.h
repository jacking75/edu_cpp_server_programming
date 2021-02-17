#pragma once
#include "ServerBase.h"
#include "LoginContext.h"
#include "AttachServerMessage.h"
#include "AdoOleDB.h"
#include <thread>

///////////////////////////////////////////////////////////
// 임시 서버 클래스
///////////////////////////////////////////////////////////
class CWrappingServer :
	public CServerBase
{
private:
	CLoginContext m_loginContext;          ///< packetMessage 처리 context
	CAttachServerMessage m_sessionModule;  ///< 메시지 출력 서버 모듈(세션의 접속, 종료, 메시지 보내기, 메시지 받기시에 동작)
	CAdoOleDB m_db;                        ///< ADB방식 DB처리 객체
	std::thread m_timerThreadHandle;       ///< 타이머 쓰레드 핸들

public:
	/// 서버 시작
	virtual bool Start() override;

	/// 서버 정보 세팅 및 초기화
	virtual void Init(CServerBase::_StInitInfo const& serverInfo_) override;

	/// serverTask에 대한 처리
	virtual void Process(CServerTask* task_) override;

	/// 세션이 닫혔을 때의 처리
	virtual void SessionClose(CSession* session_) override;

	/// thread join 수행
	virtual void Join() override;

	/// 모든 zone에 전달하는 serverTask를 만든다
	template<typename T>
	void CreateWorldTask();

	/// 특정 zoneID에 전달하는 serverTask를 만든다
	template<typename T>
	void CreateZoneTask(ZONE_ID zoneId_);

	/// 특정 session에 전달하는 serverTask를 만든다
	template<typename T>
	void CreateSessionTask(CSession* session_);

public:
	CWrappingServer();
	virtual ~CWrappingServer();
};