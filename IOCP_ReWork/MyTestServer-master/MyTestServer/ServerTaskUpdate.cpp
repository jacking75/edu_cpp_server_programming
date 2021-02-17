#include "ServerTaskUpdate.h"
#include "Zone.h"


bool CServerTaskUpdate::Execute(ZONE_MAP const& zoneMap)
{
	for (auto it : zoneMap)
	{
		CZone* zone = it.second;
		zone->Upate();
	}
	return true;
}


CServerTaskUpdate::CServerTaskUpdate(size_t executeCnt_)
	: CServerTask(E_TYPE::WORLD_TASK_TYPE, executeCnt_)
{
}


CServerTaskUpdate::~CServerTaskUpdate()
{
}
