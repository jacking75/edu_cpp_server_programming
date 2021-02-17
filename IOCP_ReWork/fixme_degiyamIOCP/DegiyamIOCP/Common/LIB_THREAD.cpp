
#include "LIB_THREAD.h"
#include <process.h>

using namespace COMMONLIB;

LIB_THREAD::LIB_THREAD()
{
	m_bRun	= FALSE;
	m_bRun			=	FALSE;
	m_hThreadHandle	=	NULL;
	m_nThreadID		=	0;
}

LIB_THREAD::~LIB_THREAD()
{
	if ( m_hThreadHandle != NULL )
		StopThread();
}

const BOOL LIB_THREAD::StartThread()
{
	m_bRun	=	TRUE;
	
	m_hThreadHandle = (HANDLE)::_beginthreadex(NULL, 0, ThreadFunc, (LPVOID)this, 0, &m_nThreadID);

	if ( m_hThreadHandle == NULL )
	{
		m_bRun = FALSE;
		return FALSE;
	}

	return TRUE;
}

const BOOL LIB_THREAD::StopThread()
{
	if ( m_hThreadHandle != NULL )
	{
		if ( WaitForSingleObject(m_hThreadHandle, THREAD_EXITTIME) == WAIT_TIMEOUT )
		{
			if ( TerminateThread(m_hThreadHandle, 0) )
			{
				m_bRun	=	FALSE;
				CloseHandle(m_hThreadHandle);
				m_hThreadHandle = NULL;
			}
		}
	}
	else
		return FALSE;

	return TRUE;
}

UINT WINAPI LIB_THREAD::ThreadFunc(LPVOID lpParam)
{
	LIB_THREAD* pThis = (LIB_THREAD*)lpParam;
	if ( pThis )
		return pThis->Run();
	else
		return FALSE;

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////

LIB_MULTITHREAD::LIB_MULTITHREAD()
{
	m_bRun	= FALSE;
}

LIB_MULTITHREAD::~LIB_MULTITHREAD()
{
	int nSize = (int)m_vecThread.size();

	for ( int iter = 0; iter < nSize; iter++ )
	{
		LIB_SINGLETHREAD* pThread = m_vecThread[iter];
		if ( pThread )
			delete pThread;
	}

	m_vecThread.clear();
}

const VOID LIB_MULTITHREAD::Initialize(const int nSize, const LPVOID lpParam, const LPTHREADCALLBACK lpFunc)
{
	for ( int iter = 0; iter < nSize; iter++ )
	{
		LIB_SINGLETHREAD* pThread = new LIB_SINGLETHREAD;
		if ( pThread )
		{
			pThread->Initialize(lpParam, lpFunc);
			m_vecThread.push_back(SINGLETHREAD::value_type(pThread));
		}
	}
}

const UINT LIB_MULTITHREAD::StartMultiThread()
{
	m_bRun		= TRUE;
	int nSize	= (int)m_vecThread.size();

	for ( int iter = 0; iter < nSize; iter++ )
	{
		LIB_SINGLETHREAD* pThread = m_vecThread[iter];
		if ( pThread )
			pThread->StartThread();
	}

	return TRUE;
}

const BOOL LIB_MULTITHREAD::StopMultiThread()
{
	m_bRun		= FALSE;
	int nSize	= (int)m_vecThread.size();

	for ( int iter = 0; iter < nSize; iter++ )
	{
		LIB_SINGLETHREAD* pThread = m_vecThread[iter];
		if ( pThread )
		{
			if ( pThread->StopThread() != TRUE )
				return FALSE;
		}
	}

	return TRUE;
}