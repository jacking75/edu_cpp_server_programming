#pragma once

#include <process.h>
#include <winsock2.h>
#include "tbb/concurrent_queue.h"
#include "vector"
#include "VirtualPoolObject.h"
#include "Define.h"


typedef std::vector<HANDLE> VEC_IOCP_HANDER;
template <typename T>
struct SThreadMessage
{
	typedef tbb::concurrent_queue<T*> THREAD_QUEUE;       
	THREAD_QUEUE queue;            ///< Task Thread용 taskQueue
	HANDLE       waitingEvent;     ///< taskThread의 다음 작업을 기다리기 위한 핸들

	SThreadMessage()
	{
		waitingEvent = ::WSACreateEvent();
	}
};


///////////////////////////////////////////////////////////
// IOCP 쓰레드 등록
// <Derived class>
///////////////////////////////////////////////////////////
template <typename T>
class CIOCPThreadRegister
{
protected:
	struct _StWaitingEvent
	{
		HANDLE hWaitingEvent;      ///< thread의 초기화가 완료될 때까지 기다리기 위한 핸들
		HANDLE hIOCompltePort;     ///< IOCP핸들
		HANDLE hThread;            ///< thread의 핸들
		unsigned threadID;         ///< thread의 ID
		unsigned threadRegion;     ///< 쓰레드 별 처리를 위한 지역 번호
		T* pCaller;                ///< 쓰레드가 만들어지는 시점에서의 this포인터
	};

protected:
	volatile bool m_bIsRunning;    ///< 쓰레드의 동작 유무
	int  m_registedThreadCount;    ///< 등록된 쓰레드 수

public:
	/// 하나의 iocp 핸들에 대한 여러개의 쓰레드 처리 등록 함수
	/// 쓰레드 함수 등록(하나의 iocp에 대한 쓰레드 수, iocp핸들, 등록함수)
	virtual bool RegisterThreadFunc(int nThreadCount_, HANDLE hCompletionPort_, _beginthreadex_proc_type threadFunc_);

	/// 여러개의 iocp 핸들에 대한 같은 수의 쓰레드 처리 등록 함수
	/// 쓰레드 함수 등록(복수개의 ioco 핸들, 등록함수)
	virtual bool RegisterThreadFunc(VEC_IOCP_HANDER& hCompletionPorts_, _beginthreadex_proc_type threadFunc_);

	/// 등록된 쓰레드의 동작 유무
	virtual bool IsRunning() const;

public:
	CIOCPThreadRegister();
	virtual ~CIOCPThreadRegister();
};


///////////////////////////////////////////////////////////
// 태스크 쓰레드 등록
// <Derived class, TaskQueue Object>
///////////////////////////////////////////////////////////
template <typename T_DERIVED, typename T_BASETASK>
class CTaskThreadRegister
{
protected:
	typedef std::vector<SThreadMessage<T_BASETASK>*> VEC_THREAD_MESSAGE;

	struct _StWaitingEvent
	{
		HANDLE hWaitingEvent;				  ///< thread의 초기화가 완료될 때까지 기다리기 위한 핸들
		HANDLE hThread;						  ///< thread의 핸들
		unsigned threadID;                    ///< thread의 ID
		SThreadMessage<T_BASETASK>* message;  ///< task 메시지 큐
		T_DERIVED* pCaller;                   ///< 쓰레드가 만들어지는 시점에서의 this포인터
	};

protected:
	volatile bool m_bIsRunning;		        ///< 쓰레드의 동작 유무
	int  m_registedThreadCount;		        ///< 등록된 쓰레드 수
	VEC_THREAD_MESSAGE m_vecThreadMessage;  ///< 각 쓰레드에 대한 task 메시지 백터

public:
	/// 쓰레드 등록 함수(쓰레드 별 task 큐, 등록 함수)
	virtual bool RegisterThreadFunc(VEC_THREAD_MESSAGE& messages_, _beginthreadex_proc_type threadFunc_);

	/// task 추가
	void PushTask(int index_, T_BASETASK* task_);

	/// 모든 task에 대한 flush처리
	virtual void FlushAllMessage();

	/// 등록된 쓰레드의 동작 유무
	virtual bool IsRunning() const;

public:
	CTaskThreadRegister();
	virtual ~CTaskThreadRegister();
};


