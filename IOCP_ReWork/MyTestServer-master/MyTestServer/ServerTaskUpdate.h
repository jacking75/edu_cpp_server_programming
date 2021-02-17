#pragma once
#include "ServerTask.h"
#include "Define.h"


///////////////////////////////////////////////////////////
// 업데이트 서버 태스크
// (수행되는 쓰레드의 개수)
///////////////////////////////////////////////////////////
class CServerTaskUpdate
	: public CServerTask
{
public:
	/// 수행
	virtual bool Execute(ZONE_MAP const& zoneMap) override;

public:
	CServerTaskUpdate(size_t executeCnt_);
	virtual ~CServerTaskUpdate();
};
