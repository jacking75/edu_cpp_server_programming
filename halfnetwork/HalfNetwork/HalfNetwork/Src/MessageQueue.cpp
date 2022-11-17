#include <string>
#include "HalfNetworkType.h"
#include "MessageQueue.h"
#include "MessageBlockUtil.h"
#include "NetworkFacade.h"
#include <ace/Event.h>

namespace HalfNetwork
{

	//////////////////////////////////////////////////////////////////////////

	CustemNotificationStrategy::CustemNotificationStrategy() 
		: ACE_Notification_Strategy(NULL, 0), _event(new ACE_Event())
	{
	}

	CustemNotificationStrategy::~CustemNotificationStrategy()
	{
		SAFE_DELETE(_event)
	}

	int CustemNotificationStrategy::notify( void )
	{
		_event->signal();
		return 0;
	}

	bool CustemNotificationStrategy::Wait(int32 timeoutMs)
	{
		ACE_Time_Value* wait_time = NULL;
		if (-1 != timeoutMs)
		{
			wait_time = new ACE_Time_Value(
				ACE_OS::gettimeofday()+ACE_Time_Value(0, timeoutMs*UsecAdjustValue));
		}
		_event->wait(wait_time);
		return true;
	}

	//////////////////////////////////////////////////////////////////////////

	MessageQueue::MessageQueue() : m_popProgress(FALSE)
	{
	}

	MessageQueue::~MessageQueue()
	{
	}

	bool MessageQueue::Push(uint8 queId, EMessageHeader command, 
								uint32 serial, ACE_Message_Block* block)
	{
		MessagePostee postee;
		postee.stream_id = serial;
		postee.queue_id = queId;
		postee.command = command;
		ACE_Message_Block* commandBlock = NetworkInstance->AllocateBlock(sizeof(MessagePostee));
		ACE_ASSERT(NULL != commandBlock);
		commandBlock->copy((char*)&postee, sizeof(MessagePostee));
		if (NULL != block)
			commandBlock->cont(block);

		return (-1 != m_queue.enqueue_tail(commandBlock));
	}

	bool MessageQueue::Pop(ACE_Message_Block** block, int timeout)
	{
		if (TRUE == m_popProgress.value())
			return false;
		InterlockedGuard guard(m_popProgress);

		ACE_Time_Value* wait_time = NULL;
		if (-1 != timeout)
		{
			wait_time = new ACE_Time_Value(ACE_OS::gettimeofday() + ACE_Time_Value(0, timeout*UsecAdjustValue));
		}

		int result = m_queue.dequeue_head(*block, wait_time);
		SAFE_DELETE(wait_time);
		return (-1 != result);
	}

	bool MessageQueue::PopAll(ACE_Message_Block** block, int timeout)
	{
		if (TRUE == m_popProgress.value())
			return false;
		InterlockedGuard guard(m_popProgress);

		ACE_Time_Value* wait_time = NULL;
		if (-1 != timeout)
		{
			wait_time = new ACE_Time_Value(ACE_OS::gettimeofday() + ACE_Time_Value(0, timeout*UsecAdjustValue));
		}

		ACE_Message_Block* current = NULL;
		ACE_Message_Block* head = NULL;
		while ( -1 != m_queue.dequeue_head(current, wait_time))
		{
			if (NULL == head)
			{
				head = current;
			}
			else
			{
				ACE_Message_Block* tail = FindTailBlock(head);
				tail->cont(current);
			}

			if (m_queue.is_empty())
				break;
		}

		SAFE_DELETE(wait_time);
		if (NULL == head)
			return false;
		*block = head;
		return true;
	}

	void MessageQueue::Pulse()
	{
		m_queue.pulse();
	}

	void MessageQueue::Clear()
	{
		m_queue.flush();
	}

	void MessageQueue::Dump()
	{
		m_queue.dump();
	}

	uint32 MessageQueue::MessageBytes()
	{
		return (uint32)m_queue.message_bytes();
	}

	uint32 MessageQueue::MessageLength()
	{
		return (uint32)m_queue.message_length();
	}

	uint32 MessageQueue::MessageCount()
	{
		return (uint32)m_queue.message_count();
	}

	void MessageQueue::SetEventNotiStrategy( ACE_Notification_Strategy* strategy)
	{
		m_queue.notification_strategy(strategy);
	}

	bool MessageQueue::IsEmpty()
	{
		return m_queue.is_empty();
	}
}