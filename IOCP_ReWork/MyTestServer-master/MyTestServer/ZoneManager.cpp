#include "ZoneManager.h"
#include "Zone.h"
#include "Actor.h"
#include "Session.h"
#include "PC.h"
#include "ServerTask.h"




void CZoneManager::Init(_stInit const& init_)
{
	m_regionCount = init_.regionCount;
	m_pServer = init_.server;
	m_vecZoneRegion.resize(m_regionCount);

	// zone 로딩...
	LoadZone();
}


bool CZoneManager::LoadZone()
{
	// 임시
	AddZone(new CZone(m_pServer, "\\all_tiles_navmesh.bin"));  //< 0
	AddZone(new CZone(m_pServer, "\\all_tiles_navmesh.bin"));  //< 1
	AddZone(new CZone(m_pServer, "\\all_tiles_navmesh.bin"));  //< 2

	return true;
}


bool CZoneManager::AddZone(CZone* zone_)
{
	if (zone_ == NULL)
	{
		// 로그
		return false;
	}

	THREAD_REGION region = GetThreadRegion(zone_->GetZoneID());
	auto re = m_vecZoneRegion[region].insert(std::make_pair(zone_->GetZoneID(), zone_));
	
	return re.second;
}


CZone* CZoneManager::FindZone(ZONE_ID id_)
{
	if (id_ == INVALID_ZONE_ID)
	{
		// 로그
		return NULL;
	}

	ZONE_MAP& zoneMap = m_vecZoneRegion[GetThreadRegion(id_)];

	auto it = zoneMap.find(id_);
	if (it == zoneMap.end())
	{
		return NULL;
	}

	return it->second;
}


bool CZoneManager::DeleteZone(ZONE_ID id_)
{
	if (id_ == INVALID_ZONE_ID)
	{
		// 로그
		return false;
	}

	ZONE_MAP& zoneMap = m_vecZoneRegion[GetThreadRegion(id_)];

	auto it = zoneMap.find(id_);
	if (it == zoneMap.end())
	{
		return false;
	}

	zoneMap.erase(id_);
	return true;
}


bool CZoneManager::EnterZone(ZONE_ID id_, CActor* actor_)
{
	if (actor_ == NULL)
	{
		return false;
	}

	//if (id_ == INIT_ZONE_ID || id_ == INVALID_ZONE_ID)
	if (id_ == INVALID_ZONE_ID)
	{
		// 로그
		return false;
	}
	
	ZONE_MAP& zoneMap = m_vecZoneRegion[GetThreadRegion(id_)];

	auto it = zoneMap.find(id_);
	if (it == zoneMap.end())
	{
		return false;
	}

	if (actor_->GetActorType() == E_ACTOR::PC)
	{
		CPC* pc = static_cast<CPC*>(actor_);
		pc->GetSession()->SetThreadRegion(CZoneManager::GetInstance().GetThreadRegion(id_));
	}
	
	return (*it).second->Enter(actor_);
}


bool CZoneManager::LeaveZone(ZONE_ID id_, CActor* actor_)
{
	if (actor_ == NULL)
	{
		return false;
	}

	//if (id_ == INIT_ZONE_ID || id_ == INVALID_ZONE_ID)
	if (id_ == INVALID_ZONE_ID)
	{
		// 로그
		return false;
	}

	ZONE_MAP& zoneMap = m_vecZoneRegion[GetThreadRegion(id_)];

	auto it = zoneMap.find(id_);
	if (it == zoneMap.end())
	{
		return false;
	}

	return (*it).second->Leave(actor_);
}


void CZoneManager::ExecuteTask(THREAD_REGION region_, CServerTask* task)
{
	if (region_ == INVALID_REGION_ID)
	{
		// 로그
		return;
	}

	ZONE_MAP& zoneMap = m_vecZoneRegion[region_];
	if (!task->Execute(zoneMap))
	{
		// 로그
	}
}


CZoneManager::CZoneManager()
{

}


CZoneManager::~CZoneManager()
{

}
