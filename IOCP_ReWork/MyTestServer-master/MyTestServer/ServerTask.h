#pragma once
#include "Define.h"
#include <winsock2.h>
#include <atomic.h>


class CSession;

///////////////////////////////////////////////////////////////////////////
// ServerTask의 기본 클래스
// *월드 단위 생성자(명시적 타입, thread 지역번호의 총 개수)
// *존 단위 생성자(명시적 타입, 존아이디)
// *세션 단위 생성자(명시적타입, 세션)
///////////////////////////////////////////////////////////////////////////
class CServerTask
{
public:
	enum E_TYPE
	{
		WORLD_TASK_TYPE,
		ZONE_TASK_TYPE,
		SESSION_TASK_TYPE,
	};

protected:
	tbb::atomic<size_t> m_expireCount;    ///< threadRegion 수만큼 시작하여 0이 되면 삭제되는 카운트
	ZONE_ID m_zoneID;                     ///< zoneID
	SESSION_ID m_sessionID;               ///< sessionID
	E_TYPE m_type;                        ///< task의 타입

public:
	/// 수행
	virtual bool Execute(ZONE_MAP const& zoneMap);

	/// 수행될 thread 지역 번호를 얻어온다
	virtual THREAD_REGION GetThreadRegion() const;
	
	/// 수행될 zoneID 값을 얻어온다
	inline ZONE_ID GetZoneID() const { return m_zoneID; }

	/// 수행될 sessionID 값을 가지고 온다
	inline SESSION_ID GetSessionID() const { return m_sessionID; }

	/// 해당 태스크의 타입값을 얻어온다
	inline E_TYPE GetTaskType() const { return m_type; }

	/// 수행해야 할 thread 지역을 모두 방문하여 만료가 되었는지 체크한다
	inline bool CheckExepired() { return m_expireCount.fetch_and_decrement() <= 1; }

public:
	explicit CServerTask(E_TYPE type_, size_t executeCnt_);
	explicit CServerTask(E_TYPE type_, ZONE_ID id_);
	explicit CServerTask(E_TYPE type_, CSession const* session_);
	virtual ~CServerTask();
};



///////////////////////////////////////////////////////////////////////////
// ServerTaskExecutor 클래스 (순수 가상함수)
///////////////////////////////////////////////////////////////////////////
class CServerTaskExecutor
{
public:
	virtual void ExecuteTask(THREAD_REGION region_, CServerTask* task) = 0;
};