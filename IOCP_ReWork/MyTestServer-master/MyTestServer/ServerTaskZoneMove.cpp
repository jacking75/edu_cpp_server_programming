#include "ServerTaskZoneMove.h"
#include "ZoneManager.h"
#include "PC.h"
#include "Serverbase.h"
#include "sample.pb.h"

THREAD_REGION CServerTaskZoneMove::GetThreadRegion() const
{
	return CZoneManager::GetInstance().GetThreadRegion(m_zoneID);
}


bool CServerTaskZoneMove::Execute(ZONE_MAP const& zoneMap)
{
	CSession* session = CZoneManager::GetInstance().GetServer()->FindSession(m_sessionID);
	if (session == NULL)
	{
		// 로그
		return false;
	}

	/// 뭉쳐 있던 패킷들을 다시 처리
	session->SetTranslateMessageState(true);

	if (!CZoneManager::GetInstance().EnterZone(m_zoneID, session->GetPlayer()))
	{
		// 로그
		return false;
	}
	return true;
}


CServerTaskZoneMove::CServerTaskZoneMove(CSession const* session_, ZONE_ID zoneid_)
	: CServerTask(E_TYPE::SESSION_TASK_TYPE, session_)
{
	CSession* session = const_cast<CSession*>(session_);
	if (session == NULL)
	{
		// 로그
		return;
	}

	if (!CZoneManager::GetInstance().LeaveZone(session->GetZoneID(), session->GetPlayer()))
	{
		// 로그
		return;
	}

	m_zoneID = zoneid_;

	/// zone 이동 직후에 뭉쳐서 들어온 메시지가 이전 zone에서 처리되지 않도록 함 (예 : 존이동 + 상점거래 패킷이 뭉쳐왔을 때)
	session->SetTranslateMessageState(false);

	/// 앞으로 recv를 받을 쓰레드 지역 세팅
	session->SetThreadRegion(CZoneManager::GetInstance().GetThreadRegion(zoneid_));
}


CServerTaskZoneMove::~CServerTaskZoneMove()
{
}
