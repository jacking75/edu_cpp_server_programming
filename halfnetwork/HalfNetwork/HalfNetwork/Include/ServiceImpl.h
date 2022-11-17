#ifndef __serviceimpl_h__
#define __serviceimpl_h__

#pragma once

#include <ace/Thread_Mutex.h>
#include <ace/Message_Queue.h>
#include "MemoryObject.h"

class ACE_Message_Block;

namespace HalfNetwork
{

	class InterlockedValue;

	//////////////////////////////////////////////////////////////////////////
	// Description:
	//   
	//////////////////////////////////////////////////////////////////////////
	class ServiceImpl : public MemoryObject
	{
	public:
		ServiceImpl();
		~ServiceImpl();

	public:
		bool	PushQueue(ACE_Message_Block* block, uint32 tick);
		bool	PopQueue(ACE_Message_Block** param_block);
		int8	GetCloseFlag();
		void	SetCloseFlag(int8 flag);

	public:
		bool	AcquireTimerLock();
		void	ReleaseTimerLock();
		bool	AcquireSendLock();
		void	ReleaseSendLock();
		bool	TrySendLock();
		bool	IsZombieConnection();
	
	public:
		void	PushEventBlock(int8 eventType, 
												 uint8 queueID, 
												 uint32 serial, 
												 ACE_Message_Block* block);

		ACE_Message_Block*	AllocateBlock(size_t size);

	private:
		ACE_Message_Queue<ACE_MT_SYNCH>*	_block_queue;
		InterlockedValue*		_closeFlag;
		InterlockedValue*		_timerLock;
		InterlockedValue*		_sendLock;
		uint32							_lastReceivedTick;
	};

} // namespace HalfNetwork


#endif // __serviceimpl_h__