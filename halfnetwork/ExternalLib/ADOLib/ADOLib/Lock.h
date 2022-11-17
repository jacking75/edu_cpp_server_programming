#pragma once
#include <Windows.h>

class CriticalSection
{
public:
	CriticalSection(){ InitializeCriticalSection(&cs); }
	~CriticalSection(){ DeleteCriticalSection(&cs); }
	void Lock(){ EnterCriticalSection(&cs); }
	void Unlock(){ LeaveCriticalSection(&cs); }
private:
	CRITICAL_SECTION cs;
};

class ScopedLock
{
public:
	ScopedLock(CriticalSection &cs):m_cs(cs){ m_cs.Lock(); }
	~ScopedLock(){ m_cs.Unlock(); }

private:
	CriticalSection &m_cs;
};