#ifndef __messagequeuerepository_h__
#define __messagequeuerepository_h__

#pragma once

#include <ace/Message_Queue.h>
#include <ace/Thread_Manager.h>
#include <ace/Map_Manager.h>
#include "MessageQueue.h"

namespace HalfNetwork
{
	/////////////////////////////////////
	// Description:
	//   MessageQueue Repository
	/////////////////////////////////////
	class MessageQueueRepository
	{
	public:
		typedef ACE_Map_Manager<uint8, MessageQueue*, ACE_Thread_Mutex> MessageQueueMap;

	public:
		MessageQueueRepository();
		virtual ~MessageQueueRepository() {}

	public:
		bool	Open();
		void	Close();

	public:
		/////////////////////////////////////////////////////
		// Description:
		//   CreateQueue
		//   should CreateQueue once before use(push, pop)
		/////////////////////////////////////////////////////
		bool	CreateQueue(uint8 id);

	public:
		bool	Push(uint8 queId, 
						EMessageHeader command, 
						uint32 serial, 
						ACE_Message_Block* block);

		bool	Pop(uint8 queId, ACE_Message_Block** block, int timeout);
		bool	Pop(ACE_Message_Block** block, int timeout);

		bool	PopAll(uint8 que_id, ACE_Message_Block** block, int timeout);
		bool	PopAll(ACE_Message_Block** block, int timeout);
		
	public:
		bool	ExistQueueID(uint8 queId) const;
		void	SuspendPush();
		void	Dump();
		void	Pulse(const uint8 queId) const;
		void	PulseAll();

	protected:
		void	ClearAll();
		void	DeleteAll();
		bool	PopAllImpl(ACE_Message_Block** block);
		bool	IsAllEmpty();

	protected:
		MessageQueue*	GetQueue(const uint8 id) const;

	private:
		MessageQueueMap		m_queueContainer;
		ACE_Thread_Mutex	m_popLock;
		ACE_Atomic_Op<ACE_Thread_Mutex, int32>	m_popProgress;
		ACE_Atomic_Op<ACE_Thread_Mutex, int32>	m_suspendPush;
		CustemNotificationStrategy _eventNotiStrategy;
	};

} // namespace HalfNetwork

#endif // __messagequeuerepository_h__