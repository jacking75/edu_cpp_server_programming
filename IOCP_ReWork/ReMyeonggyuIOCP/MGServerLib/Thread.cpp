#include "Thread.h"
#include <process.h>
#include <time.h>

Thread::Thread(Runnable* r)
{
	m_Runner = r;
}

Thread::~Thread()
{
	Join();
	
	if (INVALID_HANDLE_VALUE != m_Handle) {
		CloseHandle(m_Handle);
	}
}

unsigned __stdcall Thread::handleRunner(void* parameter)
{	
	Thread* aThread = reinterpret_cast<Thread*>(parameter);
	
	if (aThread->m_Runner) {
		aThread->m_Runner->Run(aThread);
	} 
	else {
		aThread->Run();
	}

	_endthreadex(0);
	return 0;
}

void Thread::Sleep(long millis)
{
	::Sleep(millis);
}

DWORD Thread::Start()
{
	if (FALSE != m_IsStarted) {
		return ERROR_ALREADY_THREAD;
	}

	if (INVALID_HANDLE_VALUE != m_Handle) {
		CloseHandle(m_Handle);
	}

	m_IsStarted = true;

	m_Handle = (HANDLE)::_beginthreadex(NULL, 0, &Thread::handleRunner, reinterpret_cast<void*>(this), 0, &m_Identifier);
	return 0;
}

bool Thread::WaitFor()
{
	DWORD r = ::WaitForSingleObject(m_Handle, INFINITE);

	if (r == WAIT_OBJECT_0) {
		return true;
	}

	return false;
}
