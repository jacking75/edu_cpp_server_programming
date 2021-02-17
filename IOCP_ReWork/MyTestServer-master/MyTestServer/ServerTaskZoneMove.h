#pragma once
#include "ServerTask.h"
#include "Define.h"


///////////////////////////////////////////////////////////
// 존 이동 서버 태스크
// (세션)
///////////////////////////////////////////////////////////
class CServerTaskZoneMove
	: public CServerTask
{
public:
	/// 수행될 thread의 지역 아이디을 읽어온다
	virtual THREAD_REGION GetThreadRegion() const override;

	/// 수행
	virtual bool Execute(ZONE_MAP const& zoneMap) override;

public:
	CServerTaskZoneMove(CSession const* session_, ZONE_ID zoneid_);
	virtual ~CServerTaskZoneMove();
};
