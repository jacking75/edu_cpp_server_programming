#pragma once
#include <Windows.h>
#include <mutex>

namespace asyncadodblib
{
	const INT32 MAX_SPIN_LOCK_COUNT = 4000;

	class ISynchronizeObj
	{
	public:
		virtual void Lock() = 0;
		virtual void UnLock() = 0;
	};

	
	/// <summary>
	/// Win32 API의 스핀락 크리티컬섹션 </summary>
	class CSSpinLockWin32 : public ISynchronizeObj
	{
	public:
		CSSpinLockWin32()
		{ 
			InitializeCriticalSectionAndSpinCount(&m_lock, MAX_SPIN_LOCK_COUNT); 
		}
		
		~CSSpinLockWin32() 
		{ 
			DeleteCriticalSection(&m_lock); 
		}
		
		virtual void Lock()		{ EnterCriticalSection(&m_lock); }
		virtual void UnLock()	{ LeaveCriticalSection(&m_lock); }
	
	
	private:
		CRITICAL_SECTION m_lock;
	};

	
	/// <summary>
	/// C++11의 뮤텍스 사용(Windows 환경에서는 Win32 API 크리티컬섹션 사용) </summary>
	class StandardLock : public ISynchronizeObj
	{
	public:
		StandardLock()
		{ 
		}
		
		~StandardLock() 
		{ 
		}
		
		virtual void Lock()		{ m_lock.lock(); }
		virtual void UnLock()	{ m_lock.unlock(); }
	
	
	private:
		std::mutex m_lock;
	};

	
	/// <summary>
	/// 락을 객체 생성과 해제에서 자동으로 락과 언락을 하도록 동작 </summary>
	class ScopedLock
	{
	public:
		ScopedLock(ISynchronizeObj &SyncObj):m_SyncObj(SyncObj) 
		{ 
			m_SyncObj.Lock(); 
		}

		~ScopedLock() 
		{ 
			m_SyncObj.UnLock(); 
		}

	private:
		ISynchronizeObj &m_SyncObj;
	};
}