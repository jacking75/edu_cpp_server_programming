#include "CacheManager.h"
#include "CacheObject.h"
#include "Define.h"
#include "ThreadRegister.hpp"
#include <algorithm>


template<typename T_OBJ>
void CCacheManager<T_OBJ>::PushCacheObject(CCacheObject* obj_, __int64 key_, __int64 milliseconds_)
{
	obj_->Init(key_, milliseconds_);
	PushInList(obj_);	
}


template<typename T_OBJ>
CCacheObject* CCacheManager<T_OBJ>::FindCacheObject(__int64 key_)
{
	return TakeInList(key_);
}


template<typename T_OBJ>
void CCacheManager<T_OBJ>::ReleaseCache(CCacheObject* obj_)
{
	// Don't call this
}


template<typename T_OBJ>
void CCacheManager<T_OBJ>::Join()
{
	m_joinHandle.join();
}


template<typename T_OBJ>
void CCacheManager<T_OBJ>::SetCurrectObject(CCacheObject* obj_)
{
	Concurrency::critical_section::scoped_lock lock(m_cacheQueueLock);
	m_pCurrentObject = obj_;
}


template<typename T_OBJ>
CCacheObject* CCacheManager<T_OBJ>::GetCurrentObject()
{
	Concurrency::critical_section::scoped_lock lock(m_cacheQueueLock);
	return m_pCurrentObject;
}


template<typename T_OBJ>
void CCacheManager<T_OBJ>::FlushAllObejct()
{
	Concurrency::critical_section::scoped_lock lock(m_cacheQueueLock);
	while (!m_queueObj.empty())
	{
		CCacheObject* obj = m_queueObj.front();
		if (obj)
		{
			ReleaseCache(obj);
		}
		m_queueObj.erase(m_queueObj.begin());
	}
	SetSignal();
}


template<typename T_OBJ>
void CCacheManager<T_OBJ>::PushInList(CCacheObject const* obj_)
{
	Concurrency::critical_section::scoped_lock lock(m_cacheQueueLock);
	m_queueObj.push_back(const_cast<CCacheObject*>(obj_));
}


template<typename T_OBJ>
bool CCacheManager<T_OBJ>::PopInList(CCacheObject*& __out obj_)
{
	Concurrency::critical_section::scoped_lock lock(m_cacheQueueLock);
	if (m_queueObj.empty())
	{
		return false;
	}
	obj_ = m_queueObj.front();
	m_queueObj.erase(m_queueObj.begin());
	return true;
}


template<typename T_OBJ>
CCacheObject* CCacheManager<T_OBJ>::TakeInList(__int64 key_)
{
	CCacheObject* result = GetCurrentObject();
	if (result)
	{
		if (result->GetCacheKey() == key_)
		{
			result->SetCacheWaitState(false);
			SetCurrectObject(NULL);
			return result;
		}
	}

	result = NULL;

	{
		Concurrency::critical_section::scoped_lock lock(m_cacheQueueLock);
		m_queueObj.remove_if([key_, &result](CCacheObject* obj) {
			if (obj->GetCacheKey() == key_)
			{
				result = obj;
				result->SetCacheWaitState(false);
				return true;
			}
			return false;
		});
	}

	return result;
}


template<typename T_OBJ>
void CCacheManager<T_OBJ>::WaitForSignal(DWORD milliseconds_)
{	
	::WSAResetEvent(m_hWaitNextObject);
	::WaitForSingleObject(m_hWaitNextObject, milliseconds_);
}


template<typename T_OBJ>
void CCacheManager<T_OBJ>::SetSignal()
{
	::WSASetEvent(m_hWaitNextObject);
}


template<typename T_OBJ>
unsigned CALLBACK CCacheManager<T_OBJ>::CacheProc()
{
	while (_IsRunning())
	{
		CCacheObject* obj = GetCurrentObject();
		if (obj)
		{
			if (!obj->CheckEnableReleaseCache())
			{
				// session을 예로 들면 sendQueue(dbQueue)가 남아있는데 cache 시간이 지난 경우(이럴 일은 없겠지만...)
				/// 얼마를 기다려야 할지 모르는 상황...
				// 로그
				::Sleep(DEFAULT_CHECK_CACHE_TICK);
				continue;
			}

			ReleaseCache(obj);
			SetCurrectObject(NULL);	
		}

		if (!PopInList(obj))
		{
			::Sleep(DEFAULT_CHECK_CACHE_TICK);
			continue;
		}

		DWORD interval = static_cast<DWORD>(obj->GetExpireTimer() - ::GetTickCount());
		interval = (interval > 0) ? interval : 0;
		SetCurrectObject(obj);
		WaitForSignal(interval);
	}

	return 0;
}


template<typename T_OBJ>
CCacheManager<T_OBJ>::CCacheManager()
	: m_pCurrentObject(NULL)
{
	m_bRunning = true;
	m_hWaitNextObject = ::WSACreateEvent();
	m_joinHandle = std::thread(&CCacheManager::CacheProc, this);
}


template<typename T_OBJ>
CCacheManager<T_OBJ>::~CCacheManager()
{
	m_bRunning = false;
	::WSACloseEvent(m_hWaitNextObject);
}
