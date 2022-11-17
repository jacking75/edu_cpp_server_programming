#include <string>
#include "HalfNetworkType.h"
#include "ProactorPool.h"
#include <ace/OS_NS_unistd.h>
#include <ace/Proactor.h>

namespace HalfNetwork
{
	/////////////////////////////////////////
	// ProactorTask
	/////////////////////////////////////////
	ProactorTask::ProactorTask()
	{
	}

	ProactorTask::~ProactorTask()	
	{
	}

	bool ProactorTask::Open(uint8 poolSize)
	{
		int properPoolSize = poolSize;
		if (0 == properPoolSize)
			properPoolSize = ProperThreadCount();

		this->m_poolSize = properPoolSize;
		return (-1 != this->activate(THR_NEW_LWP|THR_JOINABLE, m_poolSize));
	}

	void ProactorTask::Close()
	{
		ACE_Proactor::instance()->proactor_end_event_loop();
		this->wait();
		ACE_Proactor::instance()->proactor_reset_event_loop();
	}

	int ProactorTask::svc()
	{
		ACE_Proactor::instance()->proactor_run_event_loop();
		return 0;
	}

	uint8 ProactorTask::GetPoolSize()
	{
		return m_poolSize;
	}

	uint8 ProactorTask::ProperThreadCount()
	{
		return 2 * (uint8)ACE_OS::num_processors_online();
	}


	/////////////////////////////////////////
	// ProactorPool
	/////////////////////////////////////////
	bool ProactorPool::Open(uint8 poolSize)
	{
		return m_task.Open(poolSize);
	}

	void ProactorPool::Close()
	{
		m_task.Close();
	}

	uint8 ProactorPool::GetPoolSize()
	{
		return m_task.GetPoolSize();
	}


} // namespace HalfNetwork