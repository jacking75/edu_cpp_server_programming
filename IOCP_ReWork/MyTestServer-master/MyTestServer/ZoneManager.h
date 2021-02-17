#pragma once
#include "Define.h"
#include "Singleton.h"
#include "ServerTask.h"
#include <map>
#include <vector>

class CServerTask;
class CServerBase;
class CActor;
class CZone;

///////////////////////////////////////////////////////////
// Zone 관리 클래스
///////////////////////////////////////////////////////////
class CZoneManager
	: public CSingleton<CZoneManager>,
	public CServerTaskExecutor
{
public:
	typedef std::vector<ZONE_MAP> THREAD_REGION_VEC;
	struct _stInit
	{
		int regionCount;           ///< thread의 수행 지역 아이디
		CServerBase* server;       ///< zone이 속해 있는 서버
	};

private:
	int m_regionCount;				     ///< thread의 수행 지역 갯수
	THREAD_REGION_VEC m_vecZoneRegion;   ///< thread의 수행 지역에 속해 있는 zone에 대한 백터
	CServerBase* m_pServer;              ///< zone이 속해 있는 서버

public:
	/// 초기화
	void Init(_stInit const& init_);

	/// 존 정보 로딩
	bool LoadZone();

	/// 해당 zone의 thread 수행 지역 아이디를 구한다
	inline THREAD_REGION GetThreadRegion(ZONE_ID id_) const { return id_ % m_regionCount; }

	/// zone이 속해 있는 서버를 가지고 온다
	inline CServerBase* GetServer() { return m_pServer; }

	/// zoneID에 대한 zone을 가지고 온다
	CZone* FindZone(ZONE_ID id_);

	/// zoneID에 맞는 zone을 삭제한다
	bool DeleteZone(ZONE_ID id_);

	/// 해당 zoneID의 zone에 actor을 진입시킨다
	bool EnterZone(ZONE_ID id_, CActor* actor_);

	/// 해당 zoneID의 zone에서 actor를 퇴장시킨다
	bool LeaveZone(ZONE_ID id_, CActor* actor_);

	/// 서버 task를 수행한다
	virtual void ExecuteTask(THREAD_REGION region_, CServerTask* task) override;

private:
	/// 존 정보를 삽인한다
	bool AddZone(CZone* zone_);

public:
	CZoneManager();
	~CZoneManager();
};