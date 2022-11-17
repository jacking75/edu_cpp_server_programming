#ifndef __messagequeue_h__
#define __messagequeue_h__

#pragma once

#include <ace/Message_Queue.h>
#include <ace/Thread_Manager.h>
#include <ace/Map_Manager.h>
#include "MessageHeader.h"


class ACE_Event;

namespace HalfNetwork
{
	///////////////////////////////////////////////////////////
	// Description:
	//   similar to ACE_Guard. but it is Interlocked
	///////////////////////////////////////////////////////////
	class InterlockedGuard
	{
	public:
		typedef ACE_Atomic_Op<ACE_Thread_Mutex, int32> InterlockedValue;

	public:
		InterlockedGuard(InterlockedValue& value) : m_inProgress(value)
		{
			m_inProgress = TRUE;
		}
		~InterlockedGuard()
		{
			m_inProgress = FALSE;
		}

	private:
		InterlockedValue&	m_inProgress;
		
	};

	//////////////////////////////////////////////////////////////////////////

	class CustemNotificationStrategy : public ACE_Notification_Strategy
	{
	public:
		CustemNotificationStrategy();
		virtual ~CustemNotificationStrategy();

		virtual int notify (void);
		virtual int notify (ACE_Event_Handler *, ACE_Reactor_Mask mask) { return 0; }

		bool Wait(int32 timeoutMs);

	private:
		ACE_Event* _event;
	};

	///////////////////////////////////////////////////////////
	// Description:
	//   connecting point Network layer and Application layer
	//   MessageQueue role in Half-Sync Half-Asynch pattern
	///////////////////////////////////////////////////////////
	class MessageQueue
	{
	public:
		MessageQueue();
		~MessageQueue();
	public:
		//////////////////////////////////////////////////
		// Arguments:
		//   command : type of Message
		//   serial : Stream ID
		//   block : payload data block
		/////////////////////////////////////////////////
		bool	Push(uint8 queId, EMessageHeader command, 
					uint32 serial, ACE_Message_Block* block);

		/////////////////////////////////////
		// Description:
		//   Dequeue one item
		/////////////////////////////////////
		bool	Pop(ACE_Message_Block** block, int timeout);

		///////////////////////////////////////
		// Description:
		//   Dequeue all item as a linked list
		///////////////////////////////////////
		bool	PopAll(ACE_Message_Block** block, int timeout);
		void	Pulse();
		void	Clear();
		void	Dump();
		bool	IsEmpty();

		void	SetEventNotiStrategy(ACE_Notification_Strategy* );

	public:
		uint32	MessageBytes();
		uint32	MessageLength();
		uint32	MessageCount();

	private:
		ACE_Message_Queue<ACE_MT_SYNCH>			m_queue;
		ACE_Atomic_Op<ACE_Thread_Mutex, int32>	m_popProgress;
	};

} // namespace HalfNetwork

#endif // __messagequeue_h__