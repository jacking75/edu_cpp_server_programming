#include <string>
#include "HalfNetworkType.h"
#include "MessageQueueRepository.h"
#include "MessageBlockUtil.h"

namespace HalfNetwork
{
	MessageQueueRepository::MessageQueueRepository() : m_popProgress(FALSE), m_suspendPush(FALSE)
	{
	}

	bool MessageQueueRepository::Open()
	{
		return true;
	}

	void MessageQueueRepository::Close()
	{
		PulseAll();
		DeleteAll();
		m_queueContainer.unbind_all();
	}

	bool MessageQueueRepository::CreateQueue(uint8 id)
	{
		MessageQueue* que = GetQueue(id);
		if ( NULL != que )
			return false;

		MessageQueue* queue = new MessageQueue();
		queue->SetEventNotiStrategy(&_eventNotiStrategy);
		m_queueContainer.bind(id, queue);
		return true;
	}

	MessageQueue* MessageQueueRepository::GetQueue(uint8 id) const
	{
		MessageQueue* que = NULL;
		if ( -1 == m_queueContainer.find(id, que) )
			return NULL;
		return que;
	}

	bool MessageQueueRepository::Push( uint8 queId, 
									EMessageHeader command, 
									uint32 serial, 
									ACE_Message_Block* block)
	{
		if (TRUE == m_suspendPush.value())
			return false;

		MessageQueue* que = GetQueue(queId);
		if (NULL == que)
			return false;
		
		return que->Push(queId, command, serial, block);
	}

	bool MessageQueueRepository::Pop(uint8 queId, ACE_Message_Block** block, int timeout)
	{
		MessageQueue* que = GetQueue(queId);
		if (NULL == que)
			return false;

		return que->Pop(block, timeout);
	}

	bool MessageQueueRepository::Pop(ACE_Message_Block** block, int timeout)
	{
		if (IsAllEmpty())
			_eventNotiStrategy.Wait(timeout);

		if (TRUE == m_popProgress.value())
			return false;
		InterlockedGuard guard(m_popProgress);

		int timeoutValue = timeout;
		if (timeoutValue <= -1)
			timeoutValue = 0;
		MessageQueueMap::iterator iter(m_queueContainer);
		for (MessageQueueMap::iterator iter = m_queueContainer.begin(); iter != m_queueContainer.end(); iter++)
		{
			MessageQueue* mq = (*iter).int_id_;
			if ( mq->Pop(block, timeoutValue) )
				return true;
		}
		return false;
	}

	bool MessageQueueRepository::PopAll(uint8 que_id, ACE_Message_Block** block, int timeout)
	{
		MessageQueue* que = GetQueue(que_id);
		if (NULL == que)
			return false;

		return que->PopAll(block, timeout);
	}

	bool MessageQueueRepository::PopAll(ACE_Message_Block** block, int timeout)
	{
		if (IsAllEmpty())
			_eventNotiStrategy.Wait(timeout);

		if (TRUE == m_popProgress.value())
			return false;
		InterlockedGuard guard(m_popProgress);

		return PopAllImpl(block);
	}

	bool MessageQueueRepository::PopAllImpl( ACE_Message_Block** block )
	{
		ACE_Message_Block* current = NULL;
		ACE_Message_Block* head = NULL;
		MessageQueueMap::iterator iter(m_queueContainer);
		for (MessageQueueMap::iterator iter = m_queueContainer.begin(); iter != m_queueContainer.end(); iter++)
		{
			MessageQueue* mq = (*iter).int_id_;
			if (false == mq->PopAll(&current, 0))
				continue;
			if (NULL == head)
			{
				head = current;
			}
			else
			{
				ACE_Message_Block* tail = FindTailBlock(head);
				tail->cont(current);
			}			
		}
		if (NULL == head)
			return false;

		*block = head;
		return true;
	}

	bool MessageQueueRepository::IsAllEmpty()
	{
		MessageQueueMap::iterator iter(m_queueContainer);
		for (MessageQueueMap::iterator iter = m_queueContainer.begin(); iter != m_queueContainer.end(); iter++)
		{
			MessageQueue* mq = (*iter).int_id_;
			if (false == mq->IsEmpty())
				return false;
		}
		return true;
	}

	bool MessageQueueRepository::ExistQueueID(uint8 queId) const
	{
		MessageQueue* que = GetQueue(queId);
		return (NULL != que);
	}

	void MessageQueueRepository::SuspendPush()
	{
		m_suspendPush = TRUE;
	}

	void MessageQueueRepository::Dump()
	{
		//HALF_DEBUG("Queue Count(%d).\n", m_queueContainer.current_size());
		MessageQueueMap::iterator end_iter = m_queueContainer.end();
		for (MessageQueueMap::iterator iter = m_queueContainer.begin(); iter != end_iter; iter++)
		{
			MessageQueue* que = (*iter).int_id_;
			//HALF_DEBUG("ID(%d) Byte(%d) Length(%d) Count(%d)\n", 
			//	(*iter).ext_id_, 
			//	que->MessageBytes(),
			//	que->MessageLength(), 
			//	que->MessageCount());
			que->Dump();
		}
	}

	void MessageQueueRepository::PulseAll() 
	{
		MessageQueueMap::iterator iter(m_queueContainer);
		MessageQueueMap::iterator end_iter = m_queueContainer.end();
		for (MessageQueueMap::iterator iter = m_queueContainer.begin(); iter != end_iter; iter++)
		{
			(*iter).int_id_->Pulse();
			//(*iter).int_id_->Clear();
		}
		_eventNotiStrategy.notify();
	}

	void MessageQueueRepository::ClearAll()
	{
		MessageQueueMap::iterator iter(m_queueContainer);
		MessageQueueMap::iterator end_iter = m_queueContainer.end();
		for (MessageQueueMap::iterator iter = m_queueContainer.begin(); iter != end_iter; iter++)
		{
			(*iter).int_id_->Clear();
		}
	}

	void MessageQueueRepository::DeleteAll()
	{
		MessageQueueMap::iterator end_iter = m_queueContainer.end();
		for (MessageQueueMap::iterator iter = m_queueContainer.begin(); iter != end_iter; iter++)
		{
			SAFE_DELETE((*iter).int_id_);
		}
	}

	void MessageQueueRepository::Pulse(const uint8 queId) const
	{
		MessageQueue* que = GetQueue(queId);
		if (NULL == que)
			return;
		que->Pulse();
	}
}