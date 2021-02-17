
#include "Thread.h"
#include <process.h>


CThread::CThread()
{
	m_bRun			= false;
	m_nThreadID		= 0;
	m_hStopEvent	= INVALID_HANDLE_VALUE;
}

CThread::~CThread()
{
	if (m_hStopEvent != INVALID_HANDLE_VALUE) {
		CloseHandle(m_hStopEvent);
		m_hStopEvent = INVALID_HANDLE_VALUE;
	}
}

// 스레드 생성
const bool CThread::StartThread()
{
	m_bRun = true;
	
	m_hStopEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	m_Thread = std::thread{ ThreadFunc, (LPVOID)this };

	//Sleep(1000);

	return true;
}

// 스레드를 정지시킨다.
const bool CThread::StopThread()
{
	if (this->JoinThread() == false) {
		return false;
	}

	m_bRun = false;

	return true;
}

const bool CThread::JoinThread()
{
	if (m_Thread.joinable() == true) {
		m_Thread.join();
		return true;
	}

	return false;
}

// 스레드 함수
void CThread::ThreadFunc(LPVOID lpparam)
{
	CThread* pthis = reinterpret_cast<CThread *>(lpparam);
	if (pthis != nullptr)			
		pthis->Run();			// Sendthread일때 메모리 참조를 할 수 없다 왜?

	return;
}

//////////////////////////////////////////////////////////////////////////

CMultiThread::CMultiThread()
{
	m_bRun	= false;
}

CMultiThread::~CMultiThread()
{
	int nsize = (int)m_vecThread.size();

	for ( int i = 0; i < nsize; i++ )
	{
		CSingleThread* pthread = m_vecThread[i];
		pthread->JoinThread();
		if (pthread)
			delete pthread;
	}

	m_vecThread.clear();
}

VOID CMultiThread::Initialize(const int nsize, const LPVOID lpparam, const LPTHREADCALLBACK lpfunc)
{
	for ( int i = 0; i < nsize; i++ )
	{
		CSingleThread* pthread = new CSingleThread;
		if (pthread)
		{
			pthread->Initialize(lpparam, lpfunc);
			m_vecThread.emplace_back(pthread);
		}
	}
}

const bool CMultiThread::StartMultiThread()
{
	m_bRun = true;

	int nsize = (int)m_vecThread.size();

	for ( int i = 0; i < nsize; i++ )
	{
		CSingleThread* pthread = m_vecThread[i];
     		if (pthread)
			pthread->StartThread();
	}

	return true;
}

const bool CMultiThread::StopMultiThread()
{
	if (this->JoinMultiThread() == false) {
		return false;
	}

	m_bRun = false;

	return true;
}

const bool CMultiThread::JoinMultiThread()
{
	int nsize = (int)m_vecThread.size();

	for (int i = 0; i < nsize; ++i)
	{
		CSingleThread* pthread = m_vecThread[i];

		if (pthread->JoinThread() == false)
			return false;
	}

	return true;
}