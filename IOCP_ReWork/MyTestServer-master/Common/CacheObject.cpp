#include "CacheObject.h"
#include "Define.h"
#include <winsock2.h>


void CCacheObject::Init(__int64 key_, __int64 after_)
{
	m_key = key_;
	m_expireTimer = after_ + ::GetTickCount();
	m_bIsCacheWait = true;
}


bool CCacheObject::CheckEnableReleaseCache() const
{
	// Don't call this
	return false;
}

void CCacheObject::SetCacheWaitState(bool is_)
{
	m_bIsCacheWait = is_;
}


CCacheObject::CCacheObject() 
	: m_bIsCacheWait(false),
	m_key(0)
{
}


CCacheObject::~CCacheObject()
{
}