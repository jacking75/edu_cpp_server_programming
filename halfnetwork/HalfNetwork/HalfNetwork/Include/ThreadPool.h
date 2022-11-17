#ifndef __threadpool_h__
#define __threadpool_h__

#pragma once

#include <ace/Task.h>
#include <ace/Event.h>

class ACE_Event;

namespace HalfNetwork
{
	////////////////////////////////////////
	// Description: 
	// Easy to create/terminate Thread
	////////////////////////////////////////
	class ThreadPool : public ACE_Task_Base
	{
	public:
		ThreadPool();
		virtual ~ThreadPool();

	public:
		bool	Open(int pool_size);
		void	Close();
		void	EndSignal();
		void	SetWaitTime(uint32 ms);

	public:
		int		svc();

	public:
		virtual bool	Begin() = 0;
		virtual	void	Update() = 0;
		virtual	void	End() = 0;

	protected:
		uint8		_poolSize;
		uint32	_wait_milisecond;

	private:
		ACE_Event*	_event;
	};

} // namespace HalfNetwork

#endif // __threadpool_h__