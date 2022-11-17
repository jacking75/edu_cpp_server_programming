#include <string>
#include <ace/Singleton.h>
#include "HalfNetworkType.h"
#include "ReactorPool.h"

namespace HalfNetwork
{
	/////////////////////////////////////////
	// ReactorTask
	/////////////////////////////////////////
	ReactorTask::ReactorTask()
	{
		this->reactor(ACE_Reactor::instance());
	}

	ReactorTask::~ReactorTask()
	{
		this->reactor(NULL);
	}

	bool ReactorTask::Open(uint8 poolSize)
	{
		int properPoolSize = poolSize;
		if (0 == properPoolSize)
			properPoolSize = 1;
		m_poolSize = properPoolSize;
		return (-1 != this->activate(THR_NEW_LWP|THR_JOINABLE, m_poolSize));
	}

	void ReactorTask::Close()
	{
		ACE_Reactor::instance()->end_reactor_event_loop();
		this->wait();
		ACE_Reactor::instance()->reset_reactor_event_loop();
	}

	uint8 ReactorTask::GetPoolSize()
	{
		return m_poolSize;
	}

	int ReactorTask::svc()
	{
		ACE_Reactor::instance()->owner(ACE_Thread::self());
		ACE_Reactor::instance()->run_reactor_event_loop();
		return 0;
	}

	/////////////////////////////////////////
	// ReactorPool
	/////////////////////////////////////////
	bool ReactorPool::Open(uint8 poolSize)
	{
		return _task.Open(poolSize);
	}

	void ReactorPool::Close()
	{
		_task.Close();
	}

	uint8 ReactorPool::GetPoolSize()
	{
		return _task.GetPoolSize();
	}

} // namespace HalfNetwork