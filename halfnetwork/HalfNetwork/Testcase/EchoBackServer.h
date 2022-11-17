#pragma once

#include "ThreadPool.h"

namespace HalfNetwork
{
	class NetworkFacade;
	class EchoBackServer : public ACE_Task_Base
	{
	public:
		EchoBackServer(NetworkFacade* manager);
		virtual int svc();
		void Terminate();

	public:
		void	RecvQueueID(unsigned int id);
		void	SendQueueID(unsigned int id);

	protected:
		bool Update();

	private:
		unsigned int		_recv_queue_id;
		NetworkFacade*		_manager;
	};
}