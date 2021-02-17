
#pragma once

#ifndef __THREAD_H__
#define __THREAD_H__

#include "Common.h"
#include <vector>

constexpr int THREAD_WAITTIME = 100;		// 스레드 종료시 대기시간

class CThread
{
public :
	CThread();
	~CThread();
protected :	// 스레드의 작동여부 확인을 위해 volatile으로 쓰는것이 좋을거 같다.
	volatile bool	m_bRun;
private :
	std::thread		m_Thread;						// Handle을 C++의 Thread로 대체하자
	UINT			m_nThreadID;					// ID 필요한가?
public:
	HANDLE			m_hStopEvent;
public :
	virtual const UINT		Run() = 0;
    const bool				StartThread();
	const bool				StopThread();
	const bool				JoinThread();
	inline const bool		IsRun()	{
		return m_bRun;	
	}
	inline const UINT		GetThreadID() {
		return m_nThreadID;	
	}
private :	// 스레드 클래스마다 자신의 실행함수를 가지고 실행
	static void ThreadFunc(LPVOID lpparam);
};

// 함수포인터 재정의
typedef const UINT (*LPTHREADCALLBACK)(LPVOID lpparam);

class CSingleThread	: public CThread
{
private :
	LPVOID				m_lpParam{};
	LPTHREADCALLBACK	m_lpFunc{};
public :
	// 실행할 스레드 함수포인터를 지정한다. lpparam 에는 ServerManager의 포인터, lpfunc에는 함수포인터
	inline const VOID Initialize(const LPVOID lpparam, const LPTHREADCALLBACK lpfunc)
	{
   		m_lpParam	= lpparam;
		m_lpFunc	= lpfunc;
	}
	// 스레드 함수 실행
	inline const UINT Run()	{
		return (*m_lpFunc)(m_lpParam);
	}
};

typedef std::vector<CSingleThread*> VECSINGLETHREAD;

// 같은 타입의 스레드를 여러개 관리하기 위한 클래스
class CMultiThread
{
public :
	CMultiThread();
	~CMultiThread();
private :
	VECSINGLETHREAD		m_vecThread;
	volatile BOOL		m_bRun;
public :
	VOID Initialize(const int nsize, const LPVOID lpparam, const LPTHREADCALLBACK lpfunc);
	const bool StartMultiThread();
	const bool StopMultiThread();
	const bool JoinMultiThread();
	inline const bool IsRun() {
		return m_bRun;	
	}
};

#endif	// __THREAD_H__