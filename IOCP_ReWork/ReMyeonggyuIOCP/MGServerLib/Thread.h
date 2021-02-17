#pragma once

#include <windows.h>

class Thread;

class Runnable
{
public:
	virtual void Run(Thread* info) = 0;
};

class Thread
{
public:	
	Thread() = default;
	Thread(Runnable* r);
	virtual ~Thread();

	virtual	unsigned int IsStart() { return m_IsStarted;}

	virtual	DWORD Resume() { return ::ResumeThread(m_Handle); }

	virtual void Run() {}

	virtual	DWORD Start();

	virtual	void Sleep(long millis);	

	virtual	void Stop() { m_IsStarted = FALSE; }

	virtual	DWORD Suspend() { return ::SuspendThread(m_Handle); }

	virtual	void Join() { Stop(); }

	virtual	bool WaitFor();


protected:
	static unsigned __stdcall handleRunner(void* parameter);

protected:
	Runnable*				m_Runner = nullptr;
	volatile unsigned int	m_IsStarted = false;
	unsigned		m_Identifier = 0;
	HANDLE			m_Handle = INVALID_HANDLE_VALUE;
};

