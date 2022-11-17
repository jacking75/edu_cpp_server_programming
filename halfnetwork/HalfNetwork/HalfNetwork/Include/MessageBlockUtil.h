#ifndef __messageblockutil_h__
#define __messageblockutil_h__

#pragma once

#include <ace/Message_Block.h>

namespace HalfNetwork
{
	inline ACE_Message_Block *FindTailBlock(ACE_Message_Block *block)
	{
		ACE_Message_Block *tail = block;
		while (NULL != tail->cont())
			tail = tail->cont();
		return tail;
	}

	inline bool MakeMergedBlock(ACE_Message_Block *blockList, 
															ACE_Message_Block *mergedBlock)
	{
		if (mergedBlock->capacity() < blockList->total_length())
			return false;
		ACE_Message_Block* block = blockList;
		do 
		{
			mergedBlock->copy(block->rd_ptr(), block->length());
		} while (NULL != (block = block->cont()));
		return true;
	}
}

#endif // __messageblockutil_h__