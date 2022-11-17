#include "stdafx.h"
#include "EchoBackServer.h"
#include "NetworkFacade.h"
#include "MessageHeader.h"

namespace HalfNetwork
{

	EchoBackServer::EchoBackServer(NetworkFacade* manager) 
		: _recv_queue_id(-1), _manager(manager)
	{
	}

	bool EchoBackServer::Update()
	{
		ACE_Message_Block* headBlock = NULL;
		ACE_Message_Block* commandBlock = NULL;

		bool receiveData = _manager->PopAllMessage(_recv_queue_id, &headBlock, -1);
		if (false == receiveData)
			return false;

		commandBlock = headBlock;
		while(NULL != commandBlock)
		{
			MessagePostee postee;
			memcpy(&postee, commandBlock->rd_ptr(), sizeof(MessagePostee));
			commandBlock->rd_ptr(sizeof(MessagePostee));
			ACE_Message_Block* payloadBlock = commandBlock->cont();
			if (eMH_Read == postee.command)
				_manager->SendRequest(postee.stream_id, payloadBlock, true);
				//_manager->SendReserve(postee.stream_id, payloadBlock->rd_ptr(), payloadBlock->length(), 100);

			commandBlock = payloadBlock->cont();
		}
		headBlock->release();

		return true;
	}

	void EchoBackServer::RecvQueueID(unsigned int id)
	{
		_recv_queue_id = id;
	}

	int EchoBackServer::svc()
	{
		while(Update())
			;
		return 0;
	}

	void EchoBackServer::Terminate()
	{
		_manager->Pulse(_recv_queue_id);
	}
}