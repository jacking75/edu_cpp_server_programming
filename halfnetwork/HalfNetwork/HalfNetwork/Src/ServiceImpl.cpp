#include <string>
#include <ace/Singleton.h>
#include "HalfNetworkType.h"
#include "ServiceImpl.h"
#include "NetworkFacade.h"
#include "TimerUtil.h"
#include "MessageBlockUtil.h"
#include "InterlockedValue.h"

namespace HalfNetwork
{

	const int MaxDifferenceTick = 1000*60*60*24; // a day

	ServiceImpl::ServiceImpl() 
		: _block_queue(new ACE_Message_Queue<ACE_MT_SYNCH>)
		,	_closeFlag(new InterlockedValue((long)eCF_None))
		,	_timerLock(new InterlockedValue())
		,	_sendLock(new InterlockedValue())
		,	_lastReceivedTick(GetTick())
	{
		size_t QueueHighWaterMark = 1024*32*10;
		_block_queue->high_water_mark(QueueHighWaterMark);
	}

	ServiceImpl::~ServiceImpl()
	{
		_block_queue->close();
		SAFE_DELETE(_block_queue);
		SAFE_DELETE(_closeFlag);
		SAFE_DELETE(_timerLock);
		SAFE_DELETE(_sendLock);
	}

	bool ServiceImpl::PushQueue(ACE_Message_Block* block, uint32 tick)
	{
		block->msg_priority(tick);
		ACE_Time_Value noWait(0, 0); // I don't want to wait here.
		return (-1 != _block_queue->enqueue_prio(block, &noWait));
	}

	bool ServiceImpl::PopQueue(ACE_Message_Block** param_block)
	{
		ACE_Time_Value noWait(0, 0);
		uint32 current_tick = GetTick();
		ACE_Message_Block* block = NULL;
		uint32 block_count = 0;
		while(true)
		{
			if (-1 == _block_queue->dequeue_prio(block, &noWait))
				break;

			int difference = current_tick - block->msg_priority();
			//if (difference < 0)
			if (difference < 0 && difference > MaxDifferenceTick*-1 )
			{
				_block_queue->enqueue_prio(block, &noWait);
				break;
			}

			if (++block_count <= 1)
			{
				*param_block = block;
				continue;
			}

			ACE_Message_Block* tail_block = FindTailBlock(*param_block);
			if (NULL != tail_block)
				tail_block->cont(block);

			if (block_count >= ACE_IOV_MAX)
			{
				//break;
			}
		}

		if (0 == block_count)
			return false;
		return true;
	}

	void ServiceImpl::PushEventBlock( int8 eventType, 
																		uint8 queueID, 
																		uint32 serial, 
																		ACE_Message_Block* block )
	{
		_lastReceivedTick = GetTick();
		NetworkInstance->PushMessage(queueID, eventType, serial, block);
	}

	ACE_Message_Block* ServiceImpl::AllocateBlock( size_t size )
	{
		return NetworkInstance->AllocateBlock(size);
	}

	int8 ServiceImpl::GetCloseFlag()
	{
		if (_closeFlag->Compare(eCF_Passive))
			return eCF_Passive;
		else if (_closeFlag->Compare(eCF_Active))
			return eCF_Active;
		else if (_closeFlag->Compare(eCF_Receive))
			return eCF_Receive;
		return eCF_None;
	}

	void ServiceImpl::SetCloseFlag( int8 flag )
	{
		_closeFlag->Exchange(flag);
	}

	bool ServiceImpl::AcquireTimerLock()
	{
		return  _timerLock->Acquire();
	}

	void ServiceImpl::ReleaseTimerLock()
	{
		_timerLock->Release();
	}

	bool ServiceImpl::AcquireSendLock()
	{
		return _sendLock->Acquire();
	}

	void ServiceImpl::ReleaseSendLock()
	{
		_sendLock->Release();
	}

	bool ServiceImpl::TrySendLock()
	{
		return _sendLock->Compare(0);
	}

	bool ServiceImpl::IsZombieConnection()
	{
		if (0 == SystemConfigInst->ZombieConnectionTerm)
			return false;

		if ( (GetTick() - _lastReceivedTick) < SystemConfigInst->ZombieConnectionTerm)
			return false;

		_lastReceivedTick = GetTick();
		return true;
	}

} // namespace HalfNetwork
