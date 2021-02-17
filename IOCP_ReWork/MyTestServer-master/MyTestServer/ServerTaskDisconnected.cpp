#include "ServerTaskDisconnected.h"
#include "PC.h"
#include "ZoneManager.h"
#include "Serverbase.h"

THREAD_REGION CServerTaskDisconnected::GetThreadRegion() const
{
	return CZoneManager::GetInstance().GetThreadRegion(m_zoneID);
}


bool CServerTaskDisconnected::Execute(ZONE_MAP const& zoneMap)
{
	CSession* session = CZoneManager::GetInstance().GetServer()->FindSession(m_sessionID);
	if (session == NULL)
	{
		// 로그
		return false;
	}

	CZoneManager::GetInstance().LeaveZone(m_zoneID, session->GetPlayer());
	session->CloseSocket();

	// 가장 빨리 session을 파괴할 수 있는 시간 : sendQueue가 비워지는 시간 + (dbQueue가 비워지는 시간)
	//                                           sendQueue에 데이터가 남아 있으면 send Thread에서 session에 접근하여 문제 발생
	CZoneManager::GetInstance().GetServer()->PushCacheObject(session, session->GetSessionID(), SESSION_CACHE_RELEASE_TIME);

	return true;
}


CServerTaskDisconnected::CServerTaskDisconnected(CSession const* session_)
	: CServerTask(E_TYPE::SESSION_TASK_TYPE, session_)
{
}


CServerTaskDisconnected::~CServerTaskDisconnected()
{
}
