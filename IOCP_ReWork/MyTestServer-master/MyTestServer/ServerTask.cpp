#include "ServerTask.h"
#include "Session.h"

bool CServerTask::Execute(ZONE_MAP const& zoneMap)
{
	// Don't call this
	return false;
}


THREAD_REGION CServerTask::GetThreadRegion() const
{
	return INVALID_REGION_ID;
}


CServerTask::CServerTask(E_TYPE type_, size_t executeCnt_)
	: m_expireCount(executeCnt_),
	m_zoneID(INVALID_ZONE_ID),
	m_sessionID(INVALID_SESSION_ID),
	m_type(type_)
{
}


CServerTask::CServerTask(E_TYPE type_, ZONE_ID id_)
	: m_expireCount(1),
	m_zoneID(id_),
	m_sessionID(INVALID_SESSION_ID),
	m_type(type_)
{
}


CServerTask::CServerTask(E_TYPE type_, CSession const* session_)
	: m_expireCount(1),
	m_type(type_)
{
	if (session_)
	{
		m_zoneID = session_->GetZoneID();
		m_sessionID = session_->GetSessionID();
	}
	else
	{
		// ·Î±×
	}
}


CServerTask::~CServerTask()
{
}

