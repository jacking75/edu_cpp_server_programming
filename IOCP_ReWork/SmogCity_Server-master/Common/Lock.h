
#pragma once

#ifndef __LOCK_H__
#define __LOCK_H__

#include "Common.h"
#include <mutex>

class CLock
{
public :
	CLock()	{}
	virtual ~CLock() {}
public :
	virtual VOID Lock() = 0;
	virtual VOID UnLock() = 0;
};

// CriticalSection을 이용한 Lock 수정할 예정
class CCriticalLock : public CLock
{
public:
	CCriticalLock() {
		InitializeCriticalSection(&m_CS);
	}
	~CCriticalLock() {
		DeleteCriticalSection(&m_CS);
	}
private :
	CRITICAL_SECTION	m_CS;
public :
	inline VOID Lock() {	
		EnterCriticalSection(&m_CS);	
	}
	inline VOID UnLock() {	
		LeaveCriticalSection(&m_CS);
	}
};

class CMutex : public CLock
{
private:
	std::mutex m_mutex;
public:
	inline VOID Lock() {
		m_mutex.lock();
	}
	inline VOID UnLock() {
		m_mutex.unlock();
	}
};

#endif	// __LOCK_H__