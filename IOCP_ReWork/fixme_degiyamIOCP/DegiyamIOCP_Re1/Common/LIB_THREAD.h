
#pragma once

#ifndef __LIB_THREAD_H__
#define __LIB_THREAD_H__

#include "Common.h"
#include <vector>

namespace COMMONLIB
{
	enum ENUM_THREAD
	{
		THREAD_EXITTIME	= 100,	// 스레드 종료되는 시간
	};

	/*
	 *	Make a person	: 이재득
	 *	Make a date		: 2007.02.09
	 *	Class name		: LIB_THREAD
	 *	Class describe	: 스레드 클래스
	*/

	class LIB_THREAD
	{
	public :	// Constructor & Destructor
		LIB_THREAD();
		~LIB_THREAD();
	protected :	// Member protected variable
		volatile BOOL	m_bRun;
	private :	// Member private variable
		HANDLE			m_hThreadHandle;
		unsigned		m_nThreadID;
	public :	// Member public function
		virtual const UINT Run() = 0;
        const	BOOL		StartThread();
		const	BOOL		StopThread();
		const	BOOL		IsRun()			{	return m_bRun;	}
		const	unsigned	GetThreadID()	{	return m_nThreadID;	}
	private :	// Member private function
		static	UINT WINAPI	ThreadFunc(LPVOID lpParam);
	};

	typedef const UINT (*LPTHREADCALLBACK)(LPVOID lpParam);

	class LIB_SINGLETHREAD	: public LIB_THREAD
	{
	private :
		LPVOID				m_lpParam;
		LPTHREADCALLBACK	m_lpFunc;
	public :
		const	VOID	Initialize(const LPVOID lpParam, const LPTHREADCALLBACK lpFunc)
		{
			m_lpParam	= lpParam;
			m_lpFunc	= lpFunc;
		}
		const	UINT	Run()				{	return (*m_lpFunc)(m_lpParam);	}
	};

	typedef std::vector<LIB_SINGLETHREAD*>SINGLETHREAD;

	class LIB_MULTITHREAD
	{
	public :
		LIB_MULTITHREAD();
		~LIB_MULTITHREAD();
	private :
		SINGLETHREAD		m_vecThread;
		volatile	BOOL	m_bRun;
	public :
		const	VOID	Initialize(const int nSize, const LPVOID lpParam, const LPTHREADCALLBACK lpFunc);
		const	UINT	StartMultiThread();
		const	BOOL	StopMultiThread();
		const	BOOL	IsRun()		{	return m_bRun;	}
	};
}

#endif	// __LIB_THREAD_H__