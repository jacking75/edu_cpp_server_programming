#pragma once

#include <process.h>

#include "commonDef.h"


namespace CQNetLib
{	
	//TODO 제거하기
	class Thread
	{
	public:
		Thread(void) {}

		virtual ~Thread(void)
		{
			ReleaseResource();
		}

		bool CreateThread(UINT32 dwWaitTick)			//dwWaitTick이 0이면 WorkerThread로 작동한다.
		{
			UINT32 uiThreadId = 0;


			m_hThread = (HANDLE)_beginthreadex(NULL, 0, &Thread::CallTickThread, this
				, CREATE_SUSPENDED, &uiThreadId);
			if (m_hThread == nullptr)
			{
				return false;
			}
			m_dwWaitTick = dwWaitTick;

			//이벤트를 생성한다.
			if (nullptr == m_hQuitEvent)
				m_hQuitEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

			m_bIsRun = false;
			m_bIsQuit = false;

			return true;

		}

		void TickThread() //일정 Tick마다 스케줄링 된다.
		{
			while (!m_bIsQuit)
			{
				UINT32 dwRet = WaitForSingleObject(m_hQuitEvent, m_dwWaitTick);
				if (WAIT_OBJECT_0 == dwRet)
				{
					if (m_hThread)
					{
						CloseHandle(m_hThread);
						m_hThread = nullptr;
					}
					break;
				}
				else if (WAIT_TIMEOUT == dwRet)
				{
					m_dwTickCount++;

					if (false == m_bIsQuit) {
						OnProcess();
					}

				}
			}
		}
		
		void WorkerThread() //계속 스케줄링 된다.
		{
			while (!m_bIsQuit)
			{
				OnProcess();
			}

		}
		
		virtual bool DestroyThread() //쓰레드를 파괴한다.
		{
			m_bIsQuit = true;

			if (NULL != m_hThread)
			{
				//work thread라면
				if (0 == m_dwWaitTick)
				{
					Thread::Stop();
					Thread::Run();
				}
				//tick thread라면
				else
				{
					if (false == m_bIsRun)
						Thread::Run();
					SetEvent(m_hQuitEvent);
				}
				WaitForSingleObject(m_hThread, INFINITE);
			}

			ReleaseResource();
			return true;

		}
		
		virtual bool Run() //쓰레드를 시작한다.
		{
			if (false == m_bIsRun)
			{
				m_bIsRun = true;
				ResumeThread(m_hThread);
				return true;
			}
			return false;
		}

		virtual bool Stop() //쓰레드를 멈춘다.
		{
			if (true == m_bIsRun)
			{
				m_bIsRun = false;
				SuspendThread(m_hThread);
				return true;
			}

			m_bIsQuit = true;

			return false;
		}

		virtual void					OnProcess() = 0;							//상속받은 클래스에서 처리될 일을 정의한다.

		inline UINT32 					GetTickCount() { return m_dwTickCount; }
		inline UINT32 					GetWaitTick() { return m_dwWaitTick; }
		inline bool						IsRun() { return m_bIsRun; }
		inline bool						IsQuit() { return m_bIsQuit; }

	protected:
		void ReleaseResource() //할당된 자원을 풀어준다.
		{
			m_bIsQuit = true;
			m_bIsRun = false;
			//이벤트 리소스를 제거
			if (m_hQuitEvent)
			{
				CloseHandle(m_hQuitEvent);
				m_hQuitEvent = nullptr;
			}

			//쓰레드 리소스 제거
			if (m_hThread)
			{
				CloseHandle(m_hThread);
				m_hThread = nullptr;
			}
		}


	protected:
		HANDLE							m_hThread = nullptr;
		HANDLE							m_hQuitEvent = nullptr;
		bool							m_bIsRun = false;
		bool							m_bIsQuit = false;
		UINT32							m_dwWaitTick = 0;
		UINT32							m_dwTickCount = 0;

	private:
		static UINT32 WINAPI CallTickThread(LPVOID p)
		{
			Thread* pTickThread = (Thread*)p;

			if (pTickThread->GetWaitTick())
				pTickThread->TickThread();
			else
				pTickThread->WorkerThread();


			return 1;
		}

		SET_NO_COPYING(Thread);
	};


	
}