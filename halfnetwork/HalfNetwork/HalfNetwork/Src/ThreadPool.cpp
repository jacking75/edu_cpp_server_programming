
#include <string>
#include <ace/Singleton.h>
#include "HalfNetworkType.h"
#include "ThreadPool.h"
#include <ace/Event.h>
#include "TimerUtil.h"

namespace HalfNetwork
{
	ThreadPool::ThreadPool() 
		: _wait_milisecond(0)
		, _event(new ACE_Event)
	{
	}

	ThreadPool::~ThreadPool()
	{
		if (NULL != _event)
		{
			delete _event;
			_event = NULL;
		}
	}

	bool ThreadPool::Open(int pool_size)
	{
		if ( false == Begin())
			return false;
		this->_poolSize = pool_size;
		return (-1 != this->activate(THR_NEW_LWP|THR_JOINABLE, pool_size));
	}

	int ThreadPool::svc()
	{
		ACE_Time_Value wait_time;
		ConvertTimeValue(wait_time, _wait_milisecond);
		uint32 loop_counter = 0;
		while(true)
		{
			Update();
			if (0 != ++loop_counter%100)
				continue;
			loop_counter = 0;
			int wait_result = _event->wait(&wait_time, 0);
			if (-1 != wait_result)
				break;
		}
		return 0;
	}

	void ThreadPool::Close()
	{
		EndSignal();
		this->wait();
		End();
	}

	void ThreadPool::EndSignal()
	{
		for(int i=0; i<this->_poolSize; i++)
			this->_event->signal();
	}

	void ThreadPool::SetWaitTime(uint32 ms)
	{
		_wait_milisecond = ms;
	}

} // namespace HalfNetwork