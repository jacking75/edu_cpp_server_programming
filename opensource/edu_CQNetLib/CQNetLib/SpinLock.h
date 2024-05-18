#pragma once

#define WIN32_LEAN_AND_MEAN  //원래 없음
#include <Windows.h>

namespace CQNetLib
{
	const int SPIN_LOCK_COUNT = 4000;

	class SpinLock
	{
	public:
		SpinLock()
		{
			//실패는 절대 있을 수 없음 !!!
			InitializeCriticalSectionAndSpinCount(&m_csSyncObject, SPIN_LOCK_COUNT);
		}

		~SpinLock() {}

		void Enter()
		{
			::EnterCriticalSection(&m_csSyncObject);
		}

		void Leave()
		{
			::LeaveCriticalSection(&m_csSyncObject);
		}

	private:
		CRITICAL_SECTION m_csSyncObject;
	};


	class SpinLockGuard
	{
	public:
		SpinLockGuard(SpinLock& lock) : m_Lock(lock)
		{
			m_Lock.Enter();
		}

		~SpinLockGuard()
		{
			m_Lock.Leave();
		}

	private:
		SpinLock& m_Lock;
	};
}
