#ifndef __tsqueuewithevent_h__
#define __tsqueuewithevent_h__

#pragma once

#include <ace/Containers.h>
#include <ace/Thread_Mutex.h>
#include <ace/Guard_T.h>
#include <ace/Event.h>
#include <ace/Time_Value.h>

namespace HalfNetwork
{
	////////////////////////////////////
	// Description:
	//   Thread Safe Queue with event
	////////////////////////////////////
	template<class T>
	class TSQueueWithEventT
	{
	public:
		TSQueueWithEventT() : _isWorking(false)
		{}

		void Init()
		{
			SetWorkingFlag(true);
		}

		void Fini()
		{
			SetWorkingFlag(false);
			Pulse();
		}

		void Push(T t)
		{
			if (false == IsWorking())
				return;
			ACE_Guard<ACE_Thread_Mutex> guard(_lock);
			_queue.enqueue_tail(t);
			_event.signal();
		}

		/////////////////////////////////////////
		// Description:
		//   when the queue is empty, return false
		/////////////////////////////////////////
		bool Pop(T& t, uint32 ms)
		{
			if (false == IsWorking())
				return false;
			if (0 == Size())
			{
				ACE_Time_Value waitTime(ms/UsecAdjustValue, (ms%UsecAdjustValue)*UsecAdjustValue);
				_event.wait(&waitTime, 0);
			}
			ACE_Guard<ACE_Thread_Mutex> guard(_lock);
			return (-1 != _queue.dequeue_head(t));
		}

		uint32 Size()
		{
			ACE_Guard<ACE_Thread_Mutex> guard(_lock);
			return (uint32)_queue.size();
		}

		void Pulse()
		{
			_event.pulse();
		}

		void SetWorkingFlag(bool f)
		{
			ACE_Guard<ACE_Thread_Mutex> guard(_lock);
			_isWorking = f;
		}

		bool IsWorking()
		{
			ACE_Guard<ACE_Thread_Mutex> guard(_lock);
			return _isWorking;
		}

	protected:
		ACE_Unbounded_Queue<T>	_queue;
		ACE_Thread_Mutex				_lock;
		ACE_Event								_event;
		bool										_isWorking;
	};

} // namespace HalfNetwork


#endif // __tsqueuewithevent_h__