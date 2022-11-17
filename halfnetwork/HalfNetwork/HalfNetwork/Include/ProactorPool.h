#ifndef __proactorpool_h__
#define __proactorpool_h__

#pragma once

#include <ace/Task.h>
#include "AbstractEventPool.h"

namespace HalfNetwork
{
	////////////////////////////////////////////////////////////
	// Description:
	// Proactor EventDispatcher Pool(or WorkerThread Pool)
	////////////////////////////////////////////////////////////
	class ProactorTask : public ACE_Task_Base
	{
	public:
		ProactorTask();
		virtual ~ProactorTask();

	public:
		////////////////////
		// Description:
		// Create Thread 
		////////////////////
		bool	Open(uint8 poolSize);

		////////////////////
		// Description:
		// Terminate Thread
		////////////////////
		void	Close();
		uint8	GetPoolSize();

	public:
		int		svc();

	protected:
		uint8	ProperThreadCount();

	private:
		uint8			m_poolSize;
	};

	////////////////////////////////////////////////////////////
	// Description:
	// ProactorPool interface
	////////////////////////////////////////////////////////////
	class ProactorPool : public AbstractEventPool
	{
	public:
		virtual bool	Open(uint8 poolSize);
		virtual void	Close();
		virtual uint8	GetPoolSize();

	private:
		ProactorTask	m_task;
	};

} // namespace HalfNetwork

#endif // __proactorpool_h__