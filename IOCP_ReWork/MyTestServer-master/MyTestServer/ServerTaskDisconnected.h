#pragma once

#include "ServerTask.h"
#include "Define.h"


///////////////////////////////////////////////////////////
// 세션 종료 서버 태스크
// (세션)
///////////////////////////////////////////////////////////
class CServerTaskDisconnected
	: public CServerTask
{
public:
	/// 수행될 thread의 지역 아이디을 읽어온다
	virtual THREAD_REGION GetThreadRegion() const override;

	/// 수행
	virtual bool Execute(ZONE_MAP const& zoneMap) override;

public:
	CServerTaskDisconnected(CSession const* session_);
	virtual ~CServerTaskDisconnected();
};
