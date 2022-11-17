#ifndef __reactorpool_h__
#define __reactorpool_h__

#pragma once

#include <ace/Reactor.h>
#include <ace/Task.h>
#include "AbstractEventPool.h"

namespace HalfNetwork
{
	/////////////////////////////////////////////////////////
	// Description:
	// Reactor EventDispatcher Pool
	/////////////////////////////////////////////////////////
	class ReactorTask : public ACE_Task_Base
	{
	public:
		ReactorTask();
		~ReactorTask();

	public:
		/////////////////////
		// Description:
		// Create Thread 
		/////////////////////
		bool	Open(uint8 poolSize);

		/////////////////////
		// Description:
		// Terminate Thread 
		/////////////////////
		void	Close();

		uint8	GetPoolSize();

	public:
		int		svc();

	private:
		uint8	m_poolSize;
	};

	////////////////////////////////////////////////////////////
	// Description:
	// ReactorPool interface
	////////////////////////////////////////////////////////////
	class ReactorPool : public AbstractEventPool
	{
	public:
		virtual bool	Open(uint8 poolSize);
		virtual void	Close();
		virtual uint8	GetPoolSize();

	private:
		ReactorTask	_task;
	};

} // namespace HalfNetwork

#endif // __reactorpool_h__