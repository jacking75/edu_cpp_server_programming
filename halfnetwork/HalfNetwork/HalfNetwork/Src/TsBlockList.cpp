#include <string>
#include <ace/Message_Block.h>
#include <ace/Guard_T.h>
#include "HalfNetworkType.h"
#include "TsBlockList.h"

#include "MessageBlockUtil.h"

namespace HalfNetwork
{
	TsBlockList::TsBlockList() : _head_block(new ACE_Message_Block)
	{
	}

	TsBlockList::~TsBlockList()
	{
		if (NULL != _head_block)
			_head_block->release();
	}

	void TsBlockList::InsertTail(ACE_Message_Block* block)
	{
		ACE_Guard<ACE_Thread_Mutex> guard(_lock);
		ACE_Message_Block* tail_block = FindTailBlock(_head_block);
		tail_block->cont(block);
	}

	ACE_Message_Block* TsBlockList::Pop()
	{
		ACE_Guard<ACE_Thread_Mutex> guard(_lock);
		ACE_Message_Block* next_block = _head_block->cont();
		if (NULL == next_block)
			return NULL;
		_head_block->cont(NULL);
		return next_block;
	}

	uint32 TsBlockList::TotalLength()
	{
		return (uint32)_head_block->total_length();
	}

} // namespace HalfNetwork
