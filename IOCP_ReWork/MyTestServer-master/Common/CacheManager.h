#pragma once

#include <list>
#include <process.h>
#include <winsock2.h>
#include <tbb/atomic.h>
#include <tbb/scalable_allocator.h>
//#include <tbb/concurrent_queue.h>
#include "concrt.h"
#include <thread>

class CCacheObject;

///////////////////////////////////////////////////////////
// 캐시 매니저
///////////////////////////////////////////////////////////
template<typename T_OBJ>
class CCacheManager
{
public:
	//typedef tbb::concurrent_queue<CCacheObject*> CACHE_QUEUE;  //< 꺼내올 방법이 없다...
	typedef std::list<CCacheObject*, tbb::scalable_allocator<T_OBJ>> CACHE_QUEUE;
	
private:
	volatile bool m_bRunning;						///< 쓰레드의 동작 유무
	HANDLE m_hWaitNextObject;						///< 캐시되어 있는 큐에서 다음 차례를 기다리기 위한 핸들러
	tbb::atomic<CCacheObject*> m_pCurrentObject;    ///< 마지막으로 큐에서 읽어온 데이터
	Concurrency::critical_section m_cacheQueueLock; ///< 캐시 큐 관련 lock객체
	std::thread m_joinHandle;


protected:
	CACHE_QUEUE m_queueObj;     ///< 캐시 큐(큐처럼 동작하지만 검색해서 빼는 부분이 있어서 list)

private:
	/// 쓰레드 살아있는 유무
	inline bool _IsRunning() const { return m_bRunning; }

	/// 현재 오브젝트 세팅
	void SetCurrectObject(CCacheObject* obj_);

	/// 밀리세컨드 동안 대기
	void WaitForSignal(DWORD milliseconds_);
	
	/// 대기 상태 해제
	void SetSignal();

	/// 캐시큐의 내용을 모두 flush
	void FlushAllObejct();

	/// 캐시할 오브젝트 추가
	void PushInList(CCacheObject const* obj_);

	/// 캐시할 오브젝트 꺼내옴
	bool PopInList(CCacheObject*& __out obj_);

	/// 캐시키에 맞는 캐시 오브젝트 꺼내옴
	CCacheObject* TakeInList(__int64 key);

	/// 마지막으로 캐시큐에서 읽어온 데이터
	CCacheObject* GetCurrentObject();

public:
	/// 캐시할 오브젝트 추가
	/// (캐시 오브젝트, 캐시 키, 다음까지 기다리는 시간)
	virtual void PushCacheObject(CCacheObject* obj_, __int64 key_, __int64 milliseconds_);
	
	/// 캐시 키에 맞는 오브젝트를 찾아온다
	virtual CCacheObject* FindCacheObject(__int64 key);
	
	/// 캐시 오브젝트 해제 수행
	virtual void ReleaseCache(CCacheObject* obj_);

	/// thread join 관련 수행
	virtual void Join();

	/// 캐시 관련 처리 프로시져
	unsigned CALLBACK CacheProc();

public:
	CCacheManager();
	virtual ~CCacheManager();
};
