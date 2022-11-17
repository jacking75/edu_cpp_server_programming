#ifndef __tsqueue_h__
#define __tsqueue_h__

#pragma once

#include <ace/Containers.h>
#include <ace/Thread_Mutex.h>
#include <ace/Guard_T.h>

namespace HalfNetwork
{
	/////////////////////////
	// Description:
	//   Thread Safe Queue
	////////////////////////
	template<class T>
	class TSQueueT
	{
	public:
		void Push(T t)
		{
			ACE_Guard<ACE_Thread_Mutex> guard(_lock);
			_queue.enqueue_tail(t);
		}

		/////////////////////////////////////////
		// Description:
		//   when the queue is empty, return false
		/////////////////////////////////////////
		bool Pop(T& t)
		{
			ACE_Guard<ACE_Thread_Mutex> guard(_lock);
			return (-1 != _queue.dequeue_head(t));
		}

		bool Head(T*& t)
		{
			ACE_Guard<ACE_Thread_Mutex> guard(_lock);
			return (-1 != _queue.get(t));
		}

		uint32 Size()
		{
			ACE_Guard<ACE_Thread_Mutex> guard(_lock);
			return _queue.size();
		}

	protected:
		ACE_Unbounded_Queue<T>	_queue;
		ACE_Thread_Mutex				_lock;
	};
} // namespace HalfNetwork


#endif // __tsqueue_h__