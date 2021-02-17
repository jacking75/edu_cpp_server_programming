#include "stdafx.h"
#include "ThreadRegister.h"

template<typename T>
bool CIOCPThreadRegister<T>::RegisterThreadFunc(int nThreadCount_, HANDLE hCompletionPort_, _beginthreadex_proc_type threadFunc_)
{
	m_registedThreadCount = nThreadCount_;
	m_bIsRunning = true;

	HANDLE *pEvents = new HANDLE[nThreadCount_];
	_StWaitingEvent* waitingEvent = new _StWaitingEvent[nThreadCount_];

	for (int i = 0; i < nThreadCount_; ++i)
	{
		/// 쓰래드 생성
		waitingEvent[i].hIOCompltePort = hCompletionPort_;
		waitingEvent[i].hWaitingEvent = ::WSACreateEvent();
		waitingEvent[i].hThread = (HANDLE)_beginthreadex(NULL, 0, threadFunc_, reinterpret_cast<LPVOID>(&waitingEvent[i]), 0, &waitingEvent[i].threadID);
		waitingEvent[i].pCaller = static_cast<T*>(this);
		pEvents[i] = waitingEvent[i].hWaitingEvent;

		// 로그
	}

	/// 생성된 쓰레드가 모두 만들어 질 때까지 대기
	::WSAWaitForMultipleEvents(nThreadCount_, pEvents, TRUE, INFINITE, FALSE);
	return true;
}


template<typename T>
bool CIOCPThreadRegister<T>::RegisterThreadFunc(VEC_IOCP_HANDER& hCompletionPorts_, _beginthreadex_proc_type threadFunc_)
{
	int nThreadCount = static_cast<int>(hCompletionPorts_.size());
	if (nThreadCount <= 0)
	{
		// 로그
		return false;
	}

	m_registedThreadCount = nThreadCount;
	m_bIsRunning = true;

	HANDLE *pEvents = new HANDLE[nThreadCount];
	_StWaitingEvent* waitingEvent = new _StWaitingEvent[nThreadCount];

	for (int i = 0; i < nThreadCount; ++i)
	{
		/// 쓰래드 생성
		waitingEvent[i].hIOCompltePort = hCompletionPorts_[i];
		waitingEvent[i].hWaitingEvent = ::WSACreateEvent();
		waitingEvent[i].hThread = (HANDLE)_beginthreadex(NULL, 0, threadFunc_, reinterpret_cast<LPVOID>(&waitingEvent[i]), 0, &waitingEvent[i].threadID);
		waitingEvent[i].threadRegion = i;
		waitingEvent[i].pCaller = static_cast<T*>(this);
		pEvents[i] = waitingEvent[i].hWaitingEvent;

		// 로그
	}

	/// 생성된 쓰레드가 모두 만들어 질 때까지 대기
	::WSAWaitForMultipleEvents(nThreadCount, pEvents, TRUE, INFINITE, FALSE);
	return true;
}


template<typename T>
bool CIOCPThreadRegister<T>::IsRunning() const
{
	return m_bIsRunning;
}


template <typename T>
CIOCPThreadRegister<T>::CIOCPThreadRegister()
{
}


template <typename T>
CIOCPThreadRegister<T>::~CIOCPThreadRegister()
{
	m_bIsRunning = false;
}



template <typename T_DERIVED, typename T_BASETASK>
bool CTaskThreadRegister<T_DERIVED, T_BASETASK>::RegisterThreadFunc(VEC_THREAD_MESSAGE& messages_, _beginthreadex_proc_type threadFunc_)
{
	int nThreadCount = static_cast<int>(messages_.size());
	if (nThreadCount <= 0)
	{
		// 로그
		return false;
	}

	m_registedThreadCount = nThreadCount;
	m_bIsRunning = true;

	HANDLE *pEvents = new HANDLE[nThreadCount];
	_StWaitingEvent* waitingEvent = new _StWaitingEvent[nThreadCount];

	for (int i = 0; i < nThreadCount; ++i)
	{
		/// 쓰래드 생성
		waitingEvent[i].hWaitingEvent = ::WSACreateEvent();
		waitingEvent[i].message = messages_[i];
		waitingEvent[i].hThread = (HANDLE)_beginthreadex(NULL, 0, threadFunc_, reinterpret_cast<LPVOID>(&waitingEvent[i]), 0, &waitingEvent[i].threadID);
		waitingEvent[i].pCaller = static_cast<T_DERIVED*>(this);
		pEvents[i] = waitingEvent[i].hWaitingEvent;

		// 로그
	}

	/// 생성된 쓰레드가 모두 만들어 질 때까지 대기
	::WSAWaitForMultipleEvents(nThreadCount, pEvents, TRUE, INFINITE, FALSE);

	return true;
}


template <typename T_DERIVED, typename T_BASETASK>
void CTaskThreadRegister<T_DERIVED, T_BASETASK>::PushTask(int index_, T_BASETASK* task_)
{
	if (task_ == NULL)
	{
		// 로그
		return;
	}

	/// 해당 인덱스에 맞는 queue의 정보 전달
	SThreadMessage<T_BASETASK>* message = m_vecThreadMessage[index_];
	message->queue.push(task_);
	::WSASetEvent(message->waitingEvent);
}


template <typename T_DERIVED, typename T_BASETASK>
void CTaskThreadRegister<T_DERIVED, T_BASETASK>::FlushAllMessage()
{
	// don't call this
}


template <typename T_DERIVED, typename T_BASETASK>
bool CTaskThreadRegister<T_DERIVED, T_BASETASK>::IsRunning() const
{
	return m_bIsRunning;
}


template <typename T_DERIVED, typename T_BASETASK>
CTaskThreadRegister<T_DERIVED, T_BASETASK>::CTaskThreadRegister()
{
}


template <typename T_DERIVED, typename T_BASETASK>
CTaskThreadRegister<T_DERIVED, T_BASETASK>::~CTaskThreadRegister()
{
}

